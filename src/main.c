#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include "dampspring.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#ifndef M_TAU
#define M_TAU 6.283185307
#endif

struct joint {
	struct mass mass;
};

struct member {
	struct spring spring;
	struct damper damper;
};

struct support {
	struct mass *mass;
	struct constraint {
		double n[3];
		double p0[3];
	} constraints[3];
};

int jcount = 2;
struct joint joints[jcount] = {
	{
		.mass = {
			.m = 1.0,
			.p = {0.0, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
	{
		.mass = {
			.m = 1.0,
			.p = {1.0, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	}
};

int mcount = 1;
struct member members[mcount] = {
	{
		.spring = {
			.k = 1.0,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass,
			.l0 = 0.75
		},
		.damper = {
			.c = 1.0,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass
		}
	}
};

int scount = 0;
struct support supports[scount] = {};

// TODO: Fix this
void step(void)
{
	double jforces[jcount];
	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 3; c++)
		{
			jforces[j][c] = 0.0;
		}
	}

	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		double mforce = sforce(member->spring) + dforce(member->damper);
		double mlength = mdistance(member->spring.m1, member->spring.m2);
		double mvector[3];
		for(int c = 0; c < 3; c++)
		{
			mvector[c] = (member->spring.m2.p[c] - member->spring.m1.p[c]) / mlength;
		}
		for(int c = 0; c < 3; c++)
		{
			jforces[]
		}
	}


	for(int j = 0; j < jcount; j++)
	{
		double jforce = ;
	}
}

int main(void)
{
	struct joint

	printf("sizeof struct mass: %d\n", sizeof(struct mass));

	return EXIT_SUCCESS;
}
