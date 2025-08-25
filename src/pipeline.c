#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// STEPS TO CREATE SIMULATION PIPELINE
// 1. call "allocate_executable_and_input_output_paths(1000);"
//    to prepare string variables for modification.
// 2. manually set all global variables of this file from outside.
//    to find some of the parameters involving math:
//      * dtime = 1.0 / srate;
//      * stepf = ((int) floor(srate * timef)) - 1;
//      * framef = ((int) floor(frate * timef)) - 1;
// 3. call "start_pipeline(stdout);"
// 4. call the following lines in a while loop with this order with "continue_current_frame()" as the condition:
//    1. call "print_pipeline_rendertruss_command(stdout);"
//    2. call "print_pipeline_solvetruss_command(stdout);"
//    3. call "print_pipeline_concatenate_problem_and_solution_command(stdout);"
//    4. call "print_pipeline_forcediagram_command(stdout);"
//    5. call "print_pipeline_feedback_solution_into_problem_command(stdout);"
//    6. call "progress_current_frame_and_step_number();"
// 5. call "print_pipeline_sweptarea_command(stdout);"
// 6. call "free_executable_and_input_output_paths();"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

void allocate_executable_and_input_output_paths(int character_count_each)
{
	allocate_string(&solvetruss_executable, character_count_each);
	allocate_string(&rendertruss_executable, character_count_each);
	allocate_string(&forcediagram_executable, character_count_each);
	allocate_string(&sweptarea_executable, character_count_each);
	allocate_string(&trussutils_executable, character_count_each);
	allocate_string(&problem_filename, character_count_each);
	allocate_string(&output_dirname, character_count_each);
}

void free_executable_and_input_output_paths(void)
{
	free_string(solvetruss_executable);
	free_string(rendertruss_executable);
	free_string(forcediagram_executable);
	free_string(sweptarea_executable);
	free_string(trussutils_executable);
	free_string(problem_filename);
	free_string(output_dirname);
}

void reset_current_frame_and_step_number(void)
{
	step = 0, frame = 0;
}

bool continue_current_frame(void)
{
	return frame <= framef;
}

void progress_current_frame_and_step_number(void)
{
	frame++;
	while(step * (framef + 1) < frame * (stepf + 1))
		step++;
}

void print_pipeline_header_commands(FILE *command_stream)
{
	printf("#!/bin/bash\n");
	printf("set -eo pipefail\n");
}

void print_pipeline_manage_files_and_directories_commands(FILE *command_stream)
{
	printf("mkdir -p \"%s\"\n", output_dirname);
	printf("rm -rf \"%s/problems\" \"%s/solutions\" \"%s/prosols\" \"%s/frames\" \"%s/diagrams\"\n", output_dirname, output_dirname, output_dirname, output_dirname, output_dirname);
	printf("mkdir -p \"%s/problems\" \"%s/solutions\" \"%s/prosols\" \"%s/frames\" \"%s/diagrams\"\n", output_dirname, output_dirname, output_dirname, output_dirname, output_dirname);
	printf("cp \"%s\" \"%s/problems/%09d.txt\"\n", problem_filename, output_dirname, 1);
}

void start_pipeline(FILE *command_stream)
{
	print_pipeline_header_commands(command_stream);
	print_pipeline_manage_files_and_directories_commands(command_stream);
	reset_current_frame_and_step_number();
}

void print_pipeline_concatenate_problem_and_solution_command(FILE *command_stream)
{
	printf(
		"cat \"%s/problems/%09d.txt\" \"%s/solutions/%09d.txt\" > \"%s/prosols/%09d.txt\"\n",
		output_dirname, frame + 1, output_dirname, frame + 1, output_dirname, frame + 1
	);
}

void print_pipeline_feedback_solution_into_problem_command(FILE *command_stream)
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

void print_pipeline_solvetruss_command(FILE *command_stream)
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

void print_pipeline_rendertruss_command(FILE *command_stream)
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

void print_pipeline_forcediagram_command(FILE *command_stream)
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

void print_pipeline_sweptarea_command(FILE *command_stream)
{
	if(strlen(sweptarea_executable) > 0 && sweptarea_executable[0] == '/')
		printf(
			"cat \"%s/problems/\"* | \"%s\" \"%s/sweptarea.png\" fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le\n",
			output_dirname, sweptarea_executable, output_dirname, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale
		);
	else
		printf(
			"cat \"%s/problems/\"* | \"./%s\" \"%s/sweptarea.png\" fsize=%dx%d \"fcenter=(%.9le %.9le)\" fzoom=%.9le fscale=%.9le\n",
			output_dirname, sweptarea_executable, output_dirname, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale
		);
}
