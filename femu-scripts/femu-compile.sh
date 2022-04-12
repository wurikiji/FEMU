#!/bin/bash

NRCPUS="$(cat /proc/cpuinfo | grep "vendor_id" | wc -l)"

SQLITE_FLAGS="-DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_OMIT_DEPRECATED -DSQLITE_OMIT_SHARED_CACHE -w -DSQLITE_ENABLE_MEMSYS5"

make clean
# --disable-werror --extra-cflags=-w
../configure --enable-kvm --target-list=x86_64-softmmu --disable-git-update --disable-werror \
  --extra-cflags="$SQLITE_FLAGS"

make -j 5

echo ""
echo "===> FEMU compilation done ..."
echo ""
exit
