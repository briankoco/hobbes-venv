#!/bin/bash

echo "Autogenerating dropbear config script ..."
pushd ../dropbear
autoconf || exit 1
autoheader || exit 1
popd

extern_dir=${PWD}/..
dropbear_src=${extern_dir}/dropbear
dropbear_build=${extern_dir}/build/dropbear
dropbear_install=${extern_dir}/install

mkdir -p $dropbear_build
pushd $dropbear_build
$dropbear_src/configure --enable-static --prefix=${dropbear_install}
ret_val=$?
popd

exit $ret_val
