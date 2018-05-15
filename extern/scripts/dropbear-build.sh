#!/bin/bash

pushd ../build/dropbear

nproc=$(getconf _NPROCESSORS_ONLN)
make -j $procs PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" install
ret_val=$?

popd

exit $ret_val
