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

#pragma once

extern char *solvetruss_executable;
extern char *rendertruss_executable;
extern char *forcediagram_executable;
extern char *sweptarea_executable;
extern char *trussutils_executable;
extern char *problem_filename;
extern char *output_dirname;
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

void allocate_string(char **string, int count);

void free_string(char *string);

void allocate_executable_and_input_output_paths(int character_count_each);

void free_executable_and_input_output_paths(void);

void reset_current_frame_and_step_number(void);

bool continue_current_frame(void);

void progress_current_frame_and_step_number(void);

void print_pipeline_header_commands(FILE *command_stream);

void print_pipeline_manage_files_and_directories_commands(FILE *command_stream);

void start_pipeline(FILE *command_stream);

void print_pipeline_concatenate_problem_and_solution_command(FILE *command_stream);

void print_pipeline_feedback_solution_into_problem_command(FILE *command_stream);

void print_pipeline_solvetruss_command(FILE *command_stream);

void print_pipeline_rendertruss_command(FILE *command_stream);

void print_pipeline_forcediagram_command(FILE *command_stream);

void print_pipeline_sweptarea_command(FILE *command_stream);
