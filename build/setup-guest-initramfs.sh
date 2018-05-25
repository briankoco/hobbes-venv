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

dirs=("bin" "lib" "lib64" "share" "usr" "root")

for d in ${dirs[@]}; do
    echo $d
    mkdir "guest-initramfs/$d"
    cp -r initramfs/$d/ guest-initramfs/
done

# etc is different as there are differences from the host
cp -r initramfs_files/guest/etc guest-initramfs/etc

pushd guest-initramfs
ln -s bin sbin
popd

cp initramfs/init guest-initramfs
