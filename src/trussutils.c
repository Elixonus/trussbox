#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "msd.h"

constexpr double pi = 4.0 * atan(1.0);

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

void print_truss_problem(void)
{
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
		printf("joint1=%d joint2=%d stiffness=%.9e length0=%.9e dampening=%.9e\n",
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
		printf("joint=%d axes=%s\n", jindex + 1, axes);
	}
	printf("loads=%d\n", lcount);
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == load->action.m)
			jindex = j;
		printf("joint=%d force=<%.9e %.9e>\n", jindex + 1, load->action.f[0], load->action.f[1]);
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
		fprintf(stderr, "error: count: joints parameter: %d not positive nor zero\n", jcount);
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
	}
	if(scanf("members=%d\n", &mcount) != 1)
	{
		fprintf(stderr, "error: parse: members parameter\n");
		fprintf(stderr, "usage: members parameter: members=count\n");
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
		fprintf(stderr, "error: create: members array: %zd bytes allocation\n", mcount * sizeof(struct member));
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
	}
	if(scanf("supports=%d\n", &scount) != 1)
	{
		fprintf(stderr, "error: parse: supports parameter\n");
		fprintf(stderr, "usage: supports parameter: supports=count\n");
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
		fprintf(stderr, "error: create: supports array: %zd bytes allocation\n", scount * sizeof(struct support));
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
			support.constraint.a[0] = true, support.constraint.a[1] = true;
		else if(strcmp(axes, "x") == 0)
			support.constraint.a[0] = true, support.constraint.a[1] = false;
		else if(strcmp(axes, "y") == 0)
			support.constraint.a[0] = false, support.constraint.a[1] = true;
		else
		{
			fprintf(stderr, "error: parse: support line (%d): axes parameter: %s not an option\n", s + 1, axes);
			fprintf(stderr, "usage: support line: axes parameter: axes=xy|x|y\n");
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
		fprintf(stderr, "error: count: loads parameter: %d not positive nor zero\n", lcount);
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
	return 0;
}

void free_truss_problem(void)
{
	free(joints);
	free(members);
	free(supports);
	free(loads);
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "error: count: arguments: %d of 2+ provided\n", argc);
		fprintf(stderr, "usage: arguments: %s properties|transform|undeform|printart (...)\n", argv[0]);
		return 1;
	}
	if(strcmp(argv[1], "properties") == 0)
	{
		if(argc != 3)
		{
			fprintf(stderr, "error: count: arguments: %d of 3 provided\n", argc);
			fprintf(stderr, "usage: arguments: %s properties gravity=float\n", argv[0]);
			return 1;
		}
		double gravity;
		if(sscanf(argv[2], "gravity=%lf", &gravity) != 1)
		{
			fprintf(stderr, "error: parse: gravity argument (2): %s\n", argv[2]);
			fprintf(stderr, "usage: gravity argument (2): gravity=float\n");
			return 1;
		}
		if(scan_truss_problem() != 0) return 1;
		double mass = 0.0;
		for(int j = 0; j < jcount; j++)
			mass += joints[j].mass.m;
		printf("mass=%.9e\n", mass);
		double center[2] = {0.0, 0.0};
		for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
				center[a] += joints[j].mass.m * joints[j].mass.p[a];
		for(int a = 0; a < 2; a++)
			center[a] /= mass;
		printf("center=(%.9e %.9e)\n", center[0], center[1]);
		double momentum[2] = {0.0, 0.0};
		for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
				momentum[a] += joints[j].mass.m * joints[j].mass.v[a];
		printf("momentum=<%.9e %.9e>\n", momentum[0], momentum[1]);
		double energy = 0.0;
		for(int j = 0; j < jcount; j++)
		{
			energy += gravity * joints[j].mass.m * joints[j].mass.p[1];
			double speed = sqrt(pow(joints[j].mass.v[0], 2.0) + pow(joints[j].mass.v[1], 2.0));
			energy += 0.5 * joints[j].mass.m * pow(speed, 2.0);
		}
		for(int m = 0; m < mcount; m++)
			energy += 0.5 * members[m].spring.k * pow(sdisplacement(&members[m].spring), 2.0);
		printf("energy=%.9e\n", energy);
		free_truss_problem();
	}
	else if(strcmp(argv[1], "transform") == 0)
	{
		if(scan_truss_problem() != 0) return 1;
		for(int a = 2; a < argc; a++)
		{
			if(strncmp(argv[a], "translate", 9) == 0)
			{
				double translation[2];
				if(sscanf(argv[a], "translate=<%lf %lf>", &translation[0], &translation[1]) != 2)
				{
					fprintf(stderr, "error: parse: translate argument (%d): %s\n", a, argv[a]);
					fprintf(stderr, "usage: translate argument: translate=<float float>\n");
					return 1;
				}
				for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
						joints[j].mass.p[a] += translation[a];
			}
			else if(strncmp(argv[a], "rotate", 6) == 0)
			{
				double rotation;
				if(sscanf(argv[a], "rotate=%lf", &rotation) != 1)
				{
					fprintf(stderr, "error: parse: rotate argument (%d): %s\n", a, argv[a]);
					fprintf(stderr, "usage: rotate argument: rotate=float\n");
					return 1;
				}
				double radians = pi / 180.0 * rotation;
				for(int j = 0; j < jcount; j++)
				{
					double position[2] = {joints[j].mass.p[0], joints[j].mass.p[1]};
					joints[j].mass.p[0] = position[0] * cos(radians) - position[1] * sin(radians);
					joints[j].mass.p[1] = position[0] * sin(radians) + position[1] * cos(radians);
				}
				for(int l = 0; l < lcount; l++)
				{
					double force[2] = {loads[l].action.f[0], loads[l].action.f[1]};
					loads[l].action.f[0] = force[0] * cos(radians) - force[1] * sin(radians);
					loads[l].action.f[1] = force[0] * sin(radians) + force[1] * cos(radians);
				}
			}
			else if(strncmp(argv[a], "scale", 5) == 0)
			{
				double scale;
				if(sscanf(argv[a], "scale=%lf", &scale) != 1)
				{
					fprintf(stderr, "error: parse: scale argument (%d): %s\n", a, argv[a]);
					fprintf(stderr, "usage: scale argument: scale=float\n");
					return 1;
				}
				for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
						joints[j].mass.p[a] *= scale;
				for(int m = 0; m < mcount; m++)
					members[m].spring.l0 *= scale;
			}
			else
			{
				fprintf(stderr, "error: parse: argument (%d): %s\n", a, argv[a]);
				fprintf(stderr, "usage: argument: translate...|rotate...|scale...\n");
				return 1;
			}
		}
		print_truss_problem();
		free_truss_problem();
	}
	else if(strcmp(argv[1], "undeform") == 0)
	{
		if(argc != 2)
		{
			fprintf(stderr, "error: count: arguments: %d of 2 provided\n", argc);
			fprintf(stderr, "usage: arguments: %s undeform\n", argv[0]);
			return 1;
		}
		if(scan_truss_problem() != 0) return 1;
		for(int m = 0; m < mcount; m++)
			members[m].spring.l0 = mdistance(members[m].spring.m1, members[m].spring.m2);
		print_truss_problem();
		free_truss_problem();
	}
	else if(strcmp(argv[1], "printart") == 0)
	{
		if(argc != 5)
		{
			fprintf(stderr, "error: count: arguments: %d of 5 provided\n", argc);
			fprintf(stderr, "usage: arguments: %s printart gravity=float fcenter=(float float) fzoom=float\n", argv[0]);
			return 1;
		}
		double gravity;
		if(sscanf(argv[2], "gravity=%lf", &gravity) != 1)
		{
			fprintf(stderr, "error: parse: gravity argument (2): %s\n", argv[2]);
			fprintf(stderr, "usage: gravity argument (2): gravity=float\n");
			return 1;
		}
		double fcenter[2];
		if(sscanf(argv[3], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
		{
			fprintf(stderr, "error: parse: fcenter argument (3): %s\n", argv[3]);
			fprintf(stderr, "usage: fcenter argument (3): fcenter=(float float)\n");
			return 1;
		}
		double fzoom;
		if(sscanf(argv[4], "fzoom=%lf", &fzoom) != 1)
		{
			fprintf(stderr, "error: parse: fzoom argument (4): %s\n", argv[4]);
			fprintf(stderr, "usage: fzoom argument (4): fzoom=float\n");
			return 1;
		}
		if(fzoom < epsilon)
		{
			fprintf(stderr, "error: limit: fzoom argument: %.1e not greater than %.1e\n", fzoom, epsilon);
			return 1;
		}
		if(scan_truss_problem() != 0) return 1;
		char textart[50][51];
		for(int y = 0; y < 50; y++)
		{
			for(int x = 0; x < 50; x++)
				textart[y][x] = ' ';
			textart[y][50] = '\n';
		}
		for(int m = 0; m < mcount; m++)
		{
			struct member *member = &members[m];
			int rowcol1[2] = {
				(int) round(50.0 * (0.5 - fzoom * (member->spring.m1->p[1] - fcenter[1])))
				(int) round(50.0 * (0.5 + fzoom * (member->spring.m1->p[0] - fcenter[0])))
			};
			rowcol1[a] = ;
			rowcol2[a] = ;
			printf("row=%d,col=%d\n", rowcol1[0], rowcol1[1]);
		}
		free_truss_problem();
	}
	else {
		fprintf(stderr, "error: parse: utility argument (1): %s\n", argv[1]);
		fprintf(stderr, "usage: utility argument (1): properties|transform|undeform|printart\n");
		return 1;
	}
	return 0;
}
