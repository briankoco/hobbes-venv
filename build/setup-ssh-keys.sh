#!/bin/sh

keygen=$(pwd)/initramfs/bin/dropbearkey
dest=$(pwd)/build/initramfs_files/ssh_keys
mkdir -p $dest
rm -f $dest/*

$keygen -t rsa -f "${dest}/id_dropbear"
$keygen -y -f "${dest}/id_dropbear" | grep "^ssh-rsa " > "${dest}/id_dropbear.pub"
