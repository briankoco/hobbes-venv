#!/bin/bash

# Run this script once to setup external library dependencies
# (busybox, dropbear)

if [ -d "extern/install" ]; then
    echo "The directory extern/install already exists"
    read -p "Do you want to reconfigure/recompile external libraries? (y/n) " cont

    if [ "$cont" != "y" ]; then
        exit -1
    fi
fi

git submodule init
git submodule update

setup_log=${PWD}/setup.log

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo_err() {
    local msg=$1
    echo -e "\t${RED}$1${NC}"
}

echo_success() {
    local msg=$1
    echo -e "\t${GREEN}$1${NC}"
}

exec_and_check() {
    local cmd=$1

    $cmd &>> $setup_log
    ret_val=$?

    if [ $ret_val -ne 0 ]; then
        echo_err "Failed"
        return 1
    else
        echo_success "OK"
        return 0
    fi
}


pushd extern/scripts
echo "Detailed logs available in setup.log"
echo

echo -n "Configuring busybox ... "
exec_and_check ./busybox-config.sh || exit 1

echo -n "Building busybox ... "
exec_and_check ./busybox-build.sh || exit 1

echo -n "Configuring dropbear ... "
exec_and_check ./dropbear-config.sh || exit 1

echo -n "Building dropbear ... "
exec_and_check ./dropbear-build.sh || exit 1

popd

source build/guest-initramfs/lib/firmware
