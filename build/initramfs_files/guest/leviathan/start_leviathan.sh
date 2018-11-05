#!/bin/sh

echo "Inserting Petos Module."
modprobe petos

echo "Inserting XPMEM Module."
modprobe xpmem ns=0
#modprobe xpmem ns=0 xpmem_follow_page=1

echo "Launching Leviathan Node Manager."

export HOBBES_APP_ID=0

/opt/leviathan/lnx_init ${@:1} > /opt/leviathan/leviathan.log &
#/opt/leviathan/lnx_init ${@:1} &

echo $! > /opt/leviathan/leviathan.pid
