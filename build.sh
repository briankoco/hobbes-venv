#!/bin/bash

HOST_INITRAMFS="${PWD}/initramfs"
GUEST_INITRAMFS="${PWD}/guest-initramfs"

# Source the requested configuration file
source config.sh

if [ -z $KERNEL_SOURCE ]; then
    echo "Must set KERNEL_SOURCE variable in config.sh"
    exit 1
fi

RELEASE_FILE="include/config/kernel.release"

# Determine the kernel release by invoking the kernel makefile
get_kernel_release() {
    local headers=$1
    local release_file="$headers/$RELEASE_FILE"

    # If file does not exist, build it
    if [ ! -f $release_file ]; then
        make -C $headers $RELEASE_FILE &> build.log
        if [ $? -ne 0 ]; then
            echo "Failed to generate $headers/$RELEASE_FILE. See build.log" 1>&2
            return 
        fi

        rm -f build.log
    fi

    local release=$(cat $headers/$RELEASE_FILE)
    echo $release
}

KERNEL_RELEASE=$(get_kernel_release $KERNEL_SOURCE)
if [ -z $KERNEL_RELEASE ]; then
    echo "Failed to determine KERNEL_RELEASE from kernel headers"
    exit 1
fi

if [ ! -z $GUEST_KERNEL_SOURCE ]; then
    GUEST_KERNEL_RELEASE=$(get_kernel_release $GUEST_KERNEL_SOURCE)
    if [ -z $GUEST_KERNEL_RELEASE ]; then
        echo "Failed to determine KERNEL_RELEASE from kernel headers"
        exit 1
    fi
fi

pushd build

export WANT_LEVIATHAN
export LEVIATHAN_SOURCE
export WANT_MODULES

if [ $WANT_GUEST_ISOIMAGE -eq 1 ]; then
    echo "Building guest iso image ..."

    # Install kernel and modules in guest
    INITRAMFS=${GUEST_INITRAMFS} GUEST_KERNEL_RELEASE=${GUEST_KERNEL_RELEASE} \
        KERNEL_SOURCE=${GUEST_KERNEL_SOURCE} source ./install.sh 

    # Build guest iso
    KERNEL_SOURCE=${GUEST_KERNEL_SOURCE} INITRD="${GUEST_INITRAMFS}.cpio.gz" CONFIG="guest.cfg" \
        source ./gen-isoimage.sh 

    # Copy guest iso to host initramfs
    mkdir -p ${HOST_INITRAMFS}/opt/isos
    mv image.iso ${HOST_INITRAMFS}/opt/isos/guest-img.iso
    rm "${GUEST_INITRAMFS}.cpio.gz"
else
    rm -f initramfs/opt/isos/guest-img.iso
fi

# Install kernel and modules in host
INITRAMFS=${HOST_INITRAMFS} KERNEL_RELEASE=${KERNEL_RELEASE} KERNEL_SOURCE=${KERNEL_SOURCE} \
    source ./install.sh

rm -f ../images/image.iso
rm -f ../images/initrd.img
rm -f ../images/bzImage

# Build host iso, or save initrd/bzimage
if [ $WANT_ISOIMAGE -eq 1 ]; then
    KERNEL_SOURCE=${KERNEL_SOURCE} INITRD="${HOST_INITRAMFS}.cpio.gz" CONFIG="host.cfg" \
        source ./gen-isoimage.sh 

    mkdir -p ../images
    mv image.iso ../images/image.iso
    rm "${HOST_INITRAMFS}.cpio.gz"
else
    mv "${HOST_INITRAMFS}.cpio.gz" ../images/initrd.img
    cp $KERNEL_SOURCE/arch/x86/boot/bzImage ../images/bzImage
fi

popd
