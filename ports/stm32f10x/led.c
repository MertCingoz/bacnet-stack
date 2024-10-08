/**************************************************************************
 *
 * Copyright (C) 2011 Steve Karg <skarg@users.sourceforge.net>
 *
 * SPDX-License-Identifier: MIT
 *
 *********************************************************************/
#include <stdint.h>
#include "hardware.h"
#include "bacnet/basic/sys/mstimer.h"
#include "led.h"

static struct mstimer Off_Delay_Timer_Rx;
static struct mstimer Off_Delay_Timer_Tx;
static bool Rx_State;
static bool Tx_State;
static bool LD3_State;

/*************************************************************************
 * Description: Activate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_tx_on(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
    mstimer_set(&Off_Delay_Timer_Tx, 0);
    Tx_State = true;
}

/*************************************************************************
 * Description: Activate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_rx_on(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
    mstimer_set(&Off_Delay_Timer_Rx, 0);
    Rx_State = true;
}

/*************************************************************************
 * Description: Deactivate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_tx_off(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
    mstimer_set(&Off_Delay_Timer_Tx, 0);
    Tx_State = false;
}

/*************************************************************************
 * Description: Deactivate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_rx_off(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
    mstimer_set(&Off_Delay_Timer_Rx, 0);
    Rx_State = false;
}

/*************************************************************************
 * Description: Get the state of the LED
 * Returns: true if on, false if off.
 * Notes: none
 *************************************************************************/
bool led_rx_state(void)
{
    return Rx_State;
}

/*************************************************************************
 * Description: Get the state of the LED
 * Returns: true if on, false if off.
 * Notes: none
 *************************************************************************/
bool led_tx_state(void)
{
    return Tx_State;
}

/*************************************************************************
 * Description: Toggle the state of the LED
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_tx_toggle(void)
{
    if (led_tx_state()) {
        led_tx_off();
    } else {
        led_tx_on();
    }
}

/*************************************************************************
 * Description: Toggle the state of the LED
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_rx_toggle(void)
{
    if (led_rx_state()) {
        led_rx_off();
    } else {
        led_rx_on();
    }
}

/*************************************************************************
 * Description: Delay before going off to give minimum brightness.
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_rx_off_delay(uint32_t delay_ms)
{
    mstimer_set(&Off_Delay_Timer_Rx, delay_ms);
}

/*************************************************************************
 * Description: Delay before going off to give minimum brightness.
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_tx_off_delay(uint32_t delay_ms)
{
    mstimer_set(&Off_Delay_Timer_Tx, delay_ms);
}

/*************************************************************************
 * Description: Turn on, and delay before going off.
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_rx_on_interval(uint16_t interval_ms)
{
    led_rx_on();
    mstimer_set(&Off_Delay_Timer_Rx, interval_ms);
}

/*************************************************************************
 * Description: Turn on, and delay before going off.
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_tx_on_interval(uint16_t interval_ms)
{
    led_tx_on();
    mstimer_set(&Off_Delay_Timer_Tx, interval_ms);
}

/*************************************************************************
 * Description: Task for blinking LED
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_task(void)
{
    if (mstimer_expired(&Off_Delay_Timer_Rx)) {
        mstimer_set(&Off_Delay_Timer_Rx, 0);
        led_rx_off();
    }
    if (mstimer_expired(&Off_Delay_Timer_Tx)) {
        mstimer_set(&Off_Delay_Timer_Tx, 0);
        led_tx_off();
    }
}

/*************************************************************************
 * Description: Activate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_ld4_on(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
}

/*************************************************************************
 * Description: Deactivate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_ld4_off(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
}

/*************************************************************************
 * Description: Activate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_ld3_on(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
    LD3_State = true;
}

/*************************************************************************
 * Description: Deactivate the LED
 * Returns: nothing
 * Notes: none
 **************************************************************************/
void led_ld3_off(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
    LD3_State = false;
}

/*************************************************************************
 * Description: Get the state of the LED
 * Returns: true if on, false if off.
 * Notes: none
 *************************************************************************/
bool led_ld3_state(void)
{
    return LD3_State;
}

/*************************************************************************
 * Description: Toggle the state of the LED
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_ld3_toggle(void)
{
    if (led_ld3_state()) {
        led_ld3_off();
    } else {
        led_ld3_on();
    }
}

/*************************************************************************
 * Description: Initialize the LED hardware
 * Returns: none
 * Notes: none
 *************************************************************************/
void led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);
    /* Configure the Receive LED on MS/TP board */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure the Transmit LED on MS/TP board */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    /* Configure the LD4 on Discovery board */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Configure the LD3 on Discovery board */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Enable the GPIO_LED Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    led_tx_on();
    led_rx_on();
    led_ld3_on();
    led_ld4_on();
}
