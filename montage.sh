#!/bin/bash
set -eo pipefail
if [ "$TERM" != "dumb" ]; then
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
fi
echo "* ${fg_yellow}creating${normal} a montage of each of the systems"
echo "|\\"
read -rep "create new bridges output? (y/n): " create_bridges
if ! [[ "$create_bridges" == "y" || "$create_bridges" == "Y" || "$create_bridges" == "n" || "$create_bridges" == "N" ]]
then
	echo "error: unrecognized input">&2
	exit 1
fi
read -rep "create new miscellaneous output? (y/n): " create_miscellaneous
if ! [[ "$create_miscellaneous" == "y" || "$create_miscellaneous" == "Y" || "$create_miscellaneous" == "n" || "$create_miscellaneous" == "N" ]]
then
	echo "error: unrecognized input">&2
	exit 1
fi
read -rep "create new pendulums output? (y/n): " create_pendulums
if ! [[ "$create_pendulums" == "y" || "$create_pendulums" == "Y" || "$create_pendulums" == "n" || "$create_pendulums" == "N" ]]
then
	echo "error: unrecognized input">&2
	exit 1
fi
mkdir -p tmp/montage
rm -rf tmp/montage/*
if [[ "$create_bridges" == "y" || "$create_bridges" == "Y" ]]
then
	source bridges.sh | sed -u "s/^/| /"
fi
if [[ "$create_miscellaneous" == "y" || "$create_miscellaneous" == "Y" ]]
then
	echo "| |"
	source miscellaneous.sh | sed -u "s/^/| /"
fi
if [[ "$create_pendulums" == "y" || "$create_pendulums" == "Y" ]]
then
	echo "| |"
	source pendulums.sh | sed -u "s/^/| /"
fi
echo "| |"
echo "| * ${fg_yellow}overlaying${normal} videos, force diagrams and subtitles"
mkdir -p tmp/montage/bridges/warren
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Warren Bridge
" | ./bin/subtitles tmp/montage/bridges/warren/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/bridges/warren/video.mp4 \
	-i tmp/bridges/warren/fdiagram.mp4 \
	-i tmp/montage/bridges/warren/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/bridges/warren/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/bridges/pratt
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Pratt Bridge
" | ./bin/subtitles tmp/montage/bridges/pratt/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/bridges/pratt/video.mp4 \
	-i tmp/bridges/pratt/fdiagram.mp4 \
	-i tmp/montage/bridges/pratt/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/bridges/pratt/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/bridges/howe
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Howe Bridge
" | ./bin/subtitles tmp/montage/bridges/howe/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/bridges/howe/video.mp4 \
	-i tmp/bridges/howe/fdiagram.mp4 \
	-i tmp/montage/bridges/howe/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/bridges/howe/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/bridges/parker
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Parker Bridge
" | ./bin/subtitles tmp/montage/bridges/parker/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/bridges/parker/video.mp4 \
	-i tmp/bridges/parker/fdiagram.mp4 \
	-i tmp/montage/bridges/parker/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/bridges/parker/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/bridges/cambridge
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Cambridge Bridge
" | ./bin/subtitles tmp/montage/bridges/cambridge/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/bridges/cambridge/video.mp4 \
	-i tmp/bridges/cambridge/fdiagram.mp4 \
	-i tmp/montage/bridges/cambridge/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/bridges/cambridge/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/bridges/whipple
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Whipple Bridge
" | ./bin/subtitles tmp/montage/bridges/whipple/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/bridges/whipple/video.mp4 \
	-i tmp/bridges/whipple/fdiagram.mp4 \
	-i tmp/montage/bridges/whipple/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/bridges/whipple/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/fink
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Fink Roof
" | ./bin/subtitles tmp/montage/miscellaneous/fink/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/fink/video.mp4 \
	-i tmp/miscellaneous/fink/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/fink/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/fink/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/scissor
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Scissor Roof
" | ./bin/subtitles tmp/montage/miscellaneous/scissor/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/scissor/video.mp4 \
	-i tmp/miscellaneous/scissor/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/scissor/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/scissor/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/cantilever
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Box Cantilever
" | ./bin/subtitles tmp/montage/miscellaneous/cantilever/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/cantilever/video.mp4 \
	-i tmp/miscellaneous/cantilever/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/cantilever/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/cantilever/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/stadium
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Stadium
" | ./bin/subtitles tmp/montage/miscellaneous/stadium/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/stadium/video.mp4 \
	-i tmp/miscellaneous/stadium/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/stadium/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/stadium/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/powertower
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Power Transmission Tower
" | ./bin/subtitles tmp/montage/miscellaneous/powertower/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/powertower/video.mp4 \
	-i tmp/miscellaneous/powertower/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/powertower/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/powertower/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/powertowercross
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Power Transmission Tower
" | ./bin/subtitles tmp/montage/miscellaneous/powertowercross/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/powertowercross/video.mp4 \
	-i tmp/miscellaneous/powertowercross/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/powertowercross/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/powertowercross/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/miscellaneous/strandbeest
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Theo Jansen's Strandbeest
" | ./bin/subtitles tmp/montage/miscellaneous/strandbeest/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/miscellaneous/strandbeest/video.mp4 \
	-i tmp/miscellaneous/strandbeest/fdiagram.mp4 \
	-i tmp/montage/miscellaneous/strandbeest/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/miscellaneous/strandbeest/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/pendulums/pendulum
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Single Pendulum
" | ./bin/subtitles tmp/montage/pendulums/pendulum/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/pendulums/pendulum/video.mp4 \
	-i tmp/pendulums/pendulum/fdiagram.mp4 \
	-i tmp/montage/pendulums/pendulum/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/pendulums/pendulum/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/pendulums/doublependulum
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Double Pendulum
" | ./bin/subtitles tmp/montage/pendulums/doublependulum/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/pendulums/doublependulum/video.mp4 \
	-i tmp/pendulums/doublependulum/fdiagram.mp4 \
	-i tmp/montage/pendulums/doublependulum/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/pendulums/doublependulum/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/pendulums/doublependulumroller
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Double Pendulum with Roller Support
" | ./bin/subtitles tmp/montage/pendulums/doublependulumroller/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/pendulums/doublependulumroller/video.mp4 \
	-i tmp/pendulums/doublependulumroller/fdiagram.mp4 \
	-i tmp/montage/pendulums/doublependulumroller/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/pendulums/doublependulumroller/video.mp4 \
	-loglevel error
mkdir -p tmp/montage/pendulums/decuplependulum
echo "subtitles=1
center=(0.0 -0.4) lineheight=0.04 text=Decuple Pendulum
" | ./bin/subtitles tmp/montage/pendulums/decuplependulum/subtitles.png fsize=1920x1080
ffmpeg \
	-i tmp/pendulums/decuplependulum/video.mp4 \
	-i tmp/pendulums/decuplependulum/fdiagram.mp4 \
	-i tmp/montage/pendulums/decuplependulum/subtitles.png \
	-filter_complex "[0:v][1:v]concat=n=2:v=1[z];[z][2:v]overlay[v]" \
	-map "[v]" \
	-y tmp/montage/pendulums/decuplependulum/video.mp4 \
	-loglevel error
echo "| |"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "| |"
echo "| * ${fg_yellow}stitching${normal} each of the video solutions together"
ffmpeg \
	-i tmp/montage/bridges/warren/video.mp4 \
	-i tmp/montage/bridges/pratt/video.mp4 \
	-i tmp/montage/bridges/howe/video.mp4 \
	-i tmp/montage/bridges/parker/video.mp4 \
	-i tmp/montage/bridges/cambridge/video.mp4 \
	-i tmp/montage/bridges/whipple/video.mp4 \
	-i tmp/montage/miscellaneous/fink/video.mp4 \
	-i tmp/montage/miscellaneous/scissor/video.mp4 \
	-i tmp/montage/miscellaneous/cantilever/video.mp4 \
	-i tmp/montage/miscellaneous/stadium/video.mp4 \
	-i tmp/montage/miscellaneous/powertower/video.mp4 \
	-i tmp/montage/miscellaneous/powertowercross/video.mp4 \
	-i tmp/montage/miscellaneous/strandbeest/video.mp4 \
	-i tmp/montage/pendulums/pendulum/video.mp4 \
	-i tmp/montage/pendulums/doublependulum/video.mp4 \
	-i tmp/montage/pendulums/doublependulumroller/video.mp4 \
	-i tmp/montage/pendulums/decuplependulum/video.mp4 \
	-filter_complex "[0:v][1:v][2:v][3:v][4:v][5:v][6:v][7:v][8:v][9:v][10:v][11:v][12:v][13:v][14:v][15:v][16:v]concat=n=17:v=1[v]" \
	-map "[v]" \
	-y tmp/montage/video.mp4 \
	-loglevel error
echo "| |"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "| |"
echo "| * ${fg_yellow}stacking${normal} each of the video solutions together"
ffmpeg \
	-t 20 -stream_loop -1 -i tmp/montage/bridges/warren/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/bridges/pratt/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/bridges/howe/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/bridges/parker/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/bridges/cambridge/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/bridges/whipple/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/fink/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/scissor/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/cantilever/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/stadium/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/powertower/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/powertowercross/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/miscellaneous/strandbeest/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/pendulums/pendulum/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/pendulums/doublependulum/video.mp4 \
	-t 20 -stream_loop -1 -i tmp/montage/pendulums/doublependulumroller/video.mp4 \
	-filter_complex "\
		[0:v]scale=480:-1[v0]; \
		[1:v]scale=480:-1[v1]; \
		[2:v]scale=480:-1[v2]; \
		[3:v]scale=480:-1[v3]; \
		[4:v]scale=480:-1[v4]; \
		[5:v]scale=480:-1[v5]; \
		[6:v]scale=480:-1[v6]; \
		[7:v]scale=480:-1[v7]; \
		[8:v]scale=480:-1[v8]; \
		[9:v]scale=480:-1[v9]; \
		[10:v]scale=480:-1[v10]; \
		[11:v]scale=480:-1[v11]; \
		[12:v]scale=480:-1[v12]; \
		[13:v]scale=480:-1[v13]; \
		[14:v]scale=480:-1[v14]; \
		[15:v]scale=480:-1[v15]; \
		[v0][v1][v2][v3][v4][v5][v6][v7][v8][v9][v10][v11][v12][v13][v14][v15]xstack=inputs=16:layout=0_0|w0_0|w0+w1_0|w0+w1+w2_0|0_h0|w0_h0|w0+w1_h0|w0+w1+w2_h0|0_h0+h1|w0_h0+h1|w0+w1_h0+h1|w0+w1+w2_h0+h1|0_h0+h1+h2|w0_h0+h1+h2|w0+w1_h0+h1+h2|w0+w1+w2_h0+h1+h2:fill=black[v]" \
	-map "[v]" \
	-y tmp/montage/parallel.mp4 \
	-loglevel error
echo "| |"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "|"
echo "${bold}* montage files can now be found in ${underline}$(pwd)/tmp/montage${normal}"
