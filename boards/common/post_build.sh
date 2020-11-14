#!/bin/bash
LINUX_ADDRESS=0x80000000
$HOST_DIR/bin/mkimage -A riscv -O linux -T kernel -C none -a $LINUX_ADDRESS -e $LINUX_ADDRESS -n Linux -d $BINARIES_DIR/Image $BINARIES_DIR/uImage

