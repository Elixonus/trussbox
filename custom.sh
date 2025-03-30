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
read -r -e -p "output dirname: " dirname
mkdir -p $dirname
rm -rf $dirname/*
mkdir -p $dirname/problems
mkdir -p $dirname/solutions
mkdir -p $dirname/prosols
mkdir -p $dirname/frames
mkdir -p $dirname/diagrams
./bin/pipeline \
	solvetruss_executable=bin/solvetruss \
	rendertruss_executable=bin/rendertruss \
	forcediagram_executable=bin/forcediagram \
	feedback_executable=bin/feedback \
	problem_filename=$filename \
	problems_dirname=$dirname/problems \
	solutions_dirname=$dirname/solutions \
	prosols_dirname=$dirname/prosols \
	frames_dirname=$dirname/frames \
	diagrams_dirname=$dirname/diagrams \
	gravity=$gravity \
	timef=$timef \
	srate=$srate \
	frate=$frate \
	fsize=${fwidth}x${fheight} \
	"fcenter=($fcenterx $fcentery)" \
	fzoom=$fzoom \
	fscale=$fscale \
	> $dirname/pipeline.sh
source $dirname/pipeline.sh
rm -rf $dirname/prosols
ffmpeg -r $frate -i $dirname/frames/%09d.png -y $dirname/video.mp4 -loglevel error
rm -rf $dirname/frames
ffmpeg -r $frate -i $dirname/diagrams/%09d.png -y $dirname/fdiagram.mp4 -loglevel error
rm -rf $dirname/diagrams
