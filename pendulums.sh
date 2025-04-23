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
echo "* ${fg_yellow}creating${normal} output for each of the pendulums"
echo "|\\"
mkdir -p tmp/pendulums
rm -rf tmp/pendulums/*
echo "| * ${fg_yellow}working on${normal} single pendulum problem"
echo "| |\\"
echo "\
pendulum.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
0.0
1.5
1.0
tmp/pendulums/pendulum
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "| |"
echo "| * ${fg_yellow}working on${normal} double pendulum problem"
echo "| |\\"
echo "\
doublependulum.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
0.0
0.75
1.0
tmp/pendulums/doublependulum
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "| |"
echo "| * ${fg_yellow}working on${normal} double pendulum (roller) problem"
echo "| |\\"
echo "\
doublependulumroller.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
0.0
0.75
1.0
tmp/pendulums/doublependulumroller
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "| |"
echo "| * ${fg_yellow}working on${normal} decuple pendulum problem"
echo "| |\\"
echo "\
decuplependulum.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
-1.35
0.25
1.0
tmp/pendulums/decuplependulum
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "|"
echo "${bold}* pendulum files can now be found in ${underline}$(pwd)/tmp/pendulums${normal}"
