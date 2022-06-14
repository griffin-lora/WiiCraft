#!/usr/bin/env bash
mkdir -p build/image
if ! [ -f image.iso ]; then
    if command -v mksdcard &> /dev/null; then
        mksdcard 50M image.iso
    else
        /sbin/mkfs.vfat -F 32 -C image.iso 50000
    fi
fi
sudo mount -o defaults,umask=000 image.iso build/image
cp -r build/data build/image
sudo umount build/image