#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *solvetruss_executable;
char *rendertruss_executable;
char *forcediagram_executable;
char *sweptarea_executable;
char *trussutils_executable;
char *problem_filename;
char *output_dirname;
double gacceleration;
double dtime;
double timef;
int step;
int stepf;
double srate;
int frame;
int framef;
double frate;
int fsize[2];
double fcenter[2];
double fzoom;
double fscale;

void allocate_string(char **string, int count)
{
	*string = malloc((count + 1) * sizeof(char));
}

void free_string(char *string)
{
	free(string);
}

void allocate_paths(int character_count_each)
{
	allocate_string(&solvetruss_executable, character_count_each);
	allocate_string(&rendertruss_executable, character_count_each);
	allocate_string(&forcediagram_executable, character_count_each);
	allocate_string(&sweptarea_executable, character_count_each);
	allocate_string(&trussutils_executable, character_count_each);
	allocate_string(&problem_filename, character_count_each);
	allocate_string(&output_dirname, character_count_each);
}

void free_paths(void)
{
	free_string(solvetruss_executable);
	free_string(rendertruss_executable);
	free_string(forcediagram_executable);
	free_string(sweptarea_executable);
	free_string(trussutils_executable);
	free_string(problem_filename);
	free_string(output_dirname);
}

void initialize_frame_and_step(void)
{
	step = 0, frame = 0;
}

bool check_frame(void)
{
	return frame <= framef;
}

void progress_frame_and_step(void)
{
	frame++;
	while(step * (framef + 1) < frame * (stepf + 1))
		step++;
}

void initialize_bash(FILE *command_stream)
{
	printf("#!/bin/bash\n");
	printf("set -eo pipefail\n");
}

void initialize_files_and_directories(FILE *command_stream)
{
	printf("mkdir -p \"%s\"\n", output_dirname);
	printf("rm -rf \"%s/problems\" \"%s/solutions\" \"%s/prosols\" \"%s/frames\" \"%s/diagrams\"\n", output_dirname, output_dirname, output_dirname, output_dirname, output_dirname);
	printf("mkdir -p \"%s/problems\" \"%s/solutions\" \"%s/prosols\" \"%s/frames\" \"%s/diagrams\"\n", output_dirname, output_dirname, output_dirname, output_dirname, output_dirname);
	printf("cp \"%s\" \"%s/problems/%09d.txt\"\n", problem_filename, output_dirname, 1);
}

void initialize(FILE *command_stream)
{
	initialize_bash(command_stream);
	initialize_files_and_directories(command_stream);
	initialize_frame_and_step();
}

void concatenate_problem_and_solution(FILE *command_stream)
{
	printf(
		"cat \"%s/problems/%09d.txt\" \"%s/solutions/%09d.txt\" > \"%s/prosols/%09d.txt\"\n",
		output_dirname, frame + 1, output_dirname, frame + 1, output_dirname, frame + 1
	);
}

void feedback_solution_into_problem(FILE *command_stream)
{
	if(strlen(trussutils_executable) > 0 && trussutils_executable[0] == '/')
		printf(
			"\"%s\" feedback < \"%s/prosols/%09d.txt\" > \"%s/problems/%09d.txt\"\n",
			trussutils_executable, output_dirname, frame + 1, output_dirname, frame + 2
		);
	else
		printf(
			"\"./%s\" feedback < \"%s/prosols/%09d.txt\" > \"%s/problems/%09d.txt\"\n",
			trussutils_executable, output_dirname, frame + 1, output_dirname, frame + 2
		);
}

void solvetruss(FILE *command_stream)
{
	if(strlen(solvetruss_executable) > 0 && solvetruss_executable[0] == '/')
		printf(
			"\"%s\" gacceleration=%.9le timef=%.9le srate=%.9le < \"%s/problems/%09d.txt\" > \"%s/solutions/%09d.txt\"\n",
			solvetruss_executable, gacceleration, timef / ((double) (framef + 1)), srate, output_dirname, frame + 1, output_dirname, frame + 1
		);
	else
		printf(
			"\"./%s\" gacceleration=%.9le timef=%.9le srate=%.9le < \"%s/problems/%09d.txt\" > \"%s/solutions/%09d.txt\"\n",
			solvetruss_executable, gacceleration, timef / ((double) (framef + 1)), srate, output_dirname, frame + 1, output_dirname, frame + 1
		);
}

void rendertruss(FILE *command_stream)
{
	if(strlen(rendertruss_executable) > 0 && rendertruss_executable[0] == '/')
		printf(
			"\"%s\" \"%s/frames/%09d.png\" fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le < \"%s/problems/%09d.txt\"\n",
			rendertruss_executable, output_dirname, frame + 1, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, output_dirname, frame + 1
		);
	else
		printf(
			"\"./%s\" \"%s/frames/%09d.png\" fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le < \"%s/problems/%09d.txt\"\n",
			rendertruss_executable, output_dirname, frame + 1, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, output_dirname, frame + 1
		);
}

void forcediagram(FILE *command_stream)
{
	if(strlen(forcediagram_executable) > 0 && forcediagram_executable[0] == '/')
		printf(
			"\"%s\" \"%s/diagrams/%09d.png\" gacceleration=%.9le fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le < \"%s/prosols/%09d.txt\"\n",
			forcediagram_executable, output_dirname, frame + 1, gacceleration, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, output_dirname, frame + 1
		);
	else
		printf(
			"\"./%s\" \"%s/diagrams/%09d.png\" gacceleration=%.9le fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le < \"%s/prosols/%09d.txt\"\n",
			forcediagram_executable, output_dirname, frame + 1, gacceleration, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, output_dirname, frame + 1
		);
}

void sweptarea(FILE *command_stream)
{
	if(strlen(sweptarea_executable) > 0 && sweptarea_executable[0] == '/')
		printf(
			"cat \"%s/problems/\"* | \"%s\" \"%s/sweptarea.png\" fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le fcount=%d\n",
			output_dirname, sweptarea_executable, output_dirname, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, frame + 1
		);
	else
		printf(
			"cat \"%s/problems/\"* | \"./%s\" \"%s/sweptarea.png\" fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le fcount=%d\n",
			output_dirname, sweptarea_executable, output_dirname, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, frame + 1
		);
}
