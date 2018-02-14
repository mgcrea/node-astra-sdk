#!/bin/bash
SCRIPTPATH="$(cd "$(dirname "$0")"; pwd -P)"

if [[ $OSTYPE =~ "darwin" ]]; then
  DYLD_FALLBACK_LIBRARY_PATH=${SCRIPTPATH}/../bundle/AstraSDK/lib node $1
else
  LD_LIBRARY_PATH=${SCRIPTPATH}/../bundle/OpenNI-linux-x64/Redist node $1
fi;
