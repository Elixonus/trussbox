#include <stdio.h>
#include <string.h>
#include <math.h>

#define EPSILON 1.0e-18

char solvetruss_executable[1001];
char rendertruss_executable[1001];
char forcediagram_executable[1001];
char trussutils_executable[1001];
char problem_filename[1001];
char output_dirname[1001];
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

int main(int argc, char **argv)
{
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
	if(argc < 5 || sscanf(argv[4], "trussutils_executable=%1000s", trussutils_executable) != 1)
	{
		fprintf(stderr, "error: parse: trussutils_executable argument\n");
		fprintf(stderr, "usage: trussutils_executable argument: trussutils_executable=string\n");
		return 1;
	}
	if(argc < 6 || sscanf(argv[5], "problem_filename=%1000s", problem_filename) != 1)
	{
		fprintf(stderr, "error: parse: problem_filename argument\n");
		fprintf(stderr, "usage: problem_filename argument: problem_filename=string\n");
		return 1;
	}
	if(argc < 7 || sscanf(argv[6], "output_dirname=%1000s", output_dirname) != 1)
	{
		fprintf(stderr, "error: parse: output_dirname argument\n");
		fprintf(stderr, "usage: output_dirname argument: output_dirname=string\n");
		return 1;
	}
	if(argc < 8 || sscanf(argv[7], "gacceleration=%le", &gacceleration) != 1)
	{
		fprintf(stderr, "error: parse: gacceleration argument\n");
		fprintf(stderr, "usage: gacceleration argument: gacceleration=float\n");
		return 1;
	}
	if(argc < 9 || sscanf(argv[8], "timef=%le", &timef) != 1)
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
	if(argc < 10 || sscanf(argv[9], "srate=%le", &srate) != 1)
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
	if(argc < 11 || sscanf(argv[10], "frate=%le", &frate) != 1)
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
	if(argc < 12 || sscanf(argv[11], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
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
	if(argc < 13 || sscanf(argv[12], "fcenter=(%le %le)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument\n");
		fprintf(stderr, "usage: fcenter argument: fcenter=(float float)\n");
		return 1;
	}
	if(argc < 14 || sscanf(argv[13], "fzoom=%le", &fzoom) != 1)
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
	if(argc < 15 || sscanf(argv[14], "fscale=%le", &fscale) != 1)
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
	printf("#!/bin/bash\n");
	printf("set -eo pipefail\n");
	printf("mkdir -p \"%s\"\n", output_dirname);
	printf("rm -rf \"%s/problems\" \"%s/solutions\" \"%s/prosols\" \"%s/frames\" \"%s/diagrams\"\n", output_dirname, output_dirname, output_dirname, output_dirname, output_dirname);
	printf("mkdir -p \"%s/problems\" \"%s/solutions\" \"%s/prosols\" \"%s/frames\" \"%s/diagrams\"\n", output_dirname, output_dirname, output_dirname, output_dirname, output_dirname);
	printf("cp \"%s\" \"%s/problems/%09d.txt\"\n", problem_filename, output_dirname, 1);
	step = 0, frame = 0;
	while(frame <= framef)
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
		printf(
			"cat \"%s/problems/%09d.txt\" \"%s/solutions/%09d.txt\" > \"%s/prosols/%09d.txt\"\n",
			output_dirname, frame + 1, output_dirname, frame + 1, output_dirname, frame + 1
		);
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
		frame++;
		while(step * (framef + 1) < frame * (stepf + 1))
			step++;
	}
}
