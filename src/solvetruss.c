#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
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

struct joint *joints;
double **jaccelerations;
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

double gravity;
double dtime;
double time;
double timef;
int step;
int stepf;
double srate;

double epsilon = 1.0e-18;

void solve(void)
{
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int c = 0; c < 2; c++)
		{
			joint->mass.v[c] += 0.5 * jaccelerations[j][c] * dtime;
			joint->mass.p[c] += joint->mass.v[c] * dtime;
		}
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int c = 0; c < 2; c++)
			jforces[j][c] = 0.0;
		jforces[j][1] = -gravity * joint->mass.m;
	}
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == load->action.m)
				jindex = j;
		for(int c = 0; c < 2; c++)
			jforces[jindex][c] += load->action.f[c];
	}
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		double length = mdistance(member->spring.m1, member->spring.m2);
		double force = sforce(&member->spring) + dforce(&member->damper);
		double direction[2];
		int jindex1, jindex2;
		for(int j = 0; j < jcount; j++)
		{
			if(&joints[j].mass == member->spring.m1)
				jindex1 = j;
			if(&joints[j].mass == member->spring.m2)
				jindex2 = j;
		}
		for(int c = 0; c < 2; c++)
		{
			direction[c] = ((joints[jindex2].mass.p[c] - joints[jindex1].mass.p[c]) /
			                (length > epsilon ? length : epsilon));
			jforces[jindex1][c] -= direction[c] * force;
			jforces[jindex2][c] += direction[c] * force;
		}
		mforces[m] = force;
		mlengths[m] = length;
		mdisplacements[m] = sdisplacement(&member->spring);
		mvelocities[m] = dvelocity(&member->damper);
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int c = 0; c < 2; c++)
		{
			jaccelerations[j][c] = jforces[j][c] / (joint->mass.m > epsilon ? joint->mass.m : epsilon);
			joint->mass.v[c] += 0.5 * jaccelerations[j][c] * dtime;
		}
	}
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == support->constraint.m)
				jindex = j;
		for(int c = 0; c < 2; c++)
		{
			if(support->constraint.a[c])
			{
				support->constraint.m->p[c] = support->constraint.p[c];
				support->constraint.m->v[c] = 0.0;
				sreactions[s][c] = -jforces[jindex][c];
				jforces[jindex][c] = 0.0;
			}
		}
	}
	time += dtime;
	step++;
}

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		fprintf(stderr, "error: count: arguments: %d of 3 provided\n", argc - 1);
		fprintf(stderr, "usage: arguments: %s gravity=float timef=float srate=float\n", argv[0]);
		return 1;
	}
	if(sscanf(argv[1], "gravity=%lf", &gravity) != 1)
	{
		fprintf(stderr, "error: parse: gravity argument (1): %s\n", argv[1]);
		fprintf(stderr, "usage: gravity argument (1): gravity=float\n");
		return 1;
	}
	if(sscanf(argv[2], "timef=%lf", &timef) != 1)
	{
		fprintf(stderr, "error: parse: timef argument (2): %s\n", argv[2]);
		fprintf(stderr, "usage: timef argument (2): timef=float\n");
		return 1;
	}
	if(timef < epsilon)
	{
		fprintf(stderr, "error: limit: timef argument: %.1e not greater than %.1e\n", timef, epsilon);
		return 1;
	}
	if(sscanf(argv[3], "srate=%lf", &srate) != 1)
	{
		fprintf(stderr, "error: parse: srate argument (3): %s\n", argv[3]);
		fprintf(stderr, "usage: srate argument (3): srate=float\n");
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
	if(scanf("joints=%d\n", &jcount) != 1)
	{
		fprintf(stderr, "error: parse: joints parameter\n");
		fprintf(stderr, "usage: joints parameter: joints=float\n");
		return 1;
	}
	if(jcount < 0)
	{
		fprintf(stderr, "error: count: joints parameter: %d not positive nor zero\n", jcount);
		return 1;
	}
	joints = malloc(jcount * sizeof(struct joint));
	if(!joints)
	{
		fprintf(stderr, "error: create: joints array: %zdB allocation\n", jcount * sizeof(struct joint));
		return 1;
	}
	jaccelerations = malloc(jcount * sizeof(double *));
	if(!jaccelerations)
	{
		fprintf(stderr, "error: create: jaccelerations array: %zdB allocation\n", jcount * sizeof(double *));
		return 1;
	}
	jforces = malloc(jcount * sizeof(double *));
	if(!jforces)
	{
		fprintf(stderr, "error: create: jforces array: %zdB allocation\n", jcount * sizeof(double *));
		return 1;
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint joint;
		if(scanf("mass=%lf position=(%lf %lf) velocity=<%lf %lf>\n",
		         &joint.mass.m, &joint.mass.p[0], &joint.mass.p[1], &joint.mass.v[0], &joint.mass.v[1]) != 5)
		{
			fprintf(stderr, "error: parse: joint line (%d)\n", j + 1);
			fprintf(stderr, "usage: joint line: mass=float position=(float float) velocity=(float float)\n");
			return 1;
		}
		if(joint.mass.m < epsilon)
		{
			fprintf(stderr, "error: limit: joint line (%d): mass parameter: %.1e not greater than %.1e\n", j + 1, joint.mass.m, epsilon);
			return 1;
		}
		joints[j] = joint;
		jaccelerations[j] = malloc(2 * sizeof(double));
		if(!jaccelerations[j])
		{
			fprintf(stderr, "error: create: jacceleration array (%d): %zdB allocation\n", j + 1, 2 * sizeof(double));
			return 1;
		}
		jforces[j] = malloc(2 * sizeof(double));
		if(!jforces[j])
		{
			fprintf(stderr, "error: create: jforce array (%d): %zdB allocation\n", j + 1, 2 * sizeof(double));
			return 1;
		}
		for(int c = 0; c < 2; c++)
			jaccelerations[j][c], jforces[j][c] = 0.0;
	}
	if(scanf("members=%d\n", &mcount) != 1)
	{
		fprintf(stderr, "error: parse: members parameter\n");
		fprintf(stderr, "usage: members parameter: members=float\n");
		return 1;
	}
	if(mcount < 0)
	{
		fprintf(stderr, "error: count: members parameter: %d not positive nor zero\n", mcount);
		return 1;
	}
	members = malloc(mcount * sizeof(struct member));
	if(!members)
	{
		fprintf(stderr, "error: create: members array: %zdB allocation\n", mcount * sizeof(struct member));
		return 1;
	}
	mlengths = malloc(mcount * sizeof(double));
	if(!mlengths)
	{
		fprintf(stderr, "error: create: mlengths array: %zdB allocation\n", mcount * sizeof(double));
		return 1;
	}
	mdisplacements = malloc(mcount * sizeof(double));
	if(!mdisplacements)
	{
		fprintf(stderr, "error: create: mdisplacements array: %zdB allocation\n", mcount * sizeof(double));
		return 1;
	}
	mvelocities = malloc(mcount * sizeof(double));
	if(!mvelocities)
	{
		fprintf(stderr, "error: create: mvelocities array: %zdB allocation\n", mcount * sizeof(double));
		return 1;
	}
	mforces = malloc(mcount * sizeof(double));
	if(!mforces)
	{
		fprintf(stderr, "error: create: mforces array: %zdB allocation\n", mcount * sizeof(double));
		return 1;
	}
	for(int m = 0; m < mcount; m++)
	{
		int jindex1, jindex2;
		struct member member;
		if(scanf("joint1=%d joint2=%d stiffness=%lf length0=%lf dampening=%lf\n",
		         &jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c) != 5)
		{
			fprintf(stderr, "error: parse: member line (%d)\n", m + 1);
			fprintf(stderr, "usage: member line: joint1=index joint2=index stiffness=float length0=float dampening=float\n");
			return 1;
		}
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount)
		{
			fprintf(stderr, "error: index: member line (%d): joint1 parameter: %d does not exist\n", m + 1, jindex1 + 1);
			return 1;
		}
		if(jindex2 < 0 || jindex2 >= jcount)
		{
			fprintf(stderr, "error: index: member line (%d): joint2 parameter: %d does not exist\n", m + 1, jindex2 + 1);
			return 1;
		}
		for(int m2 = 0; m2 < m; m2++)
			if((members[m2].spring.m1 == &joints[jindex1].mass &&
			    members[m2].spring.m2 == &joints[jindex2].mass) ||
			   (members[m2].spring.m1 == &joints[jindex2].mass &&
			    members[m2].spring.m2 == &joints[jindex1].mass))
			{
				fprintf(stderr, "error: index: member line (%d): joint1 and joint2 parameters: (%d and %d) or (%d and %d) already in use\n", m + 1, jindex1 + 1, jindex2 + 1, jindex2 + 1, jindex1 + 1);
				return 1;
			}
		member.spring.m1 = &joints[jindex1].mass;
		member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass;
		member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < epsilon)
		{
			fprintf(stderr, "error: limit: member line (%d): length0 parameter: %.1e not greater than %.1e\n", m + 1, member.spring.l0, epsilon);
			return 1;
		}
		members[m] = member;
		mlengths[m], mdisplacements[m], mvelocities[m], mforces[m] = 0.0;
	}
	if(scanf("supports=%d\n", &scount) != 1)
	{
		fprintf(stderr, "error: parse: supports parameter\n");
		fprintf(stderr, "usage: supports parameter: supports=float\n");
		return 1;
	}
	if(scount < 0)
	{
		fprintf(stderr, "error: count: supports parameter: %d not positive nor zero\n", scount);
		return 1;
	}
	supports = malloc(scount * sizeof(struct support));
	if(!supports)
	{
		fprintf(stderr, "error: create: supports array: %zdB allocation\n", scount * sizeof(struct support));
		return 1;
	}
	sreactions = malloc(scount * sizeof(double *));
	if(!sreactions)
	{
		fprintf(stderr, "error: create: sreactions array: %zdB allocation\n", scount * sizeof(double *));
		return 1;
	}
	for(int s = 0; s < scount; s++)
	{
		int jindex;
		char axes[101];
		struct support support;
		if(scanf("joint=%d axes=%100s\n", &jindex, axes) != 2)
		{
			fprintf(stderr, "error: parse: support line (%d)\n", s + 1);
			fprintf(stderr, "usage: support line: joint=index axes=xy|x|y\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: support line (%d): joint parameter: %d does not exist\n", s + 1, jindex + 1);
			return 1;
		}
		for(int s2 = 0; s2 < s; s2++) if(supports[s2].constraint.m == &joints[jindex].mass)
			{
				fprintf(stderr, "error: index: support line (%d): joint parameter: %d already in use\n", s + 1, jindex + 1);
				return 1;
			}
		support.constraint.m = &joints[jindex].mass;
		if(strcmp(axes, "xy") == 0 || strcmp(axes, "yx") == 0)
		{
			support.constraint.a[0] = true;
			support.constraint.a[1] = true;
		}
		else if(strcmp(axes, "x") == 0)
		{
			support.constraint.a[0] = true;
			support.constraint.a[1] = false;
		}
		else if(strcmp(axes, "y") == 0)
		{
			support.constraint.a[0] = false;
			support.constraint.a[1] = true;
		}
		else
		{
			fprintf(stderr, "error: parse: support line (%d): axes parameter: %s not an option\n", s + 1, axes);
			fprintf(stderr, "usage: support line: axes parameter: axes=xy|x|y\n");
			return 1;
		}
		for(int c = 0; c < 2; c++)
			support.constraint.p[c] = joints[jindex].mass.p[c];
		supports[s] = support;
		sreactions[s] = malloc(2 * sizeof(double));
		if(!sreactions[s])
		{
			fprintf(stderr, "error: create: sreaction array (%d): %zdB allocation\n", s + 1, 2 * sizeof(double));
			return 1;
		}
		for(int c = 0; c < 2; c++) sreactions[s][c] = 0.0;
	}
	if(scanf("loads=%d\n", &lcount) != 1)
	{
		fprintf(stderr, "error: parse: loads parameter\n");
		fprintf(stderr, "usage: loads parameter: loads=float\n");
		return 1;
	}
	if(lcount < 0)
	{
		fprintf(stderr, "error: count: loads parameter: %d not positive nor zero\n", lcount);
		return 1;
	}
	loads = malloc(lcount * sizeof(struct load));
	if(!loads)
	{
		fprintf(stderr, "error: create: loads array: %zdB allocation\n", lcount * sizeof(struct load));
		return 1;
	}
	for(int l = 0; l < lcount; l++)
	{
		int jindex;
		struct load load;
		if(scanf("joint=%d force=<%lf %lf>\n",
		         &jindex, &load.action.f[0], &load.action.f[1]) != 3)
		{
			fprintf(stderr, "error: parse: load line (%d)\n", l + 1);
			fprintf(stderr, "usage: load line: joint=index force=<float float>\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: load line (%d): joint parameter: %d does not exist\n", l + 1, jindex + 1);
			return 1;
		}
		load.action.m = &joints[jindex].mass;
		loads[l] = load;
	}
	time = 0.0, step = 0;
	while(step <= stepf)
		solve();
	printf("joints=%d\n", jcount);
	for(int j = 0; j < jcount; j++)
	{
		printf("force=<%.9e %.9e> position=(%.9e %.9e) velocity=<%.9e %.9e>\n",
		       jforces[j][0], jforces[j][1], joints[j].mass.p[0], joints[j].mass.p[1], joints[j].mass.v[0], joints[j].mass.v[1]);
	}
	printf("members=%d\n", mcount);
	for(int m = 0; m < mcount; m++)
	{
		printf("force=%.9e displacement=%.9e length=%.9e velocity=%.9e\n",
		       mforces[m], mdisplacements[m], mlengths[m], mvelocities[m]);
	}
	printf("supports=%d\n", scount);
	for(int s = 0; s < scount; s++)
	{
		printf("reaction=<%.9e %.9e>\n", sreactions[s][0], sreactions[s][1]);
	}
	free(joints);
	for(int j = 0; j < jcount; j++)
	{
		free(jaccelerations[j]);
		free(jforces[j]);
	}
	free(jaccelerations);
	free(jforces);
	free(members);
	free(mlengths);
	free(mdisplacements);
	free(mvelocities);
	free(mforces);
	free(supports);
	for(int s = 0; s < scount; s++)
		free(sreactions[s]);
	free(sreactions);
	free(loads);
	return 0;
}
