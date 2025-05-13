#ifndef PIPELINEHEADER
#define PIPELINEHEADER

extern char solvetruss_executable[1001];
extern char rendertruss_executable[1001];
extern char forcediagram_executable[1001];
extern char trussutils_executable[1001];
extern char problem_filename[1001];
extern char output_dirname[1001];
extern double gacceleration;
extern double dtime;
extern double timef;
extern int step;
extern int stepf;
extern double srate;
extern int frame;
extern int framef;
extern double frate;
extern int fsize[2];
extern double fcenter[2];
extern double fzoom;
extern double fscale;

void allocate_string(char *string, int count);
void free_string(char *string);
void allocate_paths(int character_count_each);
void free_paths(void);
void initialize_frame_and_step(void);
void progress_frame_and_step(void);
void initialize_bash(FILE *command_stream);
void initialize_files_and_directories(FILE *command_stream);
void initialize(FILE *command_stream);
void concatenate_problem_and_solution(FILE *command_stream);
void feedback_solution_into_problem(FILE *command_stream);
void solvetruss(FILE *command_stream);
void rendertruss(FILE *command_stream);
void forcediagram(FILE *command_stream);

#endif
