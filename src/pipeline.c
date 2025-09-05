#include <stdio.h>
#include <math.h>

#include "lib/pipeline.h"

#define EPSILON 1.0e-18

int main(int argc, char **argv)
{
	allocate_executable_and_input_output_paths(1000);
	if(argc < 2 || sscanf(argv[1], "solvetruss_executable=%1000s", solvetruss_executable) != 1)
	{
		fprintf(stderr, "error: parse: solvetruss_executable argument\n");
		fprintf(stderr, "usage: solvetruss_executable argument: solvetruss_executable=string\n");
		return 1;
	}
	if(argc < 3 || sscanf(argv[2], "rendertruss_executable=%1000s", rendertruss_executable) != 1)
	{
		fprintf(stderr, "error: parse: rendertruss_executable argument\n");
		fprintf(stderr, "usage: rendertruss_executable argument: rendertruss_executable=string\n");
		return 1;
	}
	if(argc < 4 || sscanf(argv[3], "forcediagram_executable=%1000s", forcediagram_executable) != 1)
	{
		fprintf(stderr, "error: parse: forcediagram_executable argument\n");
		fprintf(stderr, "usage: forcediagram_executable argument: forcediagram_executable=string\n");
		return 1;
	}
	if(argc < 5 || sscanf(argv[4], "sweptarea_executable=%1000s", sweptarea_executable) != 1)
	{
		fprintf(stderr, "error: parse: sweptarea_executable argument\n");
		fprintf(stderr, "usage: sweptarea_executable argument: sweptarea_executable=string\n");
		return 1;
	}
	if(argc < 6 || sscanf(argv[5], "feedback_executable=%1000s", feedback_executable) != 1)
	{
		fprintf(stderr, "error: parse: feedback_executable argument\n");
		fprintf(stderr, "usage: feedback_executable argument: feedback_executable=string\n");
		return 1;
	}
	if(argc < 7 || sscanf(argv[6], "problem_filename=%1000s", problem_filename) != 1)
	{
		fprintf(stderr, "error: parse: problem_filename argument\n");
		fprintf(stderr, "usage: problem_filename argument: problem_filename=string\n");
		return 1;
	}
	if(argc < 8 || sscanf(argv[7], "output_dirname=%1000s", output_dirname) != 1)
	{
		fprintf(stderr, "error: parse: output_dirname argument\n");
		fprintf(stderr, "usage: output_dirname argument: output_dirname=string\n");
		return 1;
	}
	if(argc < 9 || sscanf(argv[8], "gacceleration=%le", &gacceleration) != 1)
	{
		fprintf(stderr, "error: parse: gacceleration argument\n");
		fprintf(stderr, "usage: gacceleration argument: gacceleration=float\n");
		return 1;
	}
	if(argc < 10 || sscanf(argv[9], "timef=%le", &timef) != 1)
	{
		fprintf(stderr, "error: parse: timef argument\n");
		fprintf(stderr, "usage: timef argument: timef=float\n");
		return 1;
	}
	if(timef < EPSILON)
	{
		fprintf(stderr, "error: limit: timef argument: %.1le not greater than %.1le\n", timef, EPSILON);
		return 1;
	}
	if(argc < 11 || sscanf(argv[10], "srate=%le", &srate) != 1)
	{
		fprintf(stderr, "error: parse: srate argument\n");
		fprintf(stderr, "usage: srate argument: srate=float\n");
		return 1;
	}
	if(srate < EPSILON)
	{
		fprintf(stderr, "error: limit: srate argument: %.1le not greater than %.1le\n", srate, EPSILON);
		return 1;
	}
	dtime = 1.0 / srate;
	stepf = ((int) floor(srate * timef)) - 1;
	if(stepf < 0)
	{
		fprintf(stderr, "error: limit: stepf variable: %d not positive\n", stepf + 1);
		return 1;
	}
	if(argc < 12 || sscanf(argv[11], "frate=%le", &frate) != 1)
	{
		fprintf(stderr, "error: parse: frate argument\n");
		fprintf(stderr, "usage: frate argument: frate=float\n");
		return 1;
	}
	if(frate < EPSILON)
	{
		fprintf(stderr, "error: limit: frate argument: %.1le not greater than %.1le\n", frate, EPSILON);
		return 1;
	}
	framef = ((int) floor(frate * timef)) - 1;
	if(framef < 0)
	{
		fprintf(stderr, "error: limit: framef variable: %d not positive\n", framef + 1);
		return 1;
	}
	if(argc < 13 || sscanf(argv[12], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
	{
		fprintf(stderr, "error: parse: fsize argument\n");
		fprintf(stderr, "usage: fsize argument: fsize=integerxinteger\n");
		return 1;
	}
	if(fsize[0] < 64 || fsize[1] < 64)
	{
		fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than %dx%d nor matching\n", fsize[0], fsize[1], 64, 64);
		return 1;
	}
	if(argc < 14 || sscanf(argv[13], "fcenter=(%le %le)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument\n");
		fprintf(stderr, "usage: fcenter argument: fcenter=(float float)\n");
		return 1;
	}
	if(argc < 15 || sscanf(argv[14], "fzoom=%le", &fzoom) != 1)
	{
		fprintf(stderr, "error: parse: fzoom argument\n");
		fprintf(stderr, "usage: fzoom argument: fzoom=float\n");
		return 1;
	}
	if(fzoom < EPSILON)
	{
		fprintf(stderr, "error: limit: fzoom argument: %.1le not greater than %.1le\n", fzoom, EPSILON);
		return 1;
	}
	if(argc < 16 || sscanf(argv[15], "fscale=%le", &fscale) != 1)
	{
		fprintf(stderr, "error: parse: fscale argument\n");
		fprintf(stderr, "usage: fscale argument: fscale=float\n");
		return 1;
	}
	if(fscale < EPSILON)
	{
		fprintf(stderr, "error: limit: fscale argument: %.1le not greater than %.1le\n", fscale, EPSILON);
		return 1;
	}
	start_pipeline(stdout);
	while(continue_current_frame())
	{
		print_pipeline_rendertruss_command(stdout);
		print_pipeline_solvetruss_command(stdout);
		print_pipeline_concatenate_problem_and_solution_command(stdout);
		print_pipeline_forcediagram_command(stdout);
		print_pipeline_feedback_solution_into_problem_command(stdout);
		progress_current_frame_and_step_number();
	}
	print_pipeline_sweptarea_command(stdout);
	free_executable_and_input_output_paths();
	return 0;
}
