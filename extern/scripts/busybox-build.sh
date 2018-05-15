#!/bin/bash

pushd ../busybox

procs=$(getconf _NPROCESSORS_ONLN)
make -j $procs
ret_val=$?

if [ $ret_val -eq 0 ]; then
    mkdir -p ../install/bin
    cp busybox ../install/bin
fi

popd

exit $ret_val
