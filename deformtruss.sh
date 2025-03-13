#!/bin/bash
read -r -e -p "problem filename: " filename
read -r -e -p "gravitational acceleration (m/s^2): " gravity
read -r -e -p "final time (s): " timef
read -r -e -p "step rate (Hz): " srate
read -r -e -p "frame rate (Hz): " frate
read -r -e -p "frame width (px): " fwidth
read -r -e -p "frame height (px): " fheight
read -r -e -p "frame center x (m): " fcenterx
read -r -e -p "frame center y (m): " fcentery
read -r -e -p "frame zoom: " fzoom
read -r -e -p "frame scale: " fscale
read -r -e -p "solution dirname: " dirname
mkdir -p $dirname
mkdir -p ${dirname}/frames
rm -f ${dirname}/frames/*.png
./bin/deformtruss ${dirname}/frames gravity=${gravity} timef=${timef} srate=${srate} frate=${frate} fsize=${fwidth}x${fheight} "fcenter=(${fcenterx} ${fcentery})" fzoom=${fzoom} fscale=${fscale} < $filename > $dirname/solution.txt
ffmpeg -r 60 -i ${dirname}/frames/%05d.png -y ${dirname}/video.mp4 -loglevel fatal
rm -f ${dirname}/frames/*.png
rmdir ${dirname}/frames
