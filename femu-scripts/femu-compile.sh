#!/bin/bash

NRCPUS="$(cat /proc/cpuinfo | grep "vendor_id" | wc -l)"

make clean
# --disable-werror --extra-cflags=-w
../configure --enable-kvm --target-list=x86_64-softmmu --disable-git-update --disable-werror \
  --extra-cflags=-DSQLITE_OMIT_LOAD_EXTENSION \
  --extra-cxxflags=-DSQLITE_OMIT_LOAD_EXTENSION
make -j $NRCPUS

echo ""
echo "===> FEMU compilation done ..."
echo ""
exit
