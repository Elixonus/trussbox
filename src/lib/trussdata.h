#pragma once

#define MSDAXES 2
#include "msd.h"

struct joint
{
	struct mass mass;
};

struct member
{
	struct spring spring;
	struct damper damper;
};

struct support
{
	struct constraint
	{
		bool a[2];
		struct mass *m;
		double p[2];
	} constraint;
};

struct load
{
	struct action
	{
		double f[2];
		struct mass *m;
	} action;
};

extern struct joint *joints;
extern double **jforces;
extern int jcount;

extern struct member *members;
extern double *mlengths;
extern double *mdisplacements;
extern double *mvelocities;
extern double *mforces;
extern int mcount;

extern struct support *supports;
extern double **sreactions;
extern int scount;

extern struct load *loads;
extern int lcount;

void print_truss_problem(void);

int scan_truss_problem(void);

int scan_truss_solution(bool update_truss_problem);

void free_truss_problem(void);

void free_truss_solution(void);
