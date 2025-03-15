#!/bin/bash
read -r -e -p "problem filename: " filename
read -r -e -p "gravitational acceleration (m/s^2): " gravity
read -r -e -p "final time (s): " timef
read -r -e -p "step rate (Hz): " srate
read -r -e -p "frame center x (m): " fcenterx
read -r -e -p "frame center y (m): " fcentery
read -r -e -p "frame zoom: " fzoom
read -r -e -p "frame scale: " fscale
read -r -e -p "solution dirname: " dirname
mkdir -p $dirname
rm -rf $dirname/*
cp $filename ${dirname}/problem.txt
mkdir -p ${dirname}/frames
./bin/solvetruss ${dirname}/frames gravity=${gravity} timef=${timef} srate=${srate} frate=180 fsize=1920x1080 "fcenter=(${fcenterx} ${fcentery})" fzoom=${fzoom} fscale=${fscale} < $dirname/problem.txt > $dirname/solution.txt
ffmpeg -r 180 -i ${dirname}/frames/%09d.png -vf tmix=frames=3:weights="1 1 1",select="not(mod(n\,3))" -r 60 -y ${dirname}/video.mp4 -loglevel error
rm -f ${dirname}/frames/*.png
rmdir ${dirname}/frames
cat $dirname/problem.txt $dirname/solution.txt > $dirname/pronsol.txt
./bin/forcediagram ${dirname} gravity=${gravity} fsize=${fwidth}x${fheight} "fcenter=(${fcenterx} ${fcentery})" fzoom=${fzoom} fscale=${fscale} < $dirname/pronsol.txt
rm -f $dirname/pronsol.txt
