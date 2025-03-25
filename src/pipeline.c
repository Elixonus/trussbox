

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
double timef;
double srate;
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
        printf("error: parse: solvetruss_executable\n");
        printf("usage: solvetruss_executable: solvetruss_executable=string\n");
        return 1;
    }
    if(sscanf(argv[2], "rendertruss_executable=%s", rendertruss_executable) != 1)
    {
        printf("error: parse: rendertruss_executable\n");
        printf("usage: rendertruss_executable: rendertruss_executable=string\n");
        return 1;
    }
    if(sscanf(argv[3], "forcediagram_executable=%s", forcediagram_executable) != 1)
    {
        printf("error: parse: forcediagram_executable\n");
        printf("usage: forcediagram_executable: forcediagram_executable=string\n");
        return 1;
    }
    if(sscanf(argv[4], "feedback_executable=%s", feedback_executable) != 1)
    {
        printf("error: parse: feedback_executable\n");
        printf("usage: feedback_executable: feedback_executable=string\n");
        return 1;
    }
    if(sscanf(argv[5], "problem_filename=%s", problem_filename) != 1)
    {
        printf("error: parse: problem_filename\n");
        printf("usage: problem_filename: problem_filename=string\n");
        return 1;
    }
    if(sscanf(argv[6], "problems_dirname=%s", problems_dirname) != 1)
    {
        printf("error: parse: problems_dirname\n");
        printf("usage: problems_dirname: problems_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[7], "solutions_dirname=%s", solutions_dirname) != 1)
    {
        printf("error: parse: solutions_dirname\n");
        printf("usage: solutions_dirname: solutions_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[8], "prosols_dirname=%s", prosols_dirname) != 1)
    {
        printf("error: parse: prosols_dirname\n");
        printf("usage: prosols_dirname: prosols_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[9], "frames_dirname=%s", frames_dirname) != 1)
    {
        printf("error: parse: frames_dirname\n");
        printf("usage: frames_dirname: frames_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[10], "diagrams_dirname=%s", diagrams_dirname) != 1)
    {
        printf("error: parse: diagrams_dirname\n");
        printf("usage: diagrams_dirname: diagrams_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[11], "gravity=%lf", &gravity) != 1)
    {
        printf("error: parse: gravity\n");
        printf("usage: gravity: gravity=float\n");
        return 1;
    }
    if(sscanf(argv[12], "timef=%lf", &timef) != 1)
    {
        printf("error: parse: timef\n");
        printf("usage: timef: timef=float\n");
        return 1;
    }
    if(timef < epsilon) return 1;
    if(sscanf(argv[13], "srate=%lf", &srate) != 1)
    {
        printf("error: parse: srate\n");
        printf("usage: srate: srate=float\n");
        return 1;
    }
    if(srate < epsilon) return 1;
    double dtime = 1.0 / srate;
    if(dtime < epsilon) return 1;
    int stepf = ((int) round(srate * timef)) - 1;
    if(stepf < 0) return 1;
    if(sscanf(argv[14], "frate=%lf", &frate) != 1)
    {
        printf("error: parse: frate\n");
        printf("usage: frate: frate=float\n");
        return 1;
    }
    if(frate < epsilon) return 1;
    int framef = ((int) round(frate * timef)) - 1;
    if(framef < 0) return 1;
    if(sscanf(argv[15], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
    {
        printf("error: parse: fsize\n");
        printf("usage: fsize: fsize=integerxinteger\n");
        return 1;
    }
    if(fsize[0] < 64 || fsize[1] < 64) return 1;
    if(sscanf(argv[16], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
    {
        printf("error: parse: fcenter\n");
        printf("usage: fcenter: fcenter=(float float)\n");
        return 1;
    }
    if(sscanf(argv[17], "fzoom=%lf", &fzoom) != 1)
    {
        printf("error: parse: fzoom\n");
        printf("usage: fzoom: fzoom=float\n");
        return 1;
    }
    if(fzoom < epsilon) return 1;
    if(sscanf(argv[18], "fscale=%lf", &fscale) != 1)
    {
        printf("error: parse: fscale\n");
        printf("usage: fscale: fscale=float\n");
        return 1;
    }
    if(fscale < epsilon) return 1;
    printf("cp %s %s/%09d.txt\n", problem_filename, problems_dirname, 1);
    int step = 0;
    int frame = 0;
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
