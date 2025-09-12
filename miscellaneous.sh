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
read -rep "create new miscellaneous output? (warning: this will override the previous output) (y/n): " create_miscellaneous
if ! [[ "$create_miscellaneous" == "y" || "$create_miscellaneous" == "Y" || "$create_miscellaneous" == "n" || "$create_miscellaneous" == "N" ]]
then
	echo "error: unrecognized input">&2
	exit 1
fi
if [[ "$create_miscellaneous" == "n" || "$create_miscellaneous" == "N" ]]
then
	exit 0
fi
echo "* ${fg_yellow}creating${normal} output for each of the miscellaneous systems"
mkdir -p tmp/miscellaneous
rm -rf tmp/miscellaneous/*
echo "|\\"
echo "| * ${fg_yellow}working on${normal} Fink roof truss problem"
echo "| |\\"
update_start_time
./custom.sh filename=fink.txt gacceleration=9.8 timef=2.0 srate=10000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.5 fcentery=0.125 fzoom=1.0 fscale=1.0 dirname=tmp/miscellaneous/fink | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} scissor roof truss problem"
echo "| |\\"
update_start_time
./custom.sh filename=scissor.txt gacceleration=9.8 timef=3.0 srate=10000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=2.5 fzoom=0.1 fscale=1.0 dirname=tmp/miscellaneous/scissor | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} box cantilever problem"
echo "| |\\"
update_start_time
./custom.sh filename=cantilever.txt gacceleration=9.8 timef=3.0 srate=100000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.5 fcentery=0.1 fzoom=1.0 fscale=0.5 dirname=tmp/miscellaneous/cantilever | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} stadium truss problem"
echo "| |\\"
update_start_time
./custom.sh filename=stadium.txt gacceleration=0.0 timef=3.0 srate=1000000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=6.096 fcentery=-4.8006 fzoom=0.05 fscale=1.0 dirname=tmp/miscellaneous/stadium | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} power transmission line tower truss problem"
echo "| |\\"
update_start_time
./custom.sh filename=powertower.txt gacceleration=0.0 timef=10.0 srate=1000000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=4.8 fzoom=0.08 fscale=0.7 dirname=tmp/miscellaneous/powertower | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} power transmission line tower cross variant truss problem"
echo "| |\\"
update_start_time
./custom.sh filename=powertowercross.txt gacceleration=0.0 timef=4.0 srate=1000000.0 frate=60.0 fwidth=1920 fheight=1080 fcenterx=0.0 fcentery=4.8 fzoom=0.08 fscale=0.7 dirname=tmp/miscellaneous/powertowercross | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "${bold}* miscellaneous files can now be found in ${underline}$(pwd)/tmp/miscellaneous${normal}"
