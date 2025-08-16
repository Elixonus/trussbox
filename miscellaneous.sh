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
echo "* ${fg_yellow}creating${normal} output for each of the miscellaneous systems"
echo "|\\"
mkdir -p tmp/miscellaneous
rm -rf tmp/miscellaneous/*
echo "| * ${fg_yellow}working on${normal} Fink roof truss problem"
echo "| |\\"
update_start_time
echo "\
fink.txt
-9.8
2.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/miscellaneous/fink
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} scissor roof truss problem"
echo "| |\\"
update_start_time
echo "\
scissor.txt
-9.8
3.0
10000.0
60.0
1920
1080
0.0
2.5
0.1
1.0
tmp/miscellaneous/scissor
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} box cantilever problem"
echo "| |\\"
update_start_time
echo "\
cantilever.txt
-9.8
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
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} stadium truss problem"
echo "| |\\"
update_start_time
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
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} power transmission line tower truss problem"
echo "| |\\"
update_start_time
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
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} power transmission line tower cross variant truss problem"
echo "| |\\"
update_start_time
echo "\
powertowercross.txt
0.0
4.0
1000000.0
60.0
1920
1080
0.0
4.8
0.08
0.7
tmp/miscellaneous/powertowercross
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} pump jack truss problem"
echo "| |\\"
update_start_time
echo "\
pumpjack.txt
0.0
7.0
1000000.0
60.0
1920
1080
20.0
13.0
0.03
1.0
tmp/miscellaneous/pumpjack
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} landing gear truss problem"
echo "| |\\"
update_start_time
echo "\
landinggear.txt
-9.8
7.0
1000000.0
60.0
1920
1080
15.0
12.0
0.05
1.0
tmp/miscellaneous/landinggear
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} Baker valve gear truss problem"
echo "| |\\"
update_start_time
echo "\
bakervalvegear.txt
0.0
10.0
1000000.0
60.0
1920
1080
22.0
10.0
0.035
1.0
tmp/miscellaneous/bakervalvegear
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "| * ${fg_yellow}working on${normal} strandbeest truss problem"
echo "| |\\"
update_start_time
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
tmp/miscellaneous/strandbeest
" | source custom.sh | sed -u "s/^/| | /"
echo "| |/"
echo "| > ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "${bold}* miscellaneous files can now be found in ${underline}$(pwd)/tmp/miscellaneous${normal}"
