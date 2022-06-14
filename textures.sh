#!/usr/bin/env bash
mkdir -p build/data/textures
cd textures
for file in *
do
    name="${file%.*}"
    gxtexconv -i "$file" -o ../build/data/textures/"$name".tpl
    rm ../build/data/textures/"$name".h
done