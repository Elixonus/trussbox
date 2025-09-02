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
read -rep "create new pendulums output? (warning: this will override the previous output) (y/n): " create_pendulums
if ! [[ "$create_pendulums" == "y" || "$create_pendulums" == "Y" || "$create_pendulums" == "n" || "$create_pendulums" == "N" ]]
then
	echo "error: unrecognized input">&2
	exit 1
fi
if [[ "$create_pendulums" == "n" || "$create_pendulums" == "N" ]]
then
	exit 0
fi
echo "* ${fg_yellow}creating${normal} output for each of the pendulums"
mkdir -p tmp/pendulums
rm -rf tmp/pendulums/*
echo "|\\"
echo "| * ${fg_yellow}working on${normal} single pendulum problem"
echo "| |\\"
update_start_time
./custom.sh filename=pendulum.txt gacceleration=-9.8 timef=10.0 srate=10000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=0.0 fzoom=1.5 fscale=1.0 dirname=tmp/pendulums/pendulum | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} double pendulum problem"
echo "| |\\"
update_start_time
./custom.sh filename=doublependulum.txt gacceleration=-9.8 timef=10.0 srate=10000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=0.0 fzoom=0.75 fscale=1.0 dirname=tmp/pendulums/doublependulum | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} double pendulum (roller) problem"
echo "| |\\"
update_start_time
./custom.sh filename=doublependulumroller.txt gacceleration=-9.8 timef=10.0 srate=10000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=0.0 fzoom=0.75 fscale=1.0 dirname=tmp/pendulums/doublependulumroller | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} decuple pendulum problem"
echo "| |\\"
update_start_time
./custom.sh filename=decuplependulum.txt gacceleration=-9.8 timef=10.0 srate=10000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=-1.35 fzoom=0.25 fscale=1.0 dirname=tmp/pendulums/decuplependulum | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "${bold}* pendulum files can now be found in ${underline}$(pwd)/tmp/pendulums${normal}"
