#!/bin/bash

####
# Configuration file for building a busybox initramfs with Leviathan utilities
# installed
#
# Set the following options based on your desired environment. The :- syntax
# allows you to override the values by setting the variable in your
# environment, and if not the value to the right of the :- operator will be
# used
#
# The only __required__ value is a path the the Linux source
####



# Path to kernel source tree
# NOTE: this will build an isoimage with the kernel running in your current
# development environment, but you can pass a pointer to any compiled kernel
# source tree here

KERNEL_SOURCE=${KERNEL_SOURCE:-""}
#KERNEL_SOURCE=lib/modules/`uname -r`/build

if [ -z $KERNEL_SOURCE ]; then
    echo "You must specify KERNEL_SOURCE, either in your environment or via config.sh"
    exit
fi

# Whether or not to install the Leviathan/Petos development utilities
WANT_LEVIATHAN=${WANT_LEVIATHAN:-0}

# Path to leviathan source tree
if [ $WANT_LEVIATHAN -eq 1 ]; then
    LEVIATHAN_SOURCE=${LEVIATHAN_SOURCE:-$HOME/dev/leviathan}
fi

# Whether or not to install host kernel modules in the initramfs
WANT_MODULES=${WANT_MODULES:-1}

# An isoimage containing the initramfs and specified kernel image
# will be created
WANT_ISOIMAGE=${WANT_ISOIMAGE:-1}

# An isoimage containing a copy of the initramfs will be installed within the
# initramfs itself. This is useful if you want to run virtual machines via
# Palacios/Leviathan
WANT_GUEST_ISOIMAGE=${WANT_GUEST_ISOIMAGE:-0}

# Path to kernel source tree for guest OS (defaults to $KERNEL_SOURCE)
if [ $WANT_GUEST_ISOIMAGE -eq 1 ]; then
    GUEST_KERNEL_SOURCE=${GUEST_KERNEL_SOURCE:-$KERNEL_SOURCE}
fi

WANT_PXEBOOT=${WANT_PXEBOOT:-0}

# Path to pxedir
if [ $WANT_PXEBOOT -eq 1 ]; then
    PXEBOOT_DIR=${PXEBOOT_DIR:-/var/lib/tftpboot/hobbes-venv}
fi
