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
echo "* ${fg_yellow}creating${normal} output for each of the bridges"
echo "|\\"
mkdir -p tmp/bridges
rm -rf tmp/bridges/*
echo "| * ${fg_yellow}working on${normal} warren bridge problem"
echo "| |\\"
update_start_time
echo "\
warren.txt
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
tmp/bridges/warren
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} pratt bridge problem"
echo "| |\\"
update_start_time
echo "\
pratt.txt
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
tmp/bridges/pratt
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} howe bridge problem"
echo "| |\\"
update_start_time
echo "\
howe.txt
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
tmp/bridges/howe
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} parker bridge problem"
echo "| |\\"
update_start_time
echo "\
parker.txt
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
tmp/bridges/parker
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} cambridge bridge problem"
echo "| |\\"
update_start_time
echo "\
cambridge.txt
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
tmp/bridges/cambridge
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} whipple bridge problem"
echo "| |\\"
update_start_time
echo "\
whipple.txt
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
tmp/bridges/whipple
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "${bold}* bridge files can now be found in ${underline}$(pwd)/tmp/bridges${normal}"
