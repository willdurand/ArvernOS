#!/bin/sh

dd if=/dev/zero of=$1 bs=1M count=500

(echo o
echo n
echo p
echo 1
echo
echo
echo a
echo p
echo w) | fdisk -u -C500 -S63 -H16 $1

mkdir -p /mnt/osdev

umount /dev/loop0
umount /dev/loop1

losetup /dev/loop0 $1
losetup -o1048576 /dev/loop1 $1

mkdosfs -F32 /dev/loop1

mount /dev/loop1 /mnt/osdev

cp -R $2/* /mnt/osdev

echo "(hd0)   /dev/loop0" > device.map

cp device.map /mnt/osdev/boot/grub/device.map

rm device.map

grub-install --no-floppy                                      \
    --grub-mkdevicemap=/mnt/osdev/boot/grub/device.map              \
    --modules="biosdisk part_msdos ext2 configfile normal multiboot"\
    --root-directory=/mnt/osdev                                     \
    /dev/loop0

sync

umount /dev/loop0
umount /dev/loop1

losetup -d /dev/loop0
losetup -d /dev/loop1
