#!/bin/bash

if [ -z $KERNEL_SOURCE ]; then
    echo "KERNEL environment variable must be set"
    exit
fi

if [ -z $INITRD ]; then
    echo "INITRD environment variable must be set"
    exit
fi

if [ -z $CONFIG ]; then
    echo "CONFIG environment variable must be set"
    exit
fi

cp $KERNEL_SOURCE/arch/x86/boot/bzImage isofiles/bzImage
cp $INITRD isofiles/initrd.img
cp $CONFIG isofiles/isolinux.cfg

genisoimage -o image.iso \
    -b isolinux.bin \
    -input-charset utf-8 \
    -no-emul-boot -boot-load-size 4 \
    -boot-info-table isofiles
