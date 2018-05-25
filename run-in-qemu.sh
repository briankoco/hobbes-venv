#!/bin/bash

if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    echo "Usage: $0 <isoimage> [<CPU model ('qemu' or 'host')>]"
    exit -1
fi

isoimage=$1
host_cpu=0

if [ $# -eq 2 ]; then
    model=$2

    if [ "$model" = "host" ]; then
        host_cpu=1
    elif [ ! "$model" = "qemu" ]; then
        echo "<CPU model> must be either 'qemu' or 'host'"
        exit -1
    fi
fi


echo "Starting qemu terminal emulator in 5s."
echo "Once the VM starts, type ctrl-A+X to kill the VM and exit"
sleep 5

#qemu-system-x86_64 -enable-kvm -cpu host -smp 2 -m 1024 -boot d -cdrom $isoimage -nographic 
if [ $host_cpu -eq 1 ]; then
    cpu_opt="host"
else
    cpu_opt="qemu64"
fi

qemu-system-x86_64 -enable-kvm -cpu $cpu_opt -smp 2 -m 1024 -boot d -cdrom $isoimage -nographic 
