#!/bin/bash
SCRIPTPATH="$(cd "$(dirname "$0")"; pwd -P)"

if [[ $OSTYPE =~ "darwin" ]]; then
  DYLD_FALLBACK_LIBRARY_PATH=${SCRIPTPATH}/../bundle/AstraSDK-darwin-x64/lib node $1
else
  dpkgArch="$(dpkg --print-architecture | awk -F- '{ print $NF }')"
  LD_LIBRARY_PATH=${SCRIPTPATH}/../bundle/OpenNI-linux-${dpkgArch}/Redist node $1
fi;
