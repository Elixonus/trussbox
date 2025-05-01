#include <stdio.h>
#include <stdlib.h>
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
int jcount;

struct member *members;
int mcount;

struct support *supports;
int scount;

struct load *loads;
int lcount;

double epsilon = 1.0e-18;

int main(void)
{
	if(scanf("joints=%d\n", &jcount) != 1)
	{
		fprintf(stderr, "error: parse: joints parameter (problem)\n");
		fprintf(stderr, "usage: joints parameter (problem): joints=count\n");
		return 1;
	}
	if(jcount < 0)
	{
		fprintf(stderr, "error: count: joints parameter (problem): %d not positive nor zero\n", jcount);
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
			fprintf(stderr, "error: parse: joint line [%d] (problem)\n", j + 1);
			fprintf(stderr, "usage: joint line (problem): mass=float position=(float float) velocity=(float float)\n");
			return 1;
		}
		if(joint.mass.m < epsilon)
		{
			fprintf(stderr, "error: limit: joint line [%d] (problem): mass parameter: %.1e not greater than %.1e\n", j + 1, joint.mass.m, epsilon);
			return 1;
		}
		joints[j] = joint;
	}
	if(scanf("members=%d\n", &mcount) != 1)
	{
		fprintf(stderr, "error: parse: members parameter (problem)\n");
		fprintf(stderr, "usage: members parameter (problem): members=count\n");
		return 1;
	}
	if(mcount < 0)
	{
		fprintf(stderr, "error: count: members parameter (problem): %d not positive nor zero\n", mcount);
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
			fprintf(stderr, "error: parse: member line [%d] (problem)\n", m + 1);
			fprintf(stderr, "usage: member line (problem): joint1=[index] joint2=[index] stiffness=float length0=float dampening=float\n");
			return 1;
		}
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount)
		{
			fprintf(stderr, "error: index: member line [%d] (problem): joint1 parameter: [%d] does not exist\n", m + 1, jindex1 + 1);
			return 1;
		}
		if(jindex2 < 0 || jindex2 >= jcount)
		{
			fprintf(stderr, "error: index: member line [%d] (problem): joint2 parameter: [%d] does not exist\n", m + 1, jindex2 + 1);
			return 1;
		}
		for(int m2 = 0; m2 < m; m2++)
			if((members[m2].spring.m1 == &joints[jindex1].mass &&
			    members[m2].spring.m2 == &joints[jindex2].mass) ||
			   (members[m2].spring.m1 == &joints[jindex2].mass &&
			    members[m2].spring.m2 == &joints[jindex1].mass))
			{
				fprintf(stderr, "error: index: member line [%d] (problem): joint1 and joint2 parameters: ([%d] and [%d]) or ([%d] and [%d]) already in use\n", m + 1, jindex1 + 1, jindex2 + 1, jindex2 + 1, jindex1 + 1);
				return 1;
			}
		member.spring.m1 = &joints[jindex1].mass, member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass, member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < epsilon)
		{
			fprintf(stderr, "error: limit: member line [%d] (problem): length0 parameter: %.1e not greater than %.1e\n", m + 1, member.spring.l0, epsilon);
			return 1;
		}
		members[m] = member;
	}
	if(scanf("supports=%d\n", &scount) != 1)
	{
		fprintf(stderr, "error: parse: supports parameter (problem)\n");
		fprintf(stderr, "usage: supports parameter (problem): supports=count\n");
		return 1;
	}
	if(scount < 0)
	{
		fprintf(stderr, "error: count: supports parameter (problem): %d not positive nor zero\n", scount);
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
		char axes[101];
		struct support support;
		if(scanf("joint=[%d] axes=%100s\n", &jindex, axes) != 2)
		{
			fprintf(stderr, "error: parse: support line [%d] (problem)\n", s + 1);
			fprintf(stderr, "usage: support line (problem): joint=[index] axes=xy|x|y\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: support line [%d] (problem): joint parameter: [%d] does not exist\n", s + 1, jindex + 1);
			return 1;
		}
		for(int s2 = 0; s2 < s; s2++) if(supports[s2].constraint.m == &joints[jindex].mass)
			{
				fprintf(stderr, "error: index: support line [%d] (problem): joint parameter: [%d] already in use\n", s + 1, jindex + 1);
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
			fprintf(stderr, "error: parse: support line [%d] (problem): axes parameter: not an option\n", s + 1);
			fprintf(stderr, "usage: support line (problem): axes parameter: axes=xy|x|y\n");
			return 1;
		}
		for(int a = 0; a < 2; a++)
			support.constraint.p[a] = joints[jindex].mass.p[a];
		supports[s] = support;
	}
	if(scanf("loads=%d\n", &lcount) != 1)
	{
		fprintf(stderr, "error: parse: loads parameter (problem)\n");
		fprintf(stderr, "usage: loads parameter (problem): loads=count\n");
		return 1;
	}
	if(lcount < 0)
	{
		fprintf(stderr, "error: count: loads parameter (problem): %d not positive nor zero\n", lcount);
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
		if(scanf("joint=[%d] force=<%le %le>\n", &jindex, &load.action.f[0], &load.action.f[1]) != 3)
		{
			fprintf(stderr, "error: parse: load line [%d] (problem)\n", l + 1);
			fprintf(stderr, "usage: load line (problem): joint=[index] force=<float float>\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: load line [%d] (problem): joint parameter: [%d] does not exist\n", l + 1, jindex + 1);
			return 1;
		}
		load.action.m = &joints[jindex].mass;
		loads[l] = load;
	}
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
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		if(scanf("force=<%*f %*f> position=(%le %le) velocity=<%le %le>\n",
		         &joint->mass.p[0], &joint->mass.p[1], &joint->mass.v[0], &joint->mass.v[1]) != 4)
		{
			fprintf(stderr, "error: parse: joint line [%d] (solution)\n", j + 1);
			fprintf(stderr, "usage: joint line (solution): mass=float position=(float float) velocity=(float float)\n");
			return 1;
		}
	}
	printf("joints=%d\n", jcount);
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		printf("mass=%.9e position=(%.9e %.9e) velocity=<%.9e %.9e>\n",
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
		printf("joint1=[%d] joint2=[%d] stiffness=%.9e length0=%.9e dampening=%.9e\n",
		       jindex1 + 1, jindex2 + 1, member->spring.k, member->spring.l0, member->damper.c);
	}
	printf("supports=%d\n", scount);
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == support->constraint.m)
				jindex = j;
		char axes[101];
		if(support->constraint.a[0] && support->constraint.a[1])
			sprintf(axes, "xy");
		if(support->constraint.a[0] && !support->constraint.a[1])
			sprintf(axes, "x");
		if(!support->constraint.a[0] && support->constraint.a[1])
			sprintf(axes, "y");
		printf("joint=[%d] axes=%s\n", jindex + 1, axes);
	}
	printf("loads=%d\n", lcount);
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == load->action.m)
				jindex = j;
		printf("joint=[%d] force=<%.9e %.9e>\n", jindex + 1, load->action.f[0], load->action.f[1]);
	}
	free(joints);
	free(members);
	free(supports);
	free(loads);
	return 0;
}
