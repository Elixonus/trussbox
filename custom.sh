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
read -rep "problem filename: " filename
read -rep "gravitational acceleration (m/s^2): " gacceleration
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
rm -rf $dirname
mkdir -p $dirname
echo "* ${fg_yellow}creating${normal} a pipeline for the problem"
update_start_time
./bin/pipeline \
	solvetruss_executable=bin/solvetruss \
	rendertruss_executable=bin/rendertruss \
	forcediagram_executable=bin/forcediagram \
	feedback_executable=bin/feedback \
	problem_filename=$filename \
	output_dirname=$dirname \
	gacceleration=$gacceleration \
	timef=$timef \
	srate=$srate \
	frate=$frate \
	fsize=${fwidth}x${fheight} \
	"fcenter=($fcenterx $fcentery)" \
	fzoom=$fzoom \
	fscale=$fscale \
	> $dirname/pipeline.sh
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "* ${fg_yellow}running${normal} pipeline with parameters:
| ${fg_cyan}gacceleration${normal}=$gacceleration ${fg_gray}m/s^2${normal}
| ${fg_cyan}timef${normal}=$timef ${fg_gray}s${normal}
| ${fg_cyan}srate${normal}=$srate ${fg_gray}Hz${normal}
| ${fg_cyan}frate${normal}=$frate ${fg_gray}Hz${normal}
| ${fg_cyan}fsize${normal}=(${fwidth} ${fg_gray}px${normal})x(${fheight} ${fg_gray}px${normal})
| ${fg_cyan}fcenter${normal}=($fcenterx $fcentery) ${fg_gray}m${normal}
| ${fg_cyan}fzoom${normal}=$fzoom
| ${fg_cyan}fscale${normal}=$fscale"
update_start_time
source $dirname/pipeline.sh
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "* ${fg_yellow}stitching${normal} video frames together"
update_start_time
ffmpeg -r "$frate" -i $dirname/frames/%09d.png -y $dirname/video.mp4 -loglevel error
rm -rf $dirname/frames
ffmpeg -r "$frate" -i $dirname/diagrams/%09d.png -y $dirname/fdiagram.mp4 -loglevel error
rm -rf $dirname/diagrams
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
textzoom=$(awk "BEGIN{print 0.8 * ${fzoom}}")
./bin/trussutils textart "fcenter=($fcenterx $fcentery)" "fzoom=$textzoom" color=true vcrop=true "title=ASCII Text Art Truss Representation" < "$dirname/prosols/$(ls $dirname/prosols | tail -n 1)"
rm -rf "$dirname/prosols"
echo "${bold}* output files can now be found in ${underline}$(pwd)/$dirname${normal}"
