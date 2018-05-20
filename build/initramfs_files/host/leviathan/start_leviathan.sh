#!/bin/sh

echo "Inserting Petos Module."
modprobe petos

echo "Inserting XPMEM Module."
modprobe xpmem ns=1
#modprobe xpmem ns=1 xpmem_follow_page=1
#modprobe xpmem ns=1 xpmem_follow_page=1 xpmem_debug_on=1

echo "Inserting V3VEE Module."
modprobe v3vee

echo "Inserting Pisces Module."
modprobe pisces

echo "Launching Leviathan Node Manager."

export HOBBES_ENCLAVE_ID=0
export HOBBES_APP_ID=0
/opt/leviathan/lnx_init ${@:1} > /opt/leviathan/leviathan.log &
#/opt/leviathan/lnx_init ${@:1} &

echo $! > /opt/leviathan/leviathan.pid
