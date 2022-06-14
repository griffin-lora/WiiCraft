mkdir -p build/image
test ! -f image.iso && mksdcard 50M image.iso
sudo mount -o defaults,umask=000 image.iso build/image
cp -r build/data build/image
sudo umount build/image