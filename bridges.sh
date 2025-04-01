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
echo "solving and rendering each of the bridges"
mkdir -p tmp/bridges
rm -rf tmp/bridges/*
echo "| solving and rendering warren bridge"
echo "\
warren.txt
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
tmp/bridges/warren
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| solving and rendering pratt bridge"
echo "\
pratt.txt
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
tmp/bridges/pratt
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| solving and rendering howe bridge"
echo "\
howe.txt
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
tmp/bridges/howe
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| solving and rendering parker bridge"
echo "\
parker.txt
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
tmp/bridges/parker
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| solving and rendering cambridge bridge"
echo "\
cambridge.txt
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
tmp/bridges/cambridge
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "| solving and rendering whipple bridge"
echo "\
whipple.txt
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
tmp/bridges/whipple
" | source custom.sh | sed -u 's/^/| | /'
echo "| > ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "> ${fg_white}${bg_green}TASK COMPLETE${normal}"
echo "bridge files can now be found in ${underline}tmp/bridges/${normal}"
