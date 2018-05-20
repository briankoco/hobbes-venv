#!/bin/bash

if [ -d guest-initramfs ]; then
    echo "The directory guest-initramfs already exists"
    read -p "Do you want to remove and regenerate it? (y/n) " cont

    if [ "$cont" != "y" ]; then
        return
    fi

    rm -rf guest-initramfs
fi

mkdir guest-initramfs

# /dev, /tmp, /sys, /proc will be created by install.sh

dirs=("bin" "lib" "lib64" "share" "usr" "etc")

for d in ${dirs[@]}; do
    echo $d
    mkdir "guest-initramfs/$d"
    cp -r initramfs/$d/ guest-initramfs/
done

pushd guest-initramfs
ln -s bin sbin
popd

cp initramfs/init guest-initramfs
