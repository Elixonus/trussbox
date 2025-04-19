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
echo "* solving and rendering each of the miscellaneous systems"
echo "|\\"
mkdir -p tmp/miscellaneous
rm -rf tmp/miscellaneous/*
echo "| * solving and rendering roof truss"
echo "| |\\"
echo "\
roof.txt
9.8
2.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/miscellaneous/roof
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| * solving and rendering cantilever"
echo "| |\\"
echo "\
cantilever.txt
9.8
3.0
100000.0
60.0
1920
1080
0.5
0.1
1.0
0.5
tmp/miscellaneous/cantilever
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| * solving and rendering stadium truss"
echo "| |\\"
echo "\
stadium.txt
0.0
3.0
1000000.0
60.0
1920
1080
6.096
-4.8006
0.05
1.0
tmp/miscellaneous/stadium
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| * solving and rendering power transmission line tower truss"
echo "| |\\"
echo "\
powertower.txt
0.0
10.0
1000000.0
60.0
1920
1080
0.0
4.8
0.08
0.7
tmp/miscellaneous/powertower
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "|/"
echo "> ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "* miscellaneous files can now be found in ${underline}tmp/miscellaneous/${normal}"
