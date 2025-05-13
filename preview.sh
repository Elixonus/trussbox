#!/bin/bash
set -eo pipefail
if [ "$TERM" != "dumb" ]; then
	ncolors=$(tput colors)
	if test -n "$ncolors" && test $ncolors -ge 28; then
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
		fg_blue_misc="$(tput setaf 27)"
	fi
fi
update_start_time() {
	start_time=$(date +%s)
}
print_elapsed_time() {
	local current_time=$(date +%s)
	local delta_time=$((current_time - start_time))
	local hours=$((delta_time / 3600))
	local minutes=$(((delta_time % 3600) / 60))
	local seconds=$((delta_time % 60))
	printf "%02d:%02d:%02d\n" "$hours" "$minutes" "$seconds"
}
echo "* ${fg_yellow}creating${normal} preview media"
echo "|\\"
read -rep "create new montage? (y/n): " create_montage
if ! [[
	"$create_montage" == "y" || "$create_montage" == "Y" ||
	"$create_montage" == "n" || "$create_montage" == "N"
]]
then
	echo "error: unrecognized input">&2
	exit 1
fi
mkdir -p tmp/preview
rm -rf tmp/preview/*
if [[ "$create_montage" == "y" || "$create_montage" == "Y" ]]
then
	source montage.sh | sed -u "s/^/| /"
fi
echo "| * ${fg_yellow}working on${normal} preview example problem"
echo "| |\\"
update_start_time
cat warren.txt | sed "s/dampening=3.0e2/dampening=3.0e1/g" > warrenld.txt
echo "\
warrenld.txt
-9.8
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
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
rm -f warrenld.txt
echo "| * ${fg_yellow}copying${normal} preview media from ${underline}$(pwd)/tmp${normal} to ${underline}$(pwd)${normal}"
update_start_time
cp tmp/preview/video.mp4 preview.mp4
ffmpeg -sseof -3 -i tmp/preview/video.mp4 -vsync 0 -q:v 31 -update true -y preview.png -loglevel error
cp tmp/preview/fdiagram.mp4 previewfd.mp4
ffmpeg -sseof -3 -i tmp/preview/fdiagram.mp4 -vsync 0 -q:v 31 -update true -y previewfd.png -loglevel error
cp tmp/montage/video.mp4 previewmt.mp4
ffmpeg -i tmp/montage/miscellaneous/powertower/video.mp4 -vf "select=eq(n\,0)" -vframes 1 -y previewmt.png -loglevel error
cp tmp/montage/parallel.mp4 previewmtpl.mp4
ffmpeg -i tmp/montage/parallel.mp4 -vframes 1 -y previewmtpl.png -loglevel error
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "${bold}* preview files can now be found at ${underline}$(pwd)/preview*${normal}"
