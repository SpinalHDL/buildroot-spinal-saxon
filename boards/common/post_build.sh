#!/bin/bash
LINUX_ADDRESS=0x80000000
$HOST_DIR/bin/mkimage -A riscv -O linux -T kernel -C none -a $LINUX_ADDRESS -e $LINUX_ADDRESS -n Linux -d $BINARIES_DIR/Image $BINARIES_DIR/uImage

# haveged : change priority
sed -i 's/start-stop-daemon -S -x/start-stop-daemon -S -N 10 -x/' $TARGET_DIR/etc/init.d/S21haveged