#!/bin/bash
read -r -e -p "create new montage? (y/n): " create_montage
if ! [[
    "$create_montage" == "y" || "$create_montage" == "Y" ||
    "$create_montage" == "n" || "$create_montage" == "N"
]]
then
    echo "unrecognized input"
    exit 1
fi
mkdir -p tmp/preview
rm -rf tmp/preview/*
if [[ "$create_montage" == "y" || "$create_montage" == "Y" ]]
then
    ./montage.sh
fi
cat warren.txt | sed "s/dampening=3.0e2/dampening=3.0e1/g" > warrenld.txt
echo "\
warrenld.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/preview
" | ./custom.sh
rm -f warrenld.txt
cp tmp/preview/video.mp4 preview.mp4
ffmpeg -sseof -3 -i tmp/preview/video.mp4 -vsync 0 -q:v 31 -update true -y preview.png -loglevel error
cp tmp/montage/video.mp4 previewmt.mp4
ffmpeg -sseof -3 -i tmp/montage/video.mp4 -vsync 0 -q:v 31 -update true -y previewmt.png -loglevel error
