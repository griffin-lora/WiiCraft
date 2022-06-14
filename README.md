# Minecraft on Wii

A Minecraft clone for the Wii

### Build instructions:

#### Prerequistites

 - devkitPPC + portlibs
 - mksdcard or dosfstools
 - [PerlinNoise.hpp](https://raw.githubusercontent.com/Reputeless/PerlinNoise/a50dc1897b530b74f05fe8189c9397b8189f0fe6/PerlinNoise.hpp) (place in src/)
 - devkitPro tools (in $PATH for gxtexconv)

#### Building

 - Run make in the repository root
 - Run ./textures.sh to generate textures
 - If you're running on Dolphin, run ./image.sh to generate the SD card image (\*nix systems only)

