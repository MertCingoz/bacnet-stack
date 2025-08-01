/**************************************************************************
 *
 * Copyright (C) 2008 Steve Karg
 *
 * SPDX-License-Identifier: GPL-2.0-or-later WITH GCC-exception-2.0
 *
 *********************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
/* BACnet Stack defines - first */
#include "bacnet/bacdef.h"
/* BACnet Stack API */
#include "bacnet/iam.h"
#include "bacnet/version.h"
#include "bacnet/datetime.h"
#include "bacnet/bacstr.h"
/* basic datalink, timer, and filename */
#include "bacnet/datalink/dlmstp.h"
#include "bacnet/basic/sys/mstimer.h"
#include "bacnet/datalink/crc.h"
#include "bacnet/datalink/mstptext.h"
#include "bacnet/basic/sys/filename.h"
/* OS specific includes */
#include "bacport.h"
#include "rs485.h"

/* define our Data Link Type for libPCAP */
#define DLT_BACNET_MS_TP (165)
/* local min/max macros */
#ifndef max
#define max(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b;      \
    })

#define min(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b;      \
    })
#endif

#define MSTP_HEADER_MAX (2 + 1 + 1 + 1 + 2 + 1)

/* local port data - shared with RS-485 */
static struct mstp_port_struct_t MSTP_Port;
/* track the receive state to know when there is a broken packet */
static MSTP_RECEIVE_STATE MSTP_Receive_State = MSTP_RECEIVE_STATE_IDLE;
/* buffers needed by mstp port struct */
static uint8_t RxBuffer[DLMSTP_MPDU_MAX];
static uint8_t TxBuffer[DLMSTP_MPDU_MAX];
/* method to tell main loop to exit from CTRL-C or other signals */
static volatile bool Exit_Requested;
/* flag to indicate Wireshark is running the show - no stdout or stderr */
static bool Wireshark_Capture;
/* placed to track silence on the wire */
static struct mstimer Silence_Timer;

/* statistics derived from monitoring the network for each node */
struct mstp_statistics {
    /* counts how many times the node passes the token */
    uint32_t token_count;
    /* counts how many times the node receives the token */
    uint32_t token_received_count;
    /* counts how many times the node polls for another master */
    uint32_t pfm_count;
    /* counts how many times the node replies to polls from another master */
    uint32_t rpfm_count;
    /* counts how many times the node sends reply postponed */
    uint32_t reply_postponed_count;
    /* counts how many times the node sends a test request */
    uint32_t test_request_count;
    /* counts how many times the node sends a test response */
    uint32_t test_response_count;
    /* counts how many times the node sends a DER frame */
    uint32_t der_count;
    /* counts how many times the node sends a DNER frame */
    uint32_t dner_count;
    /* -- inferred data -- */
    /* counts how many times the node gets a second token */
    uint32_t token_retries;
    /* delay after poll for master */
    uint32_t tusage_timeout;
    /* highest number MAC during poll for master */
    uint8_t max_master;
    /* highest number of frames sent during a token */
    uint8_t max_info_frames;
    /* how long it takes a node to pass the token */
    uint32_t token_reply;
    /* how long it takes a node to reply to PFM */
    uint32_t pfm_reply;
    /* how long it takes a node to reply to DER */
    uint32_t der_reply;
    /* how long it takes a node to send a reply post poned */
    uint32_t reply_postponed;
    /* number of tokens received before a Poll For Master cycle is executed */
    uint32_t npoll;
    /* number of total tokens at the last PFM */
    uint32_t last_pfm_tokens;
    /* Addendum 2008v - sending tokens to myself */
    /* counts how many times the node passes the token */
    uint32_t self_token_count;
    /* counts how many times the node sends the token out-of-order (ooo) */
    uint32_t ooo_token_count;
    /* if we see an I-Am message from this node, store the Device ID */
    uint32_t device_id;
};

#define MAX_MSTP_DEVICES 256
static struct mstp_statistics MSTP_Statistics[MAX_MSTP_DEVICES];
static uint32_t Invalid_Frame_Count;

static uint32_t
timeval_diff_ms(const struct timeval *old, const struct timeval *now)
{
    uint32_t ms = 0;

    /* convert to milliseconds */
    ms = (now->tv_sec - old->tv_sec) * 1000 +
        (now->tv_usec - old->tv_usec) / 1000;

    return ms;
}

static void mstp_monitor_i_am(uint8_t mac, const uint8_t *pdu, uint16_t pdu_len)
{
    BACNET_ADDRESS src = { 0 };
    BACNET_ADDRESS dest = { 0 };
    BACNET_NPDU_DATA npdu_data = { 0 };
    int apdu_offset = 0;
    uint16_t apdu_len = 0;
    const uint8_t *apdu = NULL;
    uint8_t pdu_type = 0;
    uint8_t service_choice = 0;
    const uint8_t *service_request = NULL;
    uint32_t device_id = 0;
    int len = 0;

    if (pdu[0] == BACNET_PROTOCOL_VERSION) {
        MSTP_Fill_BACnet_Address(&src, mac);
        apdu_offset = bacnet_npdu_decode(pdu, pdu_len, &dest, &src, &npdu_data);
        if ((!npdu_data.network_layer_message) && (apdu_offset > 0) &&
            (apdu_offset < pdu_len) && (src.net == 0)) {
            apdu_len = pdu_len - apdu_offset;
            apdu = &pdu[apdu_offset];
            pdu_type = apdu[0] & 0xF0;
            if ((pdu_type == PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST) &&
                (apdu_len >= 2)) {
                service_choice = apdu[1];
                service_request = &apdu[2];
                if (service_choice == SERVICE_UNCONFIRMED_I_AM) {
                    len = iam_decode_service_request(
                        service_request, &device_id, NULL, NULL, NULL);
                    if (len != -1) {
                        MSTP_Statistics[mac].device_id = device_id;
                    }
                }
            }
        }
    }
}

static void packet_statistics(
    const struct timeval *tv, const struct mstp_port_struct_t *mstp_port)
{
    static struct timeval old_tv = { 0 };
    static uint8_t old_frame = 255;
    static uint8_t old_src = 255;
    static uint8_t old_dst = 255;
    static uint8_t old_token_dst = 255;
    uint8_t frame, src, dst;
    uint32_t delta;
    uint32_t npoll;

    dst = mstp_port->DestinationAddress;
    src = mstp_port->SourceAddress;
    frame = mstp_port->FrameType;
    switch (frame) {
        case FRAME_TYPE_TOKEN:
            MSTP_Statistics[src].token_count++;
            MSTP_Statistics[dst].token_received_count++;
            if (src == dst) {
                MSTP_Statistics[src].self_token_count++;
            }
            if (old_frame == FRAME_TYPE_TOKEN) {
                if ((old_dst == dst) && (old_src == src)) {
                    /* repeated token */
                    MSTP_Statistics[dst].token_retries++;
                    /* Tusage_timeout */
                    delta = timeval_diff_ms(&old_tv, tv);
                    if (delta > MSTP_Statistics[src].tusage_timeout) {
                        MSTP_Statistics[src].tusage_timeout = delta;
                    }
                } else if (old_dst == src) {
                    /* token to token response time */
                    delta = timeval_diff_ms(&old_tv, tv);
                    if (delta > MSTP_Statistics[src].token_reply) {
                        MSTP_Statistics[src].token_reply = delta;
                    }
                }
            } else if (
                (old_frame == FRAME_TYPE_POLL_FOR_MASTER) && (old_src == src)) {
                /* Tusage_timeout */
                delta = timeval_diff_ms(&old_tv, tv);
                if (delta > MSTP_Statistics[src].tusage_timeout) {
                    MSTP_Statistics[src].tusage_timeout = delta;
                }
            }
            if (old_token_dst != src) {
                /* out-of-order Token sender */
                MSTP_Statistics[src].ooo_token_count++;
            }
            old_token_dst = dst;
            break;
        case FRAME_TYPE_POLL_FOR_MASTER:
            if (MSTP_Statistics[src].last_pfm_tokens) {
                npoll = MSTP_Statistics[src].token_received_count -
                    MSTP_Statistics[src].last_pfm_tokens;
                if (npoll > MSTP_Statistics[src].npoll) {
                    MSTP_Statistics[src].npoll = npoll;
                }
            }
            MSTP_Statistics[src].last_pfm_tokens =
                MSTP_Statistics[src].token_received_count;
            MSTP_Statistics[src].pfm_count++;
            if (dst > MSTP_Statistics[src].max_master) {
                MSTP_Statistics[src].max_master = dst;
            }
            if ((old_frame == FRAME_TYPE_POLL_FOR_MASTER) && (old_src == src)) {
                /* Tusage_timeout - sole master */
                delta = timeval_diff_ms(&old_tv, tv);
                if (delta > MSTP_Statistics[src].tusage_timeout) {
                    MSTP_Statistics[src].tusage_timeout = delta;
                }
            }
            break;
        case FRAME_TYPE_REPLY_TO_POLL_FOR_MASTER:
            MSTP_Statistics[src].rpfm_count++;
            if (old_frame == FRAME_TYPE_POLL_FOR_MASTER) {
                delta = timeval_diff_ms(&old_tv, tv);
                if (delta > MSTP_Statistics[src].pfm_reply) {
                    MSTP_Statistics[src].pfm_reply = delta;
                }
            }
            break;
        case FRAME_TYPE_TEST_REQUEST:
            MSTP_Statistics[src].test_request_count++;
            break;
        case FRAME_TYPE_TEST_RESPONSE:
            MSTP_Statistics[src].test_response_count++;
            break;
        case FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY:
            MSTP_Statistics[src].der_count++;
            break;
        case FRAME_TYPE_BACNET_DATA_NOT_EXPECTING_REPLY:
            MSTP_Statistics[src].dner_count++;
            if ((old_frame == FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY) &&
                (old_dst == src)) {
                /* DER response time */
                delta = timeval_diff_ms(&old_tv, tv);
                if (delta > MSTP_Statistics[src].der_reply) {
                    MSTP_Statistics[src].der_reply = delta;
                }
            }
            if (mstp_port->ReceivedValidFrame) {
                if ((mstp_port->DataLength <= mstp_port->InputBufferSize) &&
                    (mstp_port->DataLength > 0)) {
                    mstp_monitor_i_am(
                        src, &mstp_port->InputBuffer[0], mstp_port->DataLength);
                }
            }
            break;
        case FRAME_TYPE_REPLY_POSTPONED:
            MSTP_Statistics[src].reply_postponed_count++;
            if ((old_frame == FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY) &&
                (old_dst == src)) {
                /* Postponed response time */
                delta = timeval_diff_ms(&old_tv, tv);
                if (delta > MSTP_Statistics[src].reply_postponed) {
                    MSTP_Statistics[src].reply_postponed = delta;
                }
            }
            break;
        default:
            break;
    }

    /* update the old variables */
    old_dst = dst;
    old_src = src;
    old_frame = frame;
    old_tv.tv_sec = tv->tv_sec;
    old_tv.tv_usec = tv->tv_usec;
}

static void packet_statistics_print(void)
{
    unsigned i; /* loop counter */
    unsigned node_count = 0;
    long unsigned int self_or_ooo_count;

    fprintf(stdout, "\n");
    fprintf(stdout, "==== MS/TP Frame Counts ====\n");
    fprintf(
        stdout, "%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-7s", "MAC", "Device",
        "Tokens", "PFM", "RPFM", "DER", "Postpd", "DNER", "TestReq", "TestRsp");
    fprintf(stdout, "\n");
    for (i = 0; i < MAX_MSTP_DEVICES; i++) {
        /* check for masters or slaves */
        if ((MSTP_Statistics[i].token_count) ||
            (MSTP_Statistics[i].der_reply) || (MSTP_Statistics[i].pfm_count)) {
            node_count++;
            fprintf(stdout, "%-8u", i);
            if (MSTP_Statistics[i].device_id <= 4194303) {
                fprintf(
                    stdout, "%-8lu",
                    (long unsigned int)MSTP_Statistics[i].device_id);
            } else {
                fprintf(stdout, "%-8s", "-");
            }
            fprintf(
                stdout, "%-8lu%-8lu%-8lu%-8lu",
                (long unsigned int)MSTP_Statistics[i].token_count,
                (long unsigned int)MSTP_Statistics[i].pfm_count,
                (long unsigned int)MSTP_Statistics[i].rpfm_count,
                (long unsigned int)MSTP_Statistics[i].der_count);
            fprintf(
                stdout, "%-8lu%-8lu%-8lu%-7lu",
                (long unsigned int)MSTP_Statistics[i].reply_postponed_count,
                (long unsigned int)MSTP_Statistics[i].dner_count,
                (long unsigned int)MSTP_Statistics[i].test_request_count,
                (long unsigned int)MSTP_Statistics[i].test_response_count);
            fprintf(stdout, "\n");
        }
    }
    fprintf(stdout, "Node Count: %u\n", node_count);
    node_count = 0;
    fprintf(stdout, "\n");
    fprintf(stdout, "==== MS/TP Usage and Timing Maximums ====\n");
    fprintf(
        stdout, "%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-7s", "MAC", "MaxMstr",
        "Retries", "Npoll", "Self/TT", "Treply", "Tusage", "Trpfm", "Tder",
        "Tpostpd");
    fprintf(stdout, "\n");
    for (i = 0; i < MAX_MSTP_DEVICES; i++) {
        /* check for masters or slaves */
        if ((MSTP_Statistics[i].token_count) ||
            (MSTP_Statistics[i].der_reply) || (MSTP_Statistics[i].pfm_count)) {
            node_count++;
            self_or_ooo_count = MSTP_Statistics[i].self_token_count +
                MSTP_Statistics[i].ooo_token_count;
            fprintf(stdout, "%-8u", i);
            fprintf(
                stdout, "%-8lu%-8lu%-8lu%-8lu%-8lu",
                (long unsigned int)MSTP_Statistics[i].max_master,
                (long unsigned int)MSTP_Statistics[i].token_retries,
                (long unsigned int)MSTP_Statistics[i].npoll, self_or_ooo_count,
                (long unsigned int)MSTP_Statistics[i].token_reply);
            fprintf(
                stdout, "%-8lu%-8lu%-8lu%-7lu",
                (long unsigned int)MSTP_Statistics[i].tusage_timeout,
                (long unsigned int)MSTP_Statistics[i].pfm_reply,
                (long unsigned int)MSTP_Statistics[i].der_reply,
                (long unsigned int)MSTP_Statistics[i].reply_postponed);
            fprintf(stdout, "\n");
        }
    }
    fprintf(stdout, "Node Count: %u\n", node_count);
    fprintf(
        stdout, "Invalid Frame Count: %lu\n",
        (long unsigned int)Invalid_Frame_Count);
    fflush(stdout);
}

static void packet_statistics_clear(void)
{
    unsigned i = 0;

    memset(&MSTP_Statistics[0], 0, sizeof(MSTP_Statistics));
    for (i = 0; i < MAX_MSTP_DEVICES; i++) {
        MSTP_Statistics[i].device_id = 0xFFFFFFFF;
    }
    Invalid_Frame_Count = 0;
}

static uint32_t Timer_Silence(void *pArg)
{
    (void)pArg;
    return mstimer_elapsed(&Silence_Timer);
}

static void Timer_Silence_Reset(void *pArg)
{
    (void)pArg;
    mstimer_set(&Silence_Timer, 0);
}

/* functions used by the MS/TP state machine to put or get data */
uint16_t MSTP_Put_Receive(struct mstp_port_struct_t *mstp_port)
{
    (void)mstp_port;

    return 0;
}

/* for the MS/TP state machine to use for getting data to send */
/* Return: amount of PDU data */
uint16_t MSTP_Get_Send(struct mstp_port_struct_t *mstp_port, unsigned timeout)
{ /* milliseconds to wait for a packet */
    (void)mstp_port;
    (void)timeout;
    return 0;
}

/**
 * @brief Send an MSTP frame
 * @param mstp_port - port specific data
 * @param buffer - data to send
 * @param nbytes - number of bytes of data to send
 */
void MSTP_Send_Frame(
    struct mstp_port_struct_t *mstp_port,
    const uint8_t *buffer,
    uint16_t nbytes)
{
    (void)mstp_port;
    (void)buffer;
    (void)nbytes;
}

uint16_t MSTP_Get_Reply(struct mstp_port_struct_t *mstp_port, unsigned timeout)
{ /* milliseconds to wait for a packet */
    (void)mstp_port;
    (void)timeout;
    return 0;
}

static char Capture_Filename[64] = "mstp_20090123091200.cap";
static FILE *File_Handle = NULL; /* stream pointer */
#if defined(_WIN32)
static HANDLE Pipe_Handle = INVALID_HANDLE_VALUE; /* pipe handle */
static void named_pipe_create(const char *pipe_name)
{
    if (!Wireshark_Capture) {
        fprintf(stdout, "mstpcap: Creating Named Pipe \"%s\"\n", pipe_name);
    }
    /* create the pipe */
    while (Pipe_Handle == INVALID_HANDLE_VALUE) {
        /* use CreateFile rather than CreateNamedPipe */
        Pipe_Handle = CreateFile(
            pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0,
            NULL);
        if (Pipe_Handle != INVALID_HANDLE_VALUE) {
            break;
        }
        /* if an error occured at handle creation */
        if (!WaitNamedPipe(pipe_name, 20000)) {
            printf("Could not open pipe: waited for 20sec!\n"
                   "If this message was issued before the 20sec finished,\n"
                   "then the pipe doesn't exist!\n");
            Exit_Requested = true;
            return;
        }
    }
    ConnectNamedPipe(Pipe_Handle, NULL);
}

static size_t data_write(const void *ptr, size_t size, size_t nitems)
{
    size_t written = 0;
    DWORD cbWritten = 0;

    if (Pipe_Handle != INVALID_HANDLE_VALUE) {
        (void)WriteFile(
            Pipe_Handle, /* handle to pipe  */
            ptr, /* buffer to write from  */
            size * nitems, /* number of bytes to write  */
            &cbWritten, /* number of bytes written  */
            NULL); /* not overlapped I/O  */
        written = cbWritten;
    }

    if (File_Handle) {
        written = fwrite(ptr, size, nitems, File_Handle);
    }

    return written;
}

static size_t data_write_header(const void *ptr, size_t size, size_t nitems)
{
    size_t written = 0;
    DWORD cbWritten = 0;
    if (Pipe_Handle != INVALID_HANDLE_VALUE) {
        (void)WriteFile(
            Pipe_Handle, /* handle to pipe  */
            ptr, /* buffer to write from  */
            size * nitems, /* number of bytes to write  */
            &cbWritten, /* number of bytes written  */
            NULL); /* not overlapped I/O  */
        written = cbWritten;
    }
    if (File_Handle) {
        written = fwrite(ptr, size, nitems, File_Handle);
    }

    return written;
}
#else
static int FD_Pipe = -1;
static void named_pipe_create(const char *name)
{
    int rv = 0;
    rv = mkfifo(name, 0666);
    if ((rv == -1) && (errno != EEXIST)) {
        perror("Error creating named pipe");
        exit(1);
    }
    FD_Pipe = open(name, O_WRONLY);
    if (FD_Pipe == -1) {
        perror("Error connecting to named pipe");
        exit(1);
    }
}

static size_t data_write(const void *ptr, size_t size, size_t nitems)
{
    ssize_t written = 0;

    if (FD_Pipe != -1) {
        written = write(FD_Pipe, ptr, size * nitems);
    }
    if (File_Handle) {
        written = fwrite(ptr, size, nitems, File_Handle);
    }

    return written;
}

static size_t data_write_header(const void *ptr, size_t size, size_t nitems)
{
    ssize_t written = 0;

    if (FD_Pipe != -1) {
        written = write(FD_Pipe, ptr, size * nitems);
    }
    if (File_Handle) {
        written = fwrite(ptr, size, nitems, File_Handle);
    }

    return written;
}
#endif

static void filename_create_new(void)
{
    BACNET_DATE bdate;
    BACNET_TIME btime;
    char *filename = &Capture_Filename[0];
    size_t filename_size = sizeof(Capture_Filename);

    if (Wireshark_Capture) {
        return;
    }
    if (File_Handle) {
        fclose(File_Handle);
    }
    File_Handle = NULL;
    datetime_local(&bdate, &btime, NULL, NULL);
    snprintf(
        filename, filename_size, "mstp_%04d%02d%02d%02d%02d%02d.cap",
        (int)bdate.year, (int)bdate.month, (int)bdate.day, (int)btime.hour,
        (int)btime.min, (int)btime.sec);
    File_Handle = fopen(filename, "wb");
    if (File_Handle) {
        fprintf(stdout, "mstpcap: saving capture to %s\n", filename);
    } else {
        fprintf(
            stderr, "mstpcap: failed to open %s: %s\n", filename,
            strerror(errno));
    }
}

/* write packet to file in libpcap format */
static void write_global_header(void)
{
    uint32_t magic_number = 0xa1b2c3d4; /* magic number */
    uint16_t version_major = 2; /* major version number */
    uint16_t version_minor = 4; /* minor version number */
    int32_t thiszone = 0; /* GMT to local correction */
    uint32_t sigfigs = 0; /* accuracy of timestamps */
    uint32_t snaplen = 65535; /* max length of captured packets, in octets */
    uint32_t network = DLT_BACNET_MS_TP; /* data link type - BACNET_MS_TP */

    /* create a new file. */
    (void)data_write_header(&magic_number, sizeof(magic_number), 1);
    (void)data_write_header(&version_major, sizeof(version_major), 1);
    (void)data_write_header(&version_minor, sizeof(version_minor), 1);
    (void)data_write_header(&thiszone, sizeof(thiszone), 1);
    (void)data_write_header(&sigfigs, sizeof(sigfigs), 1);
    (void)data_write_header(&snaplen, sizeof(snaplen), 1);
    (void)data_write_header(&network, sizeof(network), 1);
    fflush(File_Handle);
}

static void
write_received_packet(struct mstp_port_struct_t *mstp_port, size_t header_len)
{
    uint32_t ts_sec = 0; /* timestamp seconds */
    uint32_t ts_usec = 0; /* timestamp microseconds */
    uint32_t incl_len = 0; /* number of octets of packet saved in file */
    uint32_t orig_len = 0; /* actual length of packet */
    uint32_t data_crc_len = 2;
    uint8_t header[MSTP_HEADER_MAX] = { 0 }; /* MS/TP header */
    struct timeval tv;
    size_t max_data = 0;

    gettimeofday(&tv, NULL);
    ts_sec = tv.tv_sec;
    ts_usec = tv.tv_usec;
    if (mstp_port->ReceivedValidFrame) {
        packet_statistics(&tv, mstp_port);
    }
    (void)data_write(&ts_sec, sizeof(ts_sec), 1);
    (void)data_write(&ts_usec, sizeof(ts_usec), 1);
    if (mstp_port->ReceivedInvalidFrame) {
        if (mstp_port->Index) {
            max_data = min(mstp_port->InputBufferSize, mstp_port->Index);
            if ((mstp_port->DataLength > 0) &&
                (mstp_port->Index == (mstp_port->DataLength + 1))) {
                /* case where index is not incremented for CRC2,
                    so only 1 for checksum */
                data_crc_len = 1;
            }
            incl_len = orig_len = header_len + max_data + data_crc_len;
        } else {
            /* header only */
            incl_len = orig_len = header_len;
        }
    } else {
        if (mstp_port->DataLength) {
            max_data = min(mstp_port->InputBufferSize, mstp_port->DataLength);
            incl_len = orig_len = header_len + max_data + data_crc_len;
        } else {
            /* header only - or at least some bytes of the header */
            incl_len = orig_len = header_len;
        }
    }
    (void)data_write(&incl_len, sizeof(incl_len), 1);
    (void)data_write(&orig_len, sizeof(orig_len), 1);
    if (header_len == 1) {
        header[0] = mstp_port->DataRegister;
    } else if (header_len == 2) {
        header[0] = 0x55;
        header[1] = mstp_port->DataRegister;
    } else {
        header[0] = 0x55;
        header[1] = 0xFF;
        header[2] = mstp_port->FrameType;
        header[3] = mstp_port->DestinationAddress;
        header[4] = mstp_port->SourceAddress;
        header[5] = HI_BYTE(mstp_port->DataLength);
        header[6] = LO_BYTE(mstp_port->DataLength);
        header[7] = mstp_port->HeaderCRCActual;
    }
    (void)data_write(header, header_len, 1);
    if (max_data) {
        (void)data_write(mstp_port->InputBuffer, max_data, 1);
        (void)data_write((char *)&mstp_port->DataCRCActualMSB, 1, 1);
        (void)data_write((char *)&mstp_port->DataCRCActualLSB, 1, 1);
    }
}

/* read header from file in libpcap format */
static bool test_global_header(const char *filename)
{
    uint32_t magic_number = 0; /* magic number */
    uint16_t version_major = 0; /* major version number */
    uint16_t version_minor = 0; /* minor version number */
    int32_t thiszone = 0; /* GMT to local correction */
    uint32_t sigfigs = 0; /* accuracy of timestamps */
    uint32_t snaplen = 0; /* max length of captured packets, in octets */
    uint32_t network = 0; /* data link type - BACNET_MS_TP */
    size_t count = 0;

    /* open existing file. */
    File_Handle = fopen(filename, "rb");
    if (File_Handle) {
        count = fread(&magic_number, sizeof(magic_number), 1, File_Handle);
        if ((count != 1) || (magic_number != 0xa1b2c3d4)) {
            fprintf(stderr, "mstpcap: invalid magic number\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&version_major, sizeof(version_major), 1, File_Handle);
        if ((count != 1) || (version_major != 2)) {
            fprintf(stderr, "mstpcap: invalid major version\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&version_minor, sizeof(version_minor), 1, File_Handle);
        if ((count != 1) || (version_minor != 4)) {
            fprintf(stderr, "mstpcap: invalid minor version\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&thiszone, sizeof(thiszone), 1, File_Handle);
        if ((count != 1) || (thiszone != 0)) {
            fprintf(stderr, "mstpcap: invalid time zone\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&sigfigs, sizeof(sigfigs), 1, File_Handle);
        if ((count != 1) || (sigfigs != 0)) {
            fprintf(stderr, "mstpcap: invalid time stamp accuracy\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&snaplen, sizeof(snaplen), 1, File_Handle);
        if (count != 1) {
            fprintf(stderr, "mstpcap: unable to read SNAP length\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&network, sizeof(network), 1, File_Handle);
        if ((count != 1) || (network != DLT_BACNET_MS_TP)) {
            fprintf(stderr, "mstpcap: invalid data link type (DLT)\n");
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
    } else {
        fprintf(
            stderr, "mstpcap[scan]: failed to open %s: %s\n", filename,
            strerror(errno));
        return false;
    }

    return true;
}

static bool read_received_packet(struct mstp_port_struct_t *mstp_port)
{
    uint32_t ts_sec = 0; /* timestamp seconds */
    uint32_t ts_usec = 0; /* timestamp microseconds */
    uint32_t incl_len = 0; /* number of octets of packet saved in file */
    uint32_t orig_len = 0; /* actual length of packet */
    uint8_t header[8] = { 0 }; /* MS/TP header */
    struct timeval tv;
    size_t count = 0;
    unsigned i = 0;

    if (File_Handle) {
        count = fread(&ts_sec, sizeof(ts_sec), 1, File_Handle);
        if (count != 1) {
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&ts_usec, sizeof(ts_usec), 1, File_Handle);
        if (count != 1) {
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        tv.tv_sec = ts_sec;
        tv.tv_usec = ts_usec;
        count = fread(&incl_len, sizeof(incl_len), 1, File_Handle);
        if (count != 1) {
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&orig_len, sizeof(orig_len), 1, File_Handle);
        if (count != 1) {
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        count = fread(&header, sizeof(header), 1, File_Handle);
        if (count != 1) {
            fclose(File_Handle);
            File_Handle = NULL;
            return false;
        }
        mstp_port->FrameType = header[2];
        mstp_port->DestinationAddress = header[3];
        mstp_port->SourceAddress = header[4];
        mstp_port->DataLength = MAKE_WORD(header[6], header[5]);
        mstp_port->HeaderCRCActual = header[7];
        mstp_port->HeaderCRC = 0xFF;
        for (i = 2; i < 8; i++) {
            mstp_port->HeaderCRC =
                CRC_Calc_Header(header[i], mstp_port->HeaderCRC);
        }
        if (mstp_port->HeaderCRC == 0x55) {
            mstp_port->ReceivedValidFrame = true;
            mstp_port->ReceivedInvalidFrame = false;
            if (mstp_port->DataLength == 0) {
                mstp_port->ReceivedValidFrame = true;
            }
        } else {
            mstp_port->ReceivedValidFrame = false;
            mstp_port->ReceivedInvalidFrame = true;
        }
        if (orig_len > 8) {
            /* packet includes data */
            mstp_port->DataLength = orig_len - 8 - 2;
            count = fread(
                mstp_port->InputBuffer, mstp_port->DataLength, 1, File_Handle);
            if (count != 1) {
                fclose(File_Handle);
                File_Handle = NULL;
                return false;
            }
            count =
                fread((char *)&mstp_port->DataCRCActualMSB, 1, 1, File_Handle);
            if (count != 1) {
                fclose(File_Handle);
                File_Handle = NULL;
                return false;
            }
            count =
                fread((char *)&mstp_port->DataCRCActualLSB, 1, 1, File_Handle);
            if (count != 1) {
                fclose(File_Handle);
                File_Handle = NULL;
                return false;
            }
            mstp_port->DataCRC = 0xFFFF;
            for (i = 0; i < mstp_port->DataLength; i++) {
                mstp_port->DataCRC = CRC_Calc_Data(
                    mstp_port->InputBuffer[i], mstp_port->DataCRC);
            }
            mstp_port->DataCRC =
                CRC_Calc_Data(mstp_port->DataCRCActualMSB, mstp_port->DataCRC);
            mstp_port->DataCRC =
                CRC_Calc_Data(mstp_port->DataCRCActualLSB, mstp_port->DataCRC);
            if (mstp_port->DataCRC == 0xF0B8) {
                mstp_port->ReceivedInvalidFrame = false;
                mstp_port->ReceivedValidFrame = true;
            } else {
                mstp_port->ReceivedInvalidFrame = true;
                mstp_port->ReceivedValidFrame = false;
            }
        } else {
            mstp_port->DataLength = 0;
        }
        if (mstp_port->ReceivedInvalidFrame) {
            Invalid_Frame_Count++;
        } else if (mstp_port->ReceivedValidFrame) {
            packet_statistics(&tv, mstp_port);
        }
    } else {
        return false;
    }

    return true;
}

static void cleanup(void)
{
    if (!Wireshark_Capture) {
        packet_statistics_print();
    }
    if (File_Handle) {
        fflush(File_Handle); /* stream pointer */
        fclose(File_Handle); /* stream pointer */
    }
    File_Handle = NULL;
}

#if defined(_WIN32)
static BOOL WINAPI CtrlCHandler(DWORD dwCtrlType)
{
    dwCtrlType = dwCtrlType;

    if (Pipe_Handle != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(Pipe_Handle);
        DisconnectNamedPipe(Pipe_Handle);
        CloseHandle(Pipe_Handle);
        Pipe_Handle = INVALID_HANDLE_VALUE;
    }
    /* signal to main loop to exit */
    Exit_Requested = true;
    while (Exit_Requested) {
        Sleep(100);
    }
    exit(0);
}
#else
static void sig_int(int signo)
{
    (void)signo;
    if (FD_Pipe != -1) {
        close(FD_Pipe);
    }
    Exit_Requested = true;
    exit(0);
}

static void signal_init(void)
{
    signal(SIGINT, sig_int);
    signal(SIGHUP, sig_int);
    signal(SIGTERM, sig_int);
}
#endif

static void print_usage(const char *filename)
{
    printf("Usage: %s", filename);
    printf(" [--scan <filename>]\n");
    printf(" [--extcap-interface port]\n");
    printf(" [--extcap-interfaces][--extcap-dlts][--extcap-config]\n");
    printf(" [--capture][--baud baud][--fifo pipe]\n");
    printf(" [--version][--help]\n");
}

static void print_help(const char *filename)
{
    printf(
        "%s --scan <filename>\n"
        "perform statistic analysis on MS/TP capture file.\n",
        filename);
    printf("\n");
    printf("Captures MS/TP packets from a serial interface\n"
           "and writes them to a file or a pipe, or scans a file for stats."
           "Filename is of the form mstp_20090123091200.cap (timestamp).\n"
           "New files are created after receiving 65535 packets.\n");
    printf("\n");
    printf("Command line options:\n"
           "[--extcap-interface port] - serial interface.\n"
#if defined(_WIN32)
           "    Supported values: COM1, COM2, etc.\n"
#else
           "    Supported values: /dev/ttyS0, /dev/ttyUSB0, etc.\n"
#endif
           "[--baud baud] - MS/TP port baud rate.\n"
           "    Supported values: 9600, 19200, 38400, 57600, 76800, 115200.\n"
           "    Defaults to 38400.\n"
           "[--fifo pipe] - FIFO pipe path and name\n"
#if defined(_WIN32)
           "    Supported values: \\\\.\\pipe\\wireshark\n"
#else
           "    Supported values: any file name\n"
#endif
           "    Use that name as the interface name in Wireshark.\n");
    printf("\n");
    printf(
        "%s [--extcap-interfaces][--extcap-dlts][--extcap-config]\n"
        "[--capture][--baud baud][--fifo pipe]\n"
        "[--extcap-interface iface]\n"
        "Usage from Wireshark ExtCap interface\n",
        filename);
}

/* initialize some of the variables in the MS/TP Receive structure
   after each packet write  */
static void mstp_structure_init(struct mstp_port_struct_t *mstp_port)
{
    if (mstp_port) {
        mstp_port->FrameType = FRAME_TYPE_PROPRIETARY_MAX;
        mstp_port->DestinationAddress = MSTP_BROADCAST_ADDRESS;
        mstp_port->SourceAddress = MSTP_BROADCAST_ADDRESS;
        mstp_port->DataLength = 0;
        mstp_port->HeaderCRCActual = 0;
        mstp_port->Index = 0;
        mstp_port->EventCount = 0;
        mstp_port->DataRegister = 0xFF;
        mstp_port->ReceivedInvalidFrame = false;
        mstp_port->ReceivedValidFrame = false;
        mstp_port->ReceivedValidFrameNotForUs = false;
        mstp_port->receive_state = MSTP_RECEIVE_STATE_IDLE;
        mstp_port->SilenceTimerReset(NULL);
    }
}

/* simple test to packetize the data and print it */
int main(int argc, char *argv[])
{
    struct mstp_port_struct_t *mstp_port;
    long my_baud = 38400;
    uint32_t packet_count = 0;
    uint32_t header_len = 0;
    int argi = 0;
    const char *filename = NULL;

    MSTP_Port.InputBuffer = &RxBuffer[0];
    MSTP_Port.InputBufferSize = sizeof(RxBuffer);
    MSTP_Port.OutputBuffer = &TxBuffer[0];
    MSTP_Port.OutputBufferSize = sizeof(TxBuffer);
    MSTP_Port.This_Station = MSTP_BROADCAST_ADDRESS;
    MSTP_Port.Nmax_info_frames = 1;
    MSTP_Port.Nmax_master = 127;
    MSTP_Port.SilenceTimer = Timer_Silence;
    MSTP_Port.SilenceTimerReset = Timer_Silence_Reset;
    /* mimic our pointer in the state machine */
    mstp_port = &MSTP_Port;
    MSTP_Init(mstp_port);
    packet_statistics_clear();
    /* decode any command line parameters */
    filename = filename_remove_path(argv[0]);
    for (argi = 1; argi < argc; argi++) {
        if (strcmp(argv[argi], "--help") == 0) {
            print_usage(filename);
            print_help(filename);
            return 0;
        }
        if (strcmp(argv[argi], "--version") == 0) {
            printf("mstpcap %s\n", BACNET_VERSION_TEXT);
            printf("Copyright (C) 2011-2016 by Steve Karg\n"
                   "This is free software; see the source for copying "
                   "conditions.\n"
                   "There is NO warranty; not even for MERCHANTABILITY or\n"
                   "FITNESS FOR A PARTICULAR PURPOSE.\n");
            return 0;
        }
        if (strcmp(argv[argi], "--scan") == 0) {
            argi++;
            if (argi >= argc) {
                printf("An file name must be provided.\n");
                return 1;
            }
            printf("Scanning %s\n", argv[argi]);
            /* perform statistics on the file */
            if (test_global_header(argv[argi])) {
                while (read_received_packet(mstp_port)) {
                    packet_count++;
                    fprintf(stderr, "\r%u packets", (unsigned)packet_count);
                }
                if (packet_count) {
                    packet_statistics_print();
                }
                Exit_Requested = true;
            } else {
                fprintf(stderr, "File header does not match.\n");
                return 1;
            }
        }
        if (strcmp(argv[argi], "--extcap-interfaces") == 0) {
            RS485_Print_Ports();
            return 0;
        }
        if (strcmp(argv[argi], "--extcap-dlts") == 0) {
            argi++;
            if (argi >= argc) {
                printf("An interface must be provided.\n");
                return 0;
            }
            printf(
                "dlt {number=%u}{name=BACnet MS/TP}"
                "{display=BACnet MS/TP}\n",
                DLT_BACNET_MS_TP);
            Exit_Requested = true;
        }
        if (strcmp(argv[argi], "--extcap-config") == 0) {
            printf("arg {number=0}{call=--baud}{display=Baud Rate}"
                   "{tooltip=Serial port baud rate in bits per second}"
                   "{type=selector}\n");
            printf("value {arg=0}{value=9600}{display=9600}{default=false}\n");
            printf(
                "value {arg=0}{value=19200}{display=19200}{default=false}\n");
            printf("value {arg=0}{value=38400}{display=38400}{default=true}\n");
            printf(
                "value {arg=0}{value=57600}{display=57600}{default=false}\n");
            printf(
                "value {arg=0}{value=76800}{display=76800}{default=false}\n");
            printf(
                "value {arg=0}{value=115200}{display=115200}{default=false}\n");
            return 0;
        }
        if (strcmp(argv[argi], "--capture") == 0) {
            /* do nothing - fall through and start running! */
            Wireshark_Capture = true;
        }
        if (strcmp(argv[argi], "--extcap-interface") == 0) {
            argi++;
            if (argi >= argc) {
                printf("An interface must be provided or "
                       "the selection must be displayed.\n");
                return 0;
            }
            RS485_Set_Interface(argv[argi]);
        }
#if defined(_WIN32)
        if (bacnet_strnicmp(argv[argi], "com", 3) == 0) {
            /* legacy command line options */
            RS485_Set_Interface(argv[argi]);
            if ((argi + 1) < argc) {
                argi++;
                my_baud = strtol(argv[argi], NULL, 0);
                RS485_Set_Baud_Rate(my_baud);
            }
        }
#else
        if (bacnet_strnicmp(argv[argi], "/dev/", 5) == 0) {
            /* legacy command line options */
            RS485_Set_Interface(argv[argi]);
            if ((argi + 1) < argc) {
                argi++;
                my_baud = strtol(argv[argi], NULL, 0);
                RS485_Set_Baud_Rate(my_baud);
            }
        }
#endif
        if (strcmp(argv[argi], "--baud") == 0) {
            argi++;
            if (argi >= argc) {
                printf("A baud rate must be provided.\n");
                return 0;
            }
            my_baud = strtol(argv[argi], NULL, 0);
            RS485_Set_Baud_Rate(my_baud);
        }
        if (strcmp(argv[argi], "--fifo") == 0) {
            argi++;
            if (argi >= argc) {
                printf("A named pipe must be provided.\n");
                return 0;
            }
            named_pipe_create(argv[argi]);
        }
    }
    if (Exit_Requested) {
        return 0;
    }
    if (argc <= 1) {
        RS485_Print_Ports();
        return 0;
    }
    atexit(cleanup);
    RS485_Initialize();
    mstimer_init();
    if (!Wireshark_Capture) {
        fprintf(
            stdout, "mstpcap: Using %s for capture at %ld bps.\n",
            RS485_Interface(), (long)RS485_Get_Baud_Rate());
    }
#if defined(_WIN32)
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, TRUE);
#else
    signal_init();
#endif
    filename_create_new();
    write_global_header();
    /* run forever */
    for (;;) {
        RS485_Check_UART_Data(mstp_port);
        MSTP_Receive_Frame_FSM(mstp_port);
        /* process the data portion of the frame */
        if (mstp_port->ReceivedValidFrame ||
            mstp_port->ReceivedValidFrameNotForUs) {
            write_received_packet(mstp_port, MSTP_HEADER_MAX);
            mstp_structure_init(mstp_port);
            packet_count++;
        } else if (mstp_port->ReceivedInvalidFrame) {
            if (MSTP_Receive_State == MSTP_RECEIVE_STATE_HEADER) {
                mstp_port->Index = 0;
            }
            write_received_packet(mstp_port, MSTP_HEADER_MAX);
            mstp_structure_init(mstp_port);
            Invalid_Frame_Count++;
            packet_count++;
        } else if (mstp_port->receive_state == MSTP_RECEIVE_STATE_IDLE) {
            if (MSTP_Receive_State == MSTP_RECEIVE_STATE_IDLE) {
                if ((mstp_port->EventCount == 1) &&
                    (mstp_port->DataRegister == 0xFF)) {
                    /* 0xFF padding at end of message is allowed */
                    mstp_structure_init(mstp_port);
                } else if (mstp_port->EventCount > 1) {
                    write_received_packet(mstp_port, 1);
                    mstp_structure_init(mstp_port);
                    Invalid_Frame_Count++;
                }
            } else {
                /* invalid byte or timeout */
                if (MSTP_Receive_State == MSTP_RECEIVE_STATE_PREAMBLE) {
                    if (mstp_port->EventCount) {
                        header_len = 1;
                    } else {
                        header_len = 2;
                    }
                } else {
                    header_len = 3 + mstp_port->Index;
                }
                write_received_packet(mstp_port, header_len);
                mstp_structure_init(mstp_port);
                Invalid_Frame_Count++;
            }
        }
        if (!Wireshark_Capture) {
            if (!(packet_count % 100)) {
                fprintf(
                    stdout, "\r%u packets, %u invalid frames",
                    (unsigned)packet_count, (unsigned)Invalid_Frame_Count);
                fflush(stdout);
            }
            if (packet_count >= 65535) {
                packet_statistics_print();
                packet_statistics_clear();
                filename_create_new();
                write_global_header();
                packet_count = 0;
            }
        }
        if (Exit_Requested) {
            break;
        }
        /* track the packetizer state */
        MSTP_Receive_State = mstp_port->receive_state;
    }
    /* tell signal interrupts we are done */
    Exit_Requested = false;

    return 0;
}
