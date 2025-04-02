#!/bin/bash
set -eo pipefail
ncolors=$(tput colors)
if test -n "$ncolors" && test $ncolors -ge 9; then
	bold="$(tput bold)"
	underline="$(tput smul)"
	standout="$(tput smso)"
	normal="$(tput sgr0)"
	black="$(tput setaf 0)";   fg_black="$(tput setaf 0)";   bg_black="$(tput setab 0)"
	red="$(tput setaf 1)";     fg_red="$(tput setaf 1)";     bg_red="$(tput setab 1)"
	green="$(tput setaf 2)";   fg_green="$(tput setaf 2)";   bg_green="$(tput setab 2)"
	yellow="$(tput setaf 3)";  fg_yellow="$(tput setaf 3)";  bg_yellow="$(tput setab 3)"
	blue="$(tput setaf 4)";    fg_blue="$(tput setaf 4)";    bg_blue="$(tput setab 4)"
	magenta="$(tput setaf 5)"; fg_magenta="$(tput setaf 5)"; bg_magenta="$(tput setab 5)"
	cyan="$(tput setaf 6)";    fg_cyan="$(tput setaf 6)";    bg_cyan="$(tput setab 6)"
	white="$(tput setaf 7)";   fg_white="$(tput setaf 7)";   bg_white="$(tput setab 7)"
	gray="$(tput setaf 8)";    fg_gray="$(tput setaf 8)";    bg_gray="$(tput setab 8)"
fi
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
	source montage.sh | sed -u 's/^/| /'
fi
echo "| solving and rendering preview example"
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
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
rm -f warrenld.txt
echo "| copying preview media from tmp/ to ./"
cp tmp/preview/video.mp4 preview.mp4
ffmpeg -sseof -3 -i tmp/preview/video.mp4 -vsync 0 -q:v 31 -update true -y preview.png -loglevel error
cp tmp/preview/fdiagram.mp4 previewfd.mp4
ffmpeg -sseof -3 -i tmp/preview/fdiagram.mp4 -vsync 0 -q:v 31 -update true -y previewfd.png -loglevel error
cp tmp/montage/video.mp4 previewmt.mp4
ffmpeg -sseof -3 -i tmp/montage/video.mp4 -vsync 0 -q:v 31 -update true -y previewmt.png -loglevel error
cp tmp/montage/parallel.mp4 previewmtpl.mp4
ffmpeg -sseof -3 -i tmp/montage/parallel.mp4 -vsync 0 -q:v 31 -update true -y previewmtpl.png -loglevel error
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "> ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "preview files can now be found at ${underline}./preview*${normal}"
