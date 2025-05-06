#!/bin/bash
set -eo pipefail
shopt -s expand_aliases
alias gcc="gcc-14"
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
echo "* ${fg_yellow}compiling${normal} programs"
echo "|\\"
update_start_time
mkdir -p bin
echo -n "| * ${fg_yellow}compiling${normal} mass spring damper library object"
gcc -c src/msd.c -o bin/msd.o -DMSDAXES=2 -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} truss solver executable"
gcc src/solvetruss.c -o bin/solvetruss bin/msd.o -lm -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} truss renderer executable"
gcc src/rendertruss.c -o bin/rendertruss bin/msd.o -lm $(pkg-config --cflags --libs cairo) -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} truss force diagram renderer executable"
gcc src/forcediagram.c -o bin/forcediagram bin/msd.o -lm $(pkg-config --cflags --libs cairo) -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} truss utilities executable"
gcc src/trussutils.c -o bin/trussutils bin/msd.o -lm -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} truss pipeline feedback executable"
gcc src/feedback.c -o bin/feedback bin/msd.o -lm -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} truss pipeline generator executable"
gcc src/pipeline.c -o bin/pipeline -lm -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo -n "| * ${fg_yellow}compiling${normal} subtitles renderer executable"
gcc src/subtitles.c -o bin/subtitles -lm $(pkg-config --cflags --libs cairo) -std=c23
echo " > ${fg_white}${fg_green}[TASK COMPLETE]${normal}"
echo "|/"
echo "> ${fg_white}${fg_green}[TASK COMPLETE]${normal} - ${fg_blue_misc}$(print_elapsed_time)${normal}"
echo "${bold}* executables and object files can now be found in ${underline}$(pwd)/bin${normal}"
