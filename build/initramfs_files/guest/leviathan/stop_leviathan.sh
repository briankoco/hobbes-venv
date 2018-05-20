#!/bin/sh

pidfile=/opt/leviathan/leviathan.pid

if [ -e $pidfile ]; then
    kill -s TERM $(cat $pidfile)
    rm -f $pidfile
else
    kill -s TERM $(pidof lnx_init)
fi
