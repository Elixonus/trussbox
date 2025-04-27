#include <stdio.h>
#include <math.h>

char solvetruss_executable[1001];
char rendertruss_executable[1001];
char forcediagram_executable[1001];
char feedback_executable[1001];
char problem_filename[1001];
char problems_dirname[1001];
char solutions_dirname[1001];
char prosols_dirname[1001];
char frames_dirname[1001];
char diagrams_dirname[1001];
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
double epsilon = 1.0e-18;

int main(int argc, char **argv)
{
	if(argc != 19)
	{
		fprintf(stderr, "error: count: arguments: %d of 19 provided\n", argc);
		fprintf(stderr, "usage: arguments: %s solvetruss_executable=string rendertruss_executable=string forcediagram_executable=string feedback_executable=string problem_filename=string problems_dirname=string solutions_dirname=string prosols_dirname=string frames_dirname=string diagrams_dirname=string gacceleration=float timef=float srate=float frate=float fsize=integerxinteger fcenter=(float float) fzoom=float fscale=float\n", argv[0]);
		return 1;
	}
	if(sscanf(argv[1], "solvetruss_executable=%1000s", solvetruss_executable) != 1)
	{
		fprintf(stderr, "error: parse: solvetruss_executable argument (1)\n");
		fprintf(stderr, "usage: solvetruss_executable argument (1): solvetruss_executable=string\n");
		return 1;
	}
	if(sscanf(argv[2], "rendertruss_executable=%1000s", rendertruss_executable) != 1)
	{
		fprintf(stderr, "error: parse: rendertruss_executable argument (2)\n");
		fprintf(stderr, "usage: rendertruss_executable argument (2): rendertruss_executable=string\n");
		return 1;
	}
	if(sscanf(argv[3], "forcediagram_executable=%1000s", forcediagram_executable) != 1)
	{
		fprintf(stderr, "error: parse: forcediagram_executable argument (3)\n");
		fprintf(stderr, "usage: forcediagram_executable argument (3): forcediagram_executable=string\n");
		return 1;
	}
	if(sscanf(argv[4], "feedback_executable=%1000s", feedback_executable) != 1)
	{
		fprintf(stderr, "error: parse: feedback_executable argument (4)\n");
		fprintf(stderr, "usage: feedback_executable argument (4): feedback_executable=string\n");
		return 1;
	}
	if(sscanf(argv[5], "problem_filename=%1000s", problem_filename) != 1)
	{
		fprintf(stderr, "error: parse: problem_filename argument (5)\n");
		fprintf(stderr, "usage: problem_filename argument (5): problem_filename=string\n");
		return 1;
	}
	if(sscanf(argv[6], "problems_dirname=%1000s", problems_dirname) != 1)
	{
		fprintf(stderr, "error: parse: problems_dirname argument (6)\n");
		fprintf(stderr, "usage: problems_dirname argument (6): problems_dirname=string\n");
		return 1;
	}
	if(sscanf(argv[7], "solutions_dirname=%1000s", solutions_dirname) != 1)
	{
		fprintf(stderr, "error: parse: solutions_dirname argument (7)\n");
		fprintf(stderr, "usage: solutions_dirname argument (7): solutions_dirname=string\n");
		return 1;
	}
	if(sscanf(argv[8], "prosols_dirname=%1000s", prosols_dirname) != 1)
	{
		fprintf(stderr, "error: parse: prosols_dirname argument (8)\n");
		fprintf(stderr, "usage: prosols_dirname argument (8): prosols_dirname=string\n");
		return 1;
	}
	if(sscanf(argv[9], "frames_dirname=%1000s", frames_dirname) != 1)
	{
		fprintf(stderr, "error: parse: frames_dirname argument (9)\n");
		fprintf(stderr, "usage: frames_dirname argument (9): frames_dirname=string\n");
		return 1;
	}
	if(sscanf(argv[10], "diagrams_dirname=%1000s", diagrams_dirname) != 1)
	{
		fprintf(stderr, "error: parse: diagrams_dirname argument (10)\n");
		fprintf(stderr, "usage: diagrams_dirname argument (10): diagrams_dirname=string\n");
		return 1;
	}
	if(sscanf(argv[11], "gacceleration=%lf", &gacceleration) != 1)
	{
		fprintf(stderr, "error: parse: gacceleration argument (11)\n");
		fprintf(stderr, "usage: gacceleration argument (11): gacceleration=float\n");
		return 1;
	}
	if(sscanf(argv[12], "timef=%lf", &timef) != 1)
	{
		fprintf(stderr, "error: parse: timef argument (12)\n");
		fprintf(stderr, "usage: timef argument (12): timef=float\n");
		return 1;
	}
	if(timef < epsilon)
	{
		fprintf(stderr, "error: limit: timef argument: %.1e not greater than %.1e\n", timef, epsilon);
		return 1;
	}
	if(sscanf(argv[13], "srate=%lf", &srate) != 1)
	{
		fprintf(stderr, "error: parse: srate argument (13)\n");
		fprintf(stderr, "usage: srate argument (13): srate=float\n");
		return 1;
	}
	if(srate < epsilon)
	{
		fprintf(stderr, "error: limit: srate argument: %.1e not greater than %.1e\n", srate, epsilon);
		return 1;
	}
	dtime = 1.0 / srate;
	if(dtime < epsilon)
	{
		fprintf(stderr, "error: limit: dtime variable: %.1e not greater than %.1e\n", dtime, epsilon);
		return 1;
	}
	stepf = ((int) round(srate * timef)) - 1;
	if(stepf < 0)
	{
		fprintf(stderr, "error: limit: stepf variable: %d not positive\n", stepf);
		return 1;
	}
	if(sscanf(argv[14], "frate=%lf", &frate) != 1)
	{
		fprintf(stderr, "error: parse: frate argument (14)\n");
		fprintf(stderr, "usage: frate argument (14): frate=float\n");
		return 1;
	}
	if(frate < epsilon)
	{
		fprintf(stderr, "error: limit: frate argument: %.1e not greater than %.1e\n", frate, epsilon);
		return 1;
	}
	framef = ((int) round(frate * timef)) - 1;
	if(framef < 0)
	{
		fprintf(stderr, "error: limit: framef variable: %d not positive\n", framef);
		return 1;
	}
	if(sscanf(argv[15], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
	{
		fprintf(stderr, "error: parse: fsize argument (15)\n");
		fprintf(stderr, "usage: fsize argument (15): fsize=integerxinteger\n");
		return 1;
	}
	if(fsize[0] < 64 || fsize[1] < 64)
	{
		fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than 64x64 nor matching\n", fsize[0], fsize[1]);
		return 1;
	}
	if(sscanf(argv[16], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument (16)\n");
		fprintf(stderr, "usage: fcenter argument (16): fcenter=(float float)\n");
		return 1;
	}
	if(sscanf(argv[17], "fzoom=%lf", &fzoom) != 1)
	{
		fprintf(stderr, "error: parse: fzoom argument (17)\n");
		fprintf(stderr, "usage: fzoom argument (17): fzoom=float\n");
		return 1;
	}
	if(fzoom < epsilon)
	{
		fprintf(stderr, "error: limit: fzoom argument: %.1e not greater than %.1e\n", fzoom, epsilon);
		return 1;
	}
	if(sscanf(argv[18], "fscale=%lf", &fscale) != 1)
	{
		fprintf(stderr, "error: parse: fscale argument (18)\n");
		fprintf(stderr, "usage: fscale argument (18): fscale=float\n");
		return 1;
	}
	if(fscale < epsilon)
	{
		fprintf(stderr, "error: limit: fscale argument: %.1e not greater than %.1e\n", fscale, epsilon);
		return 1;
	}
	printf("#!/bin/bash\n");
	printf("set -eo pipefail\n");
	printf("cp \"%s\" \"%s/%09d.txt\"\n", problem_filename, problems_dirname, 1);
	step = 0;
	frame = 0;
	while(frame <= framef)
	{
		printf(
			"\"./%s\" \"%s/%09d.png\" fsize=%dx%d \"fcenter=(%.9e %.9e)\" fzoom=%.9e fscale=%.9e < \"%s/%09d.txt\"\n",
			rendertruss_executable, frames_dirname, frame + 1, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, problems_dirname, frame + 1
		);
		printf(
			"\"./%s\" gacceleration=%.9e timef=%.9e srate=%.9e < \"%s/%09d.txt\" > \"%s/%09d.txt\"\n",
			solvetruss_executable, gacceleration, timef / ((double) (framef + 1)), srate, problems_dirname, frame + 1, solutions_dirname, frame + 1
		);
		printf(
			"cat \"%s/%09d.txt\" \"%s/%09d.txt\" > \"%s/%09d.txt\"\n",
			problems_dirname, frame + 1, solutions_dirname, frame + 1, prosols_dirname, frame + 1
		);
		printf(
			"\"./%s\" \"%s/%09d.png\" gacceleration=%.9e fsize=%dx%d \"fcenter=(%.9e %.9e)\" fzoom=%.9e fscale=%.9e < \"%s/%09d.txt\"\n",
			forcediagram_executable, diagrams_dirname, frame + 1, gacceleration, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, prosols_dirname, frame + 1
		);
		printf(
			"\"./%s\" < \"%s/%09d.txt\" > \"%s/%09d.txt\"\n",
			feedback_executable, prosols_dirname, frame + 1, problems_dirname, frame + 2
		);
		frame++;
		while(step * (framef + 1) < frame * (stepf + 1))
		{
			step++;
		}
	}
}
