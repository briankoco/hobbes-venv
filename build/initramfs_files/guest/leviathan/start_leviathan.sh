#!/bin/sh

echo "Inserting Petos Module."
modprobe petos

echo "Inserting XPMEM Module."
modprobe xpmem ns=0

echo "Launching Leviathan Node Manager."

/opt/leviathan/lnx_init ${@:1} > /opt/leviathan/leviathan.log &
#/opt/leviathan/lnx_init ${@:1} &

echo $! > /opt/leviathan/leviathan.pid
