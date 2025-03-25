/*
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
solvetruss_executable problems_dirname/000000001.txt > solutions_dirname/000000001.txt
cat problems_dirname/000000001.txt solutions_dirname/000000001.txt > prosols_dirname/000000001.txt
forcediagram_executable diagrams_dirname/000000001.png < prosols_dirname/000000001.txt
feedback_executable < prosols_dirname/000000001.txt > problems_dirname/000000002.txt

rendertruss ...

ffmpeg -r frate -i $dirname/frames/%09d.png -y $dirname/video.mp4 -loglevel error

*/

#include <stdio.h>

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

int main(int argc, char **argv)
{
    // if(argc != 100) return 1;
    if(sscanf(argv[1], "%s", solvetruss_executable) != 1)
    {
        printf("error: parse: solvetruss_executable\n");
        printf("usage: solvetruss_executable: solvetruss_executable=string\n");
        return 1;
    }
    if(sscanf(argv[2], "%s", rendertruss_executable) != 1)
    {
        printf("error: parse: rendertruss_executable\n");
        printf("usage: rendertruss_executable: rendertruss_executable=string\n");
        return 1;
    }
    if(sscanf(argv[3], "%s", forcediagram_executable) != 1)
    {
        printf("error: parse: forcediagram_executable\n");
        printf("usage: forcediagram_executable: forcediagram_executable=string\n");
        return 1;
    }
    if(sscanf(argv[4], "%s", feedback_executable) != 1)
    {
        printf("error: parse: feedback_executable\n");
        printf("usage: feedback_executable: feedback_executable=string\n");
        return 1;
    }
    if(sscanf(argv[5], "%s", problem_filename) != 1)
    {
        printf("error: parse: problem_filename\n");
        printf("usage: problem_filename: problem_filename=string\n");
        return 1;
    }
    if(sscanf(argv[6], "%s", problems_dirname) != 1)
    {
        printf("error: parse: problems_dirname\n");
        printf("usage: problems_dirname: problems_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[7], "%s", solutions_dirname) != 1)
    {
        printf("error: parse: solutions_dirname\n");
        printf("usage: solutions_dirname: solutions_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[8], "%s", prosols_dirname) != 1)
    {
        printf("error: parse: prosols_dirname\n");
        printf("usage: prosols_dirname: prosols_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[9], "%s", frames_dirname) != 1)
    {
        printf("error: parse: frames_dirname\n");
        printf("usage: frames_dirname: frames_dirname=string\n");
        return 1;
    }
    if(sscanf(argv[10], "%s", diagrams_dirname) != 1)
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
    if(sscanf(argv[13], "srate=%lf", &srate) != 1)
    {
        printf("error: parse: srate\n");
        printf("usage: srate: srate=float\n");
        return 1;
    }
    if(sscanf(argv[14], "frate=%lf", &frate) != 1)
    {
        printf("error: parse: frate\n");
        printf("usage: frate: frate=float\n");
        return 1;
    }
    if(sscanf(argv[15], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
    {
        printf("error: parse: fsize\n");
        printf("usage: fsize: fsize=integerxinteger\n");
        return 1;
    }
    if(sscanf(argv[16], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
    {
        printf("error: parse: fcenter\n");
        printf("usage: fcenter: fcenter=(float float)\n");
        return 1;
    }
    if(sscanf(argv[14], "fzoom=%lf", &fzoom) != 1)
    {
        printf("error: parse: fzoom\n");
        printf("usage: fzoom: fzoom=float\n");
        return 1;
    }
    if(sscanf(argv[14], "fscale=%lf", &fscale) != 1)
    {
        printf("error: parse: fscale\n");
        printf("usage: fscale: fscale=float\n");
        return 1;
    }

    int step = 0;
    int frame = 0;
    
}
