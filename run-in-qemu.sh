#!/bin/bash

if [ $# -eq 1 ]; then
    isoimage=$1
elif [ ! -f images/image.iso ]; then
    echo "There are two ways to use this script:"
    echo "  Specify WANT_ISOIMAGE in your config.sh script and build "
    echo "  an isoimage via ./build.sh, which will create the file "
    echo "  images/image.iso. Then invoke this script with no arguments"
    echo " OR"
    echo "  Run this command with a path to the image you want to boot"
    exit 1
else
    isoimage="images/image.iso"
fi

echo "Starting qemu terminal emulator in 5s."
echo "Once the VM starts, type ctrl-A+X to kill the VM and exit"
sleep 5

qemu-system-x86_64 -enable-kvm -cpu host -smp 2 -m 1024 -boot d -cdrom $isoimage -nographic

#qemu-system-x86_64 -enable-kvm -cpu host -smp 2 -m 1024 -kernel images/bzImage -initrd images/initrd.img -nographic -append "console=ttyS0"
