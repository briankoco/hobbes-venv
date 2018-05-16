#!/bin/bash

if [ -z $KERNEL_SOURCE ]; then
    echo "KERNEL_SOURCE environment variable must be set"
    exit
fi

if [ -z $KERNEL_RELEASE ]; then
    echo "KERNEL_RELEASE environment variable must be set"
    exit
fi

if [ -z $INITRAMFS ]; then
    echo "INITRAMFS environment variable must be set"
    exit
fi

if [ -z $WANT_LEVIATHAN ]; then
    echo "WANT_LEVIATHAN variable must be set"
    exit
fi

if [ $WANT_LEVIATHAN -eq 1 ]; then
    if [ -z $LEVIATHAN_SOURCE ]; then
        echo "LEVIATHAN_SOURCE variable must be set"
        exit
    fi
fi

if [ -z $WANT_MODULES ]; then
    echo "WANT_MODULES variable must be set"
    exit
fi

# Make sure the directory structure is populated
dev_directories=("pts" "bsg" "net" "raw" "input" "shm" "dri" "mapper" "cpu" "snd")

mkdir -p "$INITRAMFS/dev"
for d in ${dev_directories[@]}; do
    mkdir -p "$INITRAMFS/dev/$d"
done

mkdir -p "$INITRAMFS/tmp"
mkdir -p "$INITRAMFS/sys"
mkdir -p "$INITRAMFS/proc"

# Install kernel modules in the initramfs image
if [ $WANT_MODULES -eq 1 ]; then
    echo "Installing kernel modules in $INITRAMFS"

    pushd $KERNEL_SOURCE
    INSTALL_MOD_PATH=$INITRAMFS make modules_install
    popd
fi

# Install Petos environment in the initramfs image
if [ $WANT_LEVIATHAN -eq 1 ]; then
    echo "Installing Leviathan/Petos environment in $INITRAMFS"
    rm -f $INITRAMFS/lib/modules/*/extra/*

    # Find the custom module installation path based on the
    # requested kernel version
    ins_dir=""
    for dir in $(ls $INITRAMFS/lib/modules/); do
        if [[ $dir == $KERNEL_RELEASE ]]; then
            ins_dir=$INITRAMFS/lib/modules/$dir/extra
            mkdir -p $ins_dir
            break
        fi
    done

    if [ -z $ins_dir ]; then
        echo "Could not find modules subdirectory within $INITRAMFS/lib/modules"

        if [ $WANT_MODULES -ne 1 ]; then
            echo "Try installing kernel modules (set WANT_MODULES=1)"
            exit 0
        else
            echo "This looks like a bug in the build script. Sorry"
            exit 1
        fi
    fi
    
    # Palacios
    mkdir -p $INITRAMFS/opt/v3vee
    cp $LEVIATHAN_SOURCE/palacios/v3vee.ko $ins_dir
    cp -r $LEVIATHAN_SOURCE/palacios/user $INITRAMFS/opt/v3vee

    if [ -f $LEVIATHAN_SOURCE/kitten-vm/arch/x86_64/boot/image.iso ]; then
        cp $LEVIATHAN_SOURCE/kitten-vm/arch/x86_64/boot/image.iso $INITRAMFS/opt/isos/vmlwk.iso
    fi

    # Pisces
    mkdir -p $INITRAMFS/opt/pisces
    cp $LEVIATHAN_SOURCE/pisces/pisces.ko $ins_dir
    cp -r $LEVIATHAN_SOURCE/pisces/linux_usr $INITRAMFS/opt/pisces
    cp $LEVIATHAN_SOURCE/kitten/vmlwk.bin $INITRAMFS/opt/pisces

    # Petos
    cp $LEVIATHAN_SOURCE/petlib/petos/petos.ko $ins_dir
    mkdir -p $INITRAMFS/opt/utils
    cp $LEVIATHAN_SOURCE/petlib/irq_ctrl $INITRAMFS/opt/utils
    cp $LEVIATHAN_SOURCE/petlib/hw_status $INITRAMFS/opt/utils

    # XEMEM
    cp $LEVIATHAN_SOURCE/xpmem/mod/xpmem.ko $ins_dir

    # Leviathan
    mkdir -p $INITRAMFS/opt/leviathan
    cp $LEVIATHAN_SOURCE/lnx_inittask/lnx_init $INITRAMFS/opt/leviathan
    cp $LEVIATHAN_SOURCE/lwk_inittask/lwk_init $INITRAMFS/opt/leviathan
    cp $LEVIATHAN_SOURCE/shell/hobbes $INITRAMFS/opt/leviathan
    cp initramfs_files/leviathan/* $INITRAMFS/opt/leviathan

    # HIO stubs
    mkdir -p $INITRAMFS/opt/hio
    cp $LEVIATHAN_SOURCE/hio/generic-io-stub/stub $INITRAMFS/opt/hio
    cp $LEVIATHAN_SOURCE/hio/test-app/app $INITRAMFS/opt/hio/app

    # Configuration files
    mkdir -p $INITRAMFS/opt/configs
    cp initramfs_files/configs/* $INITRAMFS/opt/configs

    # External binaries
    cp -r ../extern/install/bin/* $INITRAMFS/bin
    cp -r ../extern/install/sbin/* $INITRAMFS/sbin
fi

# Install the initramfs
pushd $INITRAMFS
echo "Generating ${INITRAMFS}.cpio.gz"
find . | cpio -H newc -o | gzip > ${INITRAMFS}.cpio.gz
popd
