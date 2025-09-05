#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSDAXES 2
#include "msd.h"

#define EPSILON 1.0e-18

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

struct joint *joints;
double **jforces;
int jcount;

struct member *members;
double *mlengths;
double *mdisplacements;
double *mvelocities;
double *mforces;
int mcount;

struct support *supports;
double **sreactions;
int scount;

struct load *loads;
int lcount;

void print_truss_problem(void)
{
	printf("joints=%d\n", jcount);
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		printf("mass=%.9le position=(%.9le %.9le) velocity=<%.9le %.9le>\n",
			   joint->mass.m, joint->mass.p[0], joint->mass.p[1], joint->mass.v[0], joint->mass.v[1]);
	}
	printf("members=%d\n", mcount);
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		int jindex1, jindex2;
		for(int j = 0; j < jcount; j++)
		{
			if(&joints[j].mass == member->spring.m1)
				jindex1 = j;
			if(&joints[j].mass == member->spring.m2)
				jindex2 = j;
		}
		printf("joint1=[%d] joint2=[%d] stiffness=%.9le length0=%.9le dampening=%.9le\n",
			   jindex1 + 1, jindex2 + 1, member->spring.k, member->spring.l0, member->damper.c);
	}
	printf("supports=%d\n", scount);
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == support->constraint.m)
			jindex = j;
		char axes[3];
		if(support->constraint.a[0] && support->constraint.a[1])
			sprintf(axes, "xy");
		if(support->constraint.a[0] && !support->constraint.a[1])
			sprintf(axes, "x");
		if(!support->constraint.a[0] && support->constraint.a[1])
			sprintf(axes, "y");
		printf("joint=[%d] axes={%s}\n", jindex + 1, axes);
	}
	printf("loads=%d\n", lcount);
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == load->action.m)
			jindex = j;
		printf("joint=[%d] force=<%.9le %.9le>\n", jindex + 1, load->action.f[0], load->action.f[1]);
	}
}

int scan_truss_problem(void)
{
	if(scanf("joints=%d\n", &jcount) != 1)
	{
		fprintf(stderr, "error: parse: joints parameter\n");
		fprintf(stderr, "usage: joints parameter: joints=count\n");
		return 1;
	}
	if(jcount < 0)
	{
		fprintf(stderr, "error: count: joints parameter: %d not positive nor %d\n", jcount, 0);
		return 1;
	}
	joints = malloc(jcount * sizeof(struct joint));
	if(!joints)
	{
		fprintf(stderr, "error: create: joints array: %zd bytes allocation\n", jcount * sizeof(struct joint));
		return 1;
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint joint;
		if(scanf("mass=%le position=(%le %le) velocity=<%le %le>\n",
		         &joint.mass.m, &joint.mass.p[0], &joint.mass.p[1], &joint.mass.v[0], &joint.mass.v[1]) != 5)
		{
			fprintf(stderr, "error: parse: joint [%d] line\n", j + 1);
			fprintf(stderr, "usage: joint line: mass=float position=(float float) velocity=<float float>\n");
			return 1;
		}
		if(joint.mass.m < EPSILON)
		{
			fprintf(stderr, "error: limit: joint [%d] line: mass parameter: %.1le not greater than %.1le\n", j + 1, joint.mass.m, EPSILON);
			return 1;
		}
		joints[j] = joint;
	}
	if(scanf("members=%d\n", &mcount) != 1)
	{
		fprintf(stderr, "error: parse: members parameter\n");
		fprintf(stderr, "usage: members parameter: members=count\n");
		return 1;
	}
	if(mcount < 0)
	{
		fprintf(stderr, "error: count: members parameter: %d not positive nor %d\n", mcount, 0);
		return 1;
	}
	members = malloc(mcount * sizeof(struct member));
	if(!members)
	{
		fprintf(stderr, "error: create: members array: %zd bytes allocation\n", mcount * sizeof(struct member));
		return 1;
	}
	for(int m = 0; m < mcount; m++)
	{
		int jindex1, jindex2;
		struct member member;
		if(scanf("joint1=[%d] joint2=[%d] stiffness=%le length0=%le dampening=%le\n",
		         &jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c) != 5)
		{
			fprintf(stderr, "error: parse: member [%d] line\n", m + 1);
			fprintf(stderr, "usage: member line: joint1=[index] joint2=[index] stiffness=float length0=float dampening=float\n");
			return 1;
		}
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount)
		{
			fprintf(stderr, "error: index: member [%d] line: joint1 parameter: [%d] does not exist\n", m + 1, jindex1 + 1);
			return 1;
		}
		if(jindex2 < 0 || jindex2 >= jcount)
		{
			fprintf(stderr, "error: index: member [%d] line: joint2 parameter: [%d] does not exist\n", m + 1, jindex2 + 1);
			return 1;
		}
		if(jindex1 == jindex2)
		{
			fprintf(stderr, "error: index: member [%d] line: joint1 and joint2 parameters: [%d] and [%d] cannot match\n", m + 1, jindex1 + 1, jindex2 + 1);
			return 1;
		}
		for(int m2 = 0; m2 < m; m2++)
		{
			if(members[m2].spring.m1 == &joints[jindex1].mass &&
			   members[m2].spring.m2 == &joints[jindex2].mass)
			{
				fprintf(stderr, "error: index: member [%d] line: joint1 and joint2 parameters: [%d] and [%d] already in use\n", m + 1, jindex1 + 1, jindex2 + 1);
				return 1;
			}
			if(members[m2].spring.m1 == &joints[jindex2].mass &&
			   members[m2].spring.m2 == &joints[jindex1].mass)
			{
				fprintf(stderr, "error: index: member line [%d]: joint1 and joint2 parameters: [%d] and [%d] already in use\n", m + 1, jindex2 + 1, jindex1 + 1);
				return 1;
			}
		}
		member.spring.m1 = &joints[jindex1].mass, member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass, member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < EPSILON)
		{
			fprintf(stderr, "error: limit: member line [%d]: length0 parameter: %.1le not greater than %.1le\n", m + 1, member.spring.l0, EPSILON);
			return 1;
		}
		members[m] = member;
	}
	if(scanf("supports=%d\n", &scount) != 1)
	{
		fprintf(stderr, "error: parse: supports parameter\n");
		fprintf(stderr, "usage: supports parameter: supports=count\n");
		return 1;
	}
	if(scount < 0)
	{
		fprintf(stderr, "error: count: supports parameter: %d not positive nor %d\n", scount, 0);
		return 1;
	}
	supports = malloc(scount * sizeof(struct support));
	if(!supports)
	{
		fprintf(stderr, "error: create: supports array: %zd bytes allocation\n", scount * sizeof(struct support));
		return 1;
	}
	for(int s = 0; s < scount; s++)
	{
		int jindex;
		char axes[3];
		struct support support;
		if(scanf("joint=[%d] axes={%2[^}]}\n", &jindex, axes) != 2)
		{
			fprintf(stderr, "error: parse: support [%d] line\n", s + 1);
			fprintf(stderr, "usage: support line: joint=[index] axes={xy|x|y}\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: support [%d] line: joint parameter: [%d] does not exist\n", s + 1, jindex + 1);
			return 1;
		}
		for(int s2 = 0; s2 < s; s2++) if(supports[s2].constraint.m == &joints[jindex].mass)
			{
				fprintf(stderr, "error: index: support [%d] line: joint parameter: [%d] already in use\n", s + 1, jindex + 1);
				return 1;
			}
		support.constraint.m = &joints[jindex].mass;
		if(strcmp(axes, "xy") == 0 || strcmp(axes, "yx") == 0)
			support.constraint.a[0] = true, support.constraint.a[1] = true;
		else if(strcmp(axes, "x") == 0)
			support.constraint.a[0] = true, support.constraint.a[1] = false;
		else if(strcmp(axes, "y") == 0)
			support.constraint.a[0] = false, support.constraint.a[1] = true;
		else
		{
			fprintf(stderr, "error: parse: support [%d] line: axes parameter: not an option\n", s + 1);
			fprintf(stderr, "usage: support line: axes parameter: axes={xy|x|y}\n");
			return 1;
		}
		for(int a = 0; a < 2; a++)
			support.constraint.p[a] = joints[jindex].mass.p[a];
		supports[s] = support;
	}
	if(scanf("loads=%d\n", &lcount) != 1)
	{
		fprintf(stderr, "error: parse: loads parameter\n");
		fprintf(stderr, "usage: loads parameter: loads=count\n");
		return 1;
	}
	if(lcount < 0)
	{
		fprintf(stderr, "error: count: loads parameter: %d not positive nor %d\n", lcount, 0);
		return 1;
	}
	loads = malloc(lcount * sizeof(struct load));
	if(!loads)
	{
		fprintf(stderr, "error: create: loads array: %zd bytes allocation\n", lcount * sizeof(struct load));
		return 1;
	}
	for(int l = 0; l < lcount; l++)
	{
		int jindex;
		struct load load;
		if(scanf("joint=[%d] force=<%le %le>\n",
		         &jindex, &load.action.f[0], &load.action.f[1]) != 3)
		{
			fprintf(stderr, "error: parse: load [%d] line\n", l + 1);
			fprintf(stderr, "usage: load line: joint=[index] force=<float float>\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: load [%d] line: joint parameter: [%d] does not exist\n", l + 1, jindex + 1);
			return 1;
		}
		load.action.m = &joints[jindex].mass;
		loads[l] = load;
	}
	return 0;
}

int scan_truss_solution(bool update_truss_problem)
{
	int jcount2;
	if(scanf("joints=%d\n", &jcount2) != 1)
	{
		fprintf(stderr, "error: parse: joints parameter (solution)\n");
		fprintf(stderr, "usage: joints parameter (solution): joints=count\n");
		return 1;
	}
	if(jcount2 != jcount)
	{
		fprintf(stderr, "error: mismatch: joints parameters: %d and %d\n", jcount, jcount2);
		return 1;
	}
	jforces = malloc(jcount * sizeof(double *));
	if(!jforces)
	{
		fprintf(stderr, "error: create: jforces array: %zd bytes allocation\n", jcount * sizeof(double *));
		return 1;
	}
	for(int j = 0; j < jcount; j++)
	{
		jforces[j] = malloc(2 * sizeof(double));
		if(!jforces[j])
		{
			fprintf(stderr, "error: create: jforce [%d] array: %zd bytes allocation\n", j + 1, 2 * sizeof(double));
			return 1;
		}
		if(update_truss_problem ?
		   (scanf("force=<%le %le> position=(%le %le) velocity=<%le %le>\n",
		          &jforces[j][0], &jforces[j][1], &joints[j].mass.p[0], &joints[j].mass.p[1], &joints[j].mass.v[0], &joints[j].mass.v[1]) != 6) :
		   (scanf("force=<%le %le> position=(%*f %*f) velocity=<%*f %*f>\n",
		          &jforces[j][0], &jforces[j][1]) != 2))
		{
			fprintf(stderr, "error: parse: joint [%d] line (solution)\n", j + 1);
			fprintf(stderr, "usage: joint line (solution): mass=float position=(float float) velocity=(float float)\n");
			return 1;
		}
	}
	int mcount2;
	if(scanf("members=%d\n", &mcount2) != 1)
	{
		fprintf(stderr, "error: parse: members parameter (solution)\n");
		fprintf(stderr, "usage: members parameter (solution): members=count\n");
		return 1;
	}
	if(mcount2 != mcount)
	{
		fprintf(stderr, "error: mismatch: members parameters: %d and %d\n", mcount, mcount2);
		return 1;
	}
	mlengths = malloc(mcount * sizeof(double));
	if(!mlengths)
	{
		fprintf(stderr, "error: create: mlengths array: %zd bytes allocation\n", mcount * sizeof(double));
		return 1;
	}
	mdisplacements = malloc(mcount * sizeof(double));
	if(!mdisplacements)
	{
		fprintf(stderr, "error: create: mdisplacements array: %zd bytes allocation\n", mcount * sizeof(double));
		return 1;
	}
	mvelocities = malloc(mcount * sizeof(double));
	if(!mvelocities)
	{
		fprintf(stderr, "error: create: mvelocities array: %zd bytes allocation\n", mcount * sizeof(double));
		return 1;
	}
	mforces = malloc(mcount * sizeof(double));
	if(!mforces)
	{
		fprintf(stderr, "error: create: mforces array: %zd bytes allocation\n", mcount * sizeof(double));
		return 1;
	}
	for(int m = 0; m < mcount; m++)
	{
		if(scanf("force=%le displacement=%le length=%le velocity=%le\n",
				 &mforces[m], &mdisplacements[m], &mlengths[m], &mvelocities[m]) != 4)
		{
			fprintf(stderr, "error: parse: member [%d] line (solution)\n", m + 1);
			fprintf(stderr, "usage: member line (solution): force=float displacement=float length=float velocity=float\n");
			return 1;
		}
	}
	int scount2;
	if(scanf("supports=%d\n", &scount2) != 1)
	{
		fprintf(stderr, "error: parse: supports parameter (solution)\n");
		fprintf(stderr, "usage: supports parameter (solution): supports=count\n");
		return 1;
	}
	if(scount2 != scount)
	{
		fprintf(stderr, "error: mismatch: supports parameters: %d and %d\n", scount, scount2);
		return 1;
	}
	sreactions = malloc(scount * sizeof(double *));
	if(!sreactions)
	{
		fprintf(stderr, "error: create: sreactions array: %zd bytes allocation\n", scount * sizeof(double *));
		return 1;
	}
	for(int s = 0; s < scount; s++)
	{
		sreactions[s] = malloc(2 * sizeof(double));
		if(!sreactions[s])
		{
			fprintf(stderr, "error: create: sreaction [%d] array: %zd bytes allocation\n", s + 1, 2 * sizeof(double));
			return 1;
		}
		if(scanf("reaction=<%le %le>\n", &sreactions[s][0], &sreactions[s][1]) != 2)
		{
			fprintf(stderr, "error: parse: support [%d] line (solution)\n", s + 1);
			fprintf(stderr, "usage: support line (solution): reaction=<float float>\n");
			return 1;
		}
	}
	return 0;
}

void free_truss_problem(void)
{
	free(joints);
	free(members);
	free(supports);
	free(loads);
}

void free_truss_solution(void)
{
	for(int j = 0; j < jcount; j++)
		free(jforces[j]);
	free(jforces);
	free(mlengths);
	free(mdisplacements);
	free(mvelocities);
	free(mforces);
	for(int s = 0; s < scount; s++)
		free(sreactions[s]);
	free(sreactions);
}
