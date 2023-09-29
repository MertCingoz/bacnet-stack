#!/bin/bash
# script adapted from
# https://sourceforge.net/p/forge/documentation/Using%20the%20Release%20API/
# sudo apt-get update -qq
# sudo apt-get install -qq build-essential mingw-w64 curl git
#
# Prior to running this script, be sure to:
# a) update CHANGELOG and version.h with new version number, and commit changes.
# b) git tag to bacnet-stack-x.y.z where x.y.z is the new version number
# c) create long term branch as bacnet-stack-x.y if needed

USERNAME='skarg'

if [ -z "$1" ]
then
  echo "Usage: `basename $0` 0.0.0"
  echo "Builds the Win32 release files and uploads to sf.net"
  exit 1
fi

version="$1"
tools="bacnet-tools-$version"
tag_name="bacnet-stack-$version"
url_api='https://sourceforge.net/projects/bacnet/files'
url_frs="${USERNAME},bacnet@frs.sourceforge.net:/home/frs/project/b/ba/bacnet"
url_frs_tools="$url_frs/bacnet-tools"
url_frs_source="$url_frs/bacnet-stack"

echo "Build Win32 Apps"
export CC=i686-w64-mingw32-gcc
export LD=i686-w64-mingw32-ld
i686-w64-mingw32-gcc --version
make clean
make -s LEGACY=true win32

echo "ZIP Win32 Tools"
mkdir -p $tools
cp ./bin/*.exe $tools
zip -r $tools.zip $tools
rm $tools/*.exe
mv $tools.zip $tools/$tools.zip
cp ./bin/bvlc.bat $tools
cp ./bin/readme.txt $tools
cp ./apps/mstpcap/mstpcap.txt $tools

echo "Upload Win32 Tools with SCP"
scp -r $tools $url_frs_tools

echo "ZIP Source Code for Tag $tag_name"
git archive --format zip --output $tag_name.zip $tag_name

echo "TGZ Source Code for Tag $tag_name"
git archive --format tgz --output $tag_name.tgz $tag_name

mkdir -p $tag_name
mv $tag_name.zip $tag_name
mv $tag_name.tgz $tag_name
cp CHANGELOG.md $tag_name
cp README.md $tag_name
cp SECURITY.md $tag_name

echo "Upload Source Code with SCP"
scp -r $tag_name $url_frs_source

#echo "Set the default download for Windows and POSIX"
#api_key=""api_key=$SOURCEFORGE_RELEASE_API_KEY_SKARG""
#default_win='"default=windows"'
#default_posix='"default=mac&default=linux&default=bsd&default=solaris&default=others"'
#accept='"Accept: application/json"'
#url_tools="$url_api/bacnet-tools/$tools/$tools.zip"
#url_source="$url_api/bacnet-stack/$tag_name/$tag_name.tgz"
#curl -H $accept -X PUT -d $default_win -d $api_key $url_tools
#curl -H $accept -X PUT -d $default_posix -d $api_key $url_source