#!/bin/bash
echo "creating preview media"
read -r -e -p "create new montage? (y/n): " create_montage
if ! [[
	"$create_montage" == "y" || "$create_montage" == "Y" ||
	"$create_montage" == "n" || "$create_montage" == "N"
]]
then
	echo "error: unrecognized input"
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
" | source custom.sh
rm -f warrenld.txt
cp tmp/preview/video.mp4 preview.mp4
ffmpeg -sseof -3 -i tmp/preview/video.mp4 -vsync 0 -q:v 31 -update true -y preview.png -loglevel error
cp tmp/preview/fdiagram.mp4 previewfd.mp4
ffmpeg -sseof -3 -i tmp/preview/fdiagram.mp4 -vsync 0 -q:v 31 -update true -y previewfd.png -loglevel error
cp tmp/montage/video.mp4 previewmt.mp4
ffmpeg -sseof -3 -i tmp/montage/video.mp4 -vsync 0 -q:v 31 -update true -y previewmt.png -loglevel error
./bin/createlogo logo.png size=128
echo "preview files can now be found at preview*"
