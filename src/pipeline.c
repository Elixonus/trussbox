

/*
./bin/pipeline solvetruss_executable=./bin/solvetruss rendertruss_executable=./bin/rendertruss forcediagram_executable=./bin/forcediagram feedback_executable=./bin/feedback problem_filename=warren.txt problems_dirname=tmp/test/problems solutions_dirname=tmp/test/solutions prosols_dirname=tmp/test/prosols frames_dirname=tmp/test/frames diagrams_dirname=tmp/test/diagrams gravity=9.8 timef=1.0 srate=10000.0 frate=60.0 fsize=1920x1080 "fcenter=(0.5 0.125)" fzoom=1.0 fscale=1.0

problems/000000001.txt
problems/000000002.txt

solvetruss_executable
rendertruss_executable
forcediagram_executable
feedback_executable
problem_filename
problems_dirname
solutions_dirname
prosols_dirname
frames_dirname
diagrams_dirname
gravity
timef
srate
frate
fsize
fcenter
fzoom
fscale

cp pfilename pdirname/000000001.txt
rendertruss_executable frames_dirname/000000001.png < problems_dirname/000000001.txt
solvetruss_executable < problems_dirname/000000001.txt > solutions_dirname/000000001.txt
cat problems_dirname/000000001.txt solutions_dirname/000000001.txt > prosols_dirname/000000001.txt
forcediagram_executable diagrams_dirname/000000001.png < prosols_dirname/000000001.txt
feedback_executable < prosols_dirname/000000001.txt > problems_dirname/000000002.txt

rendertruss ...

ffmpeg -r frate -i $dirname/frames/%09d.png -y $dirname/video.mp4 -loglevel error

*/

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
double gravity;
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
		printf("error: count: arguments: %d of 18 provided\n", argc - 1);
		printf("usage: arguments: %s solvetruss_executable=string rendertruss_executable=string forcediagram_executable=string feedback_executable=string problem_filename=string problems_dirname=string solutions_dirname=string prosols_dirname=string frames_dirname=string diagrams_dirname=string gravity=float timef=float srate=float frate=float fsize=integerxinteger fcenter=(float float) fzoom=float fscale=float\n", argv[0]);
		return 1;
	}
	if(sscanf(argv[1], "solvetruss_executable=%s", solvetruss_executable) != 1)
	{
		printf("error: parse: solvetruss_executable (1)\n");
		printf("usage: solvetruss_executable: solvetruss_executable=string (1)\n");
		return 1;
	}
	if(sscanf(argv[2], "rendertruss_executable=%s", rendertruss_executable) != 1)
	{
		printf("error: parse: rendertruss_executable (2)\n");
		printf("usage: rendertruss_executable: rendertruss_executable=string (2)\n");
		return 1;
	}
	if(sscanf(argv[3], "forcediagram_executable=%s", forcediagram_executable) != 1)
	{
		printf("error: parse: forcediagram_executable (3)\n");
		printf("usage: forcediagram_executable: forcediagram_executable=string (3)\n");
		return 1;
	}
	if(sscanf(argv[4], "feedback_executable=%s", feedback_executable) != 1)
	{
		printf("error: parse: feedback_executable (4)\n");
		printf("usage: feedback_executable: feedback_executable=string (4)\n");
		return 1;
	}
	if(sscanf(argv[5], "problem_filename=%s", problem_filename) != 1)
	{
		printf("error: parse: problem_filename (5)\n");
		printf("usage: problem_filename: problem_filename=string (5)\n");
		return 1;
	}
	if(sscanf(argv[6], "problems_dirname=%s", problems_dirname) != 1)
	{
		printf("error: parse: problems_dirname (6)\n");
		printf("usage: problems_dirname: problems_dirname=string (6)\n");
		return 1;
	}
	if(sscanf(argv[7], "solutions_dirname=%s", solutions_dirname) != 1)
	{
		printf("error: parse: solutions_dirname (7)\n");
		printf("usage: solutions_dirname: solutions_dirname=string (7)\n");
		return 1;
	}
	if(sscanf(argv[8], "prosols_dirname=%s", prosols_dirname) != 1)
	{
		printf("error: parse: prosols_dirname (8)\n");
		printf("usage: prosols_dirname: prosols_dirname=string (8)\n");
		return 1;
	}
	if(sscanf(argv[9], "frames_dirname=%s", frames_dirname) != 1)
	{
		printf("error: parse: frames_dirname (9)\n");
		printf("usage: frames_dirname: frames_dirname=string (9)\n");
		return 1;
	}
	if(sscanf(argv[10], "diagrams_dirname=%s", diagrams_dirname) != 1)
	{
		printf("error: parse: diagrams_dirname (10)\n");
		printf("usage: diagrams_dirname: diagrams_dirname=string (10)\n");
		return 1;
	}
	if(sscanf(argv[11], "gravity=%lf", &gravity) != 1)
	{
		printf("error: parse: gravity (11)\n");
		printf("usage: gravity: gravity=float (11)\n");
		return 1;
	}
	if(sscanf(argv[12], "timef=%lf", &timef) != 1)
	{
		printf("error: parse: timef (12)\n");
		printf("usage: timef: timef=float (12)\n");
		return 1;
	}
	if(timef < epsilon)
	{
		fprintf(stderr, "error: limit: timef argument: %.1e not greater than %.1e\n", timef, epsilon);
		return 1;
	}
	if(sscanf(argv[13], "srate=%lf", &srate) != 1)
	{
		printf("error: parse: srate (13)\n");
		printf("usage: srate: srate=float (13)\n");
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
		printf("error: parse: frate (14)\n");
		printf("usage: frate: frate=float (14)\n");
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
		printf("error: parse: fsize (15)\n");
		printf("usage: fsize: fsize=integerxinteger (15)\n");
		return 1;
	}
	if(fsize[0] < 64 || fsize[1] < 64)
	{
		fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than 64x64 nor matching\n", fsize[0], fsize[1]);
		return 1;
	}
	if(sscanf(argv[16], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
	{
		printf("error: parse: fcenter (16)\n");
		printf("usage: fcenter: fcenter=(float float) (16)\n");
		return 1;
	}
	if(sscanf(argv[17], "fzoom=%lf", &fzoom) != 1)
	{
		printf("error: parse: fzoom (17)\n");
		printf("usage: fzoom: fzoom=float (17)\n");
		return 1;
	}
	if(fzoom < epsilon)
	{
		fprintf(stderr, "error: limit: fzoom argument: %.1e not greater than %.1e\n", fzoom, epsilon);
		return 1;
	}
	if(sscanf(argv[18], "fscale=%lf", &fscale) != 1)
	{
		printf("error: parse: fscale (18)\n");
		printf("usage: fscale: fscale=float (18)\n");
		return 1;
	}
	if(fscale < epsilon)
	{
		fprintf(stderr, "error: limit: fscale argument: %.1e not greater than %.1e\n", fscale, epsilon);
		return 1;
	}
	printf("cp %s %s/%09d.txt\n", problem_filename, problems_dirname, 1);
	step = 0;
	frame = 0;
	while(frame <= framef)
	{
		printf(
			"%s %s/%09d.png fsize=%dx%d \"fcenter=(%.9e %.9e)\" fzoom=%.9e fscale=%.9e < %s/%09d.txt\n",
			rendertruss_executable, frames_dirname, frame + 1, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, problems_dirname, frame + 1
		);
		printf(
			"%s gravity=%.9e timef=%.9e srate=%.9e < %s/%09d.txt > %s/%09d.txt\n",
			solvetruss_executable, gravity, timef / ((double) (framef + 1)), srate, problems_dirname, frame + 1, solutions_dirname, frame + 1
		);
		printf(
			"cat %s/%09d.txt %s/%09d.txt > %s/%09d.txt\n",
			problems_dirname, frame + 1, solutions_dirname, frame + 1, prosols_dirname, frame + 1
		);
		printf(
			"%s %s/%09d.png gravity=%.9e fsize=%dx%d \"fcenter=(%.9e %.9e)\" fzoom=%.9e fscale=%.9e < %s/%09d.txt\n",
			forcediagram_executable, diagrams_dirname, frame + 1, gravity, fsize[0], fsize[1], fcenter[0], fcenter[1], fzoom, fscale, prosols_dirname, frame + 1
		);
		printf(
			"%s < %s/%09d.txt > %s/%09d.txt\n",
			feedback_executable, prosols_dirname, frame + 1, problems_dirname, frame + 2
		);
		frame++;
		while(step * (framef + 1) < frame * (stepf + 1))
		{
			step++;
		}
	}
}
