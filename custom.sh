#!/bin/bash
set -eo pipefail
ncolors=$(tput colors)
if test -n "$ncolors" && test $ncolors -ge 9; then
	bold="$(tput bold)"
	underline="$(tput smul)"
	standout="$(tput smso)"
	normal="$(tput sgr0)"
	fg_black="$(tput setaf 0)";   bg_black="$(tput setab 0)"
	fg_red="$(tput setaf 1)";     bg_red="$(tput setab 1)"
	fg_green="$(tput setaf 2)";   bg_green="$(tput setab 2)"
	fg_yellow="$(tput setaf 3)";  bg_yellow="$(tput setab 3)"
	fg_blue="$(tput setaf 4)";    bg_blue="$(tput setab 4)"
	fg_magenta="$(tput setaf 5)"; bg_magenta="$(tput setab 5)"
	fg_cyan="$(tput setaf 6)";    bg_cyan="$(tput setab 6)"
	fg_white="$(tput setaf 7)";   bg_white="$(tput setab 7)"
	fg_gray="$(tput setaf 8)";    bg_gray="$(tput setab 8)"
fi
read -rep "problem filename: " filename
read -rep "gravitational acceleration (m/s^2): " gravity
read -rep "final time (s): " timef
read -rep "step rate (Hz): " srate
read -rep "frame rate (Hz): " frate
read -rep "frame width (px): " fwidth
read -rep "frame height (px): " fheight
read -rep "frame center x (m): " fcenterx
read -rep "frame center y (m): " fcentery
read -rep "frame zoom: " fzoom
read -rep "frame scale: " fscale
read -rep "output dirname: " dirname
mkdir -p $dirname
rm -rf $dirname/*
mkdir -p $dirname/problems
mkdir -p $dirname/solutions
mkdir -p $dirname/prosols
mkdir -p $dirname/frames
mkdir -p $dirname/diagrams
echo "creating pipeline"
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
echo "> ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "running pipeline with parameters:
| ${fg_cyan}gravity${normal}=$gravity ${fg_gray}m/s^2${normal}
| ${fg_cyan}timef${normal}=$timef ${fg_gray}s${normal}
| ${fg_cyan}srate${normal}=$srate ${fg_gray}Hz${normal}
| ${fg_cyan}frate${normal}=$frate ${fg_gray}Hz${normal}
| ${fg_cyan}fsize${normal}=(${fwidth}x${fheight}) ${fg_gray}px${normal}
| ${fg_cyan}fcenter${normal}=($fcenterx $fcentery) ${fg_gray}m${normal}
| ${fg_cyan}fzoom${normal}=$fzoom
| ${fg_cyan}fscale${normal}=$fscale"
source $dirname/pipeline.sh
echo "> ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "stitching video frames together"
rm -rf $dirname/prosols
ffmpeg -r $frate -i $dirname/frames/%09d.png -y $dirname/video.mp4 -loglevel error
rm -rf $dirname/frames
ffmpeg -r $frate -i $dirname/diagrams/%09d.png -y $dirname/fdiagram.mp4 -loglevel error
rm -rf $dirname/diagrams
echo "> ${fg_white}${bg_green}TASK COMPLETE${normal}"
