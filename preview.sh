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
echo "* ${fg_yellow}preparing${normal} preview media"
echo "|\\"
source montage.sh | sed -u "s/^/| /"
cp tmp/montage/video.mp4 previewmontage.mp4
ffmpeg -i tmp/montage/miscellaneous/powertower/video.mp4 -vframes 1 -y previewmontage.png -loglevel error
cp tmp/montage/parallel.mp4 previewmontageparallel.mp4
ffmpeg -i tmp/montage/parallel.mp4 -vframes 1 -y previewmontageparallel.png -loglevel error
echo "\
strandbeest.txt
0.0
8.0
100000.0
60.0
1920
1080
0.0
-0.5
0.2
1.0
tmp/previewexample
" | source custom.sh > /dev/null
ffmpeg -i tmp/previewexample/video.mp4 -vframes 1 -y previewexampleframe.png -loglevel error
ffmpeg -i tmp/previewexample/fdiagram.mp4 -vframes 1 -y previewexamplefdiagram.png -loglevel error
cp tmp/previewexample/sweptarea.png previewexamplesweptarea.png
rm -rf tmp/previewexample
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "${bold}* preview files can now be found at ${underline}$(pwd)/preview*${normal}"
