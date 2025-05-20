#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MSDAXES 2
#include "msd.h"
#include "pipeline.h"

#define EPSILON 1.0e-18

constexpr double PI = 4.0 * atan(1.0);
constexpr double TAU = 2.0 * PI;

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

int main(int argc, char **argv)
{
	if(argc < 2) goto utilargerr;
	if(strcmp(argv[1], "genpipeline") == 0)
	{
		allocate_paths(1000);
		if(argc < 3 || sscanf(argv[2], "solvetruss_executable=%1000s", solvetruss_executable) != 1)
		{
			fprintf(stderr, "error: parse: solvetruss_executable argument\n");
			fprintf(stderr, "usage: solvetruss_executable argument: solvetruss_executable=string\n");
			return 1;
		}
		if(argc < 4 || sscanf(argv[3], "rendertruss_executable=%1000s", rendertruss_executable) != 1)
		{
			fprintf(stderr, "error: parse: rendertruss_executable argument\n");
			fprintf(stderr, "usage: rendertruss_executable argument: rendertruss_executable=string\n");
			return 1;
		}
		if(argc < 5 || sscanf(argv[4], "forcediagram_executable=%1000s", forcediagram_executable) != 1)
		{
			fprintf(stderr, "error: parse: forcediagram_executable argument\n");
			fprintf(stderr, "usage: forcediagram_executable argument: forcediagram_executable=string\n");
			return 1;
		}
		if(argc < 6 || sscanf(argv[5], "trussutils_executable=%1000s", trussutils_executable) != 1)
		{
			fprintf(stderr, "error: parse: trussutils_executable argument\n");
			fprintf(stderr, "usage: trussutils_executable argument: trussutils_executable=string\n");
			return 1;
		}
		if(argc < 7 || sscanf(argv[6], "problem_filename=%1000s", problem_filename) != 1)
		{
			fprintf(stderr, "error: parse: problem_filename argument\n");
			fprintf(stderr, "usage: problem_filename argument: problem_filename=string\n");
			return 1;
		}
		if(argc < 8 || sscanf(argv[7], "output_dirname=%1000s", output_dirname) != 1)
		{
			fprintf(stderr, "error: parse: output_dirname argument\n");
			fprintf(stderr, "usage: output_dirname argument: output_dirname=string\n");
			return 1;
		}
		if(argc < 9 || sscanf(argv[8], "gacceleration=%le", &gacceleration) != 1)
		{
			fprintf(stderr, "error: parse: gacceleration argument\n");
			fprintf(stderr, "usage: gacceleration argument: gacceleration=float\n");
			return 1;
		}
		if(argc < 10 || sscanf(argv[9], "timef=%le", &timef) != 1)
		{
			fprintf(stderr, "error: parse: timef argument\n");
			fprintf(stderr, "usage: timef argument: timef=float\n");
			return 1;
		}
		if(timef < EPSILON)
		{
			fprintf(stderr, "error: limit: timef argument: %.1le not greater than %.1le\n", timef, EPSILON);
			return 1;
		}
		if(argc < 11 || sscanf(argv[10], "srate=%le", &srate) != 1)
		{
			fprintf(stderr, "error: parse: srate argument\n");
			fprintf(stderr, "usage: srate argument: srate=float\n");
			return 1;
		}
		if(srate < EPSILON)
		{
			fprintf(stderr, "error: limit: srate argument: %.1le not greater than %.1le\n", srate, EPSILON);
			return 1;
		}
		dtime = 1.0 / srate;
		stepf = ((int) floor(srate * timef)) - 1;
		if(stepf < 0)
		{
			fprintf(stderr, "error: limit: stepf variable: %d not positive\n", stepf + 1);
			return 1;
		}
		if(argc < 12 || sscanf(argv[11], "frate=%le", &frate) != 1)
		{
			fprintf(stderr, "error: parse: frate argument\n");
			fprintf(stderr, "usage: frate argument: frate=float\n");
			return 1;
		}
		if(frate < EPSILON)
		{
			fprintf(stderr, "error: limit: frate argument: %.1le not greater than %.1le\n", frate, EPSILON);
			return 1;
		}
		framef = ((int) floor(frate * timef)) - 1;
		if(framef < 0)
		{
			fprintf(stderr, "error: limit: framef variable: %d not positive\n", framef + 1);
			return 1;
		}
		if(argc < 13 || sscanf(argv[12], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
		{
			fprintf(stderr, "error: parse: fsize argument\n");
			fprintf(stderr, "usage: fsize argument: fsize=integerxinteger\n");
			return 1;
		}
		if(fsize[0] < 64 || fsize[1] < 64)
		{
			fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than %dx%d nor matching\n", fsize[0], fsize[1], 64, 64);
			return 1;
		}
		if(argc < 14 || sscanf(argv[13], "fcenter=(%le %le)", &fcenter[0], &fcenter[1]) != 2)
		{
			fprintf(stderr, "error: parse: fcenter argument\n");
			fprintf(stderr, "usage: fcenter argument: fcenter=(float float)\n");
			return 1;
		}
		if(argc < 15 || sscanf(argv[14], "fzoom=%le", &fzoom) != 1)
		{
			fprintf(stderr, "error: parse: fzoom argument\n");
			fprintf(stderr, "usage: fzoom argument: fzoom=float\n");
			return 1;
		}
		if(fzoom < EPSILON)
		{
			fprintf(stderr, "error: limit: fzoom argument: %.1le not greater than %.1le\n", fzoom, EPSILON);
			return 1;
		}
		if(argc < 16 || sscanf(argv[15], "fscale=%le", &fscale) != 1)
		{
			fprintf(stderr, "error: parse: fscale argument\n");
			fprintf(stderr, "usage: fscale argument: fscale=float\n");
			return 1;
		}
		if(fscale < EPSILON)
		{
			fprintf(stderr, "error: limit: fscale argument: %.1le not greater than %.1le\n", fscale, EPSILON);
			return 1;
		}
		initialize(stdout);
		while(check_frame())
		{
			rendertruss(stdout);
			solvetruss(stdout);
			concatenate_problem_and_solution(stdout);
			forcediagram(stdout);
			feedback_solution_into_problem(stdout);
			progress_frame_and_step();
		}
		free_paths();
	}
	else if(strcmp(argv[1], "properties") == 0)
	{
		double gacceleration;
		if(argc < 3 || sscanf(argv[2], "gacceleration=%le", &gacceleration) != 1)
		{
			fprintf(stderr, "error: parse: gacceleration argument\n");
			fprintf(stderr, "usage: gacceleration argument: gacceleration=float\n");
			return 1;
		}
		if(scan_truss_problem() != 0) return 1;
		double mass = 0.0;
		for(int j = 0; j < jcount; j++)
			mass += joints[j].mass.m;
		printf("mass=%.9le\n", mass);
		double center[2] = {0.0, 0.0};
		for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
				center[a] += joints[j].mass.m * joints[j].mass.p[a];
		for(int a = 0; a < 2; a++)
			center[a] /= mass;
		printf("center=(%.9le %.9le)\n", center[0], center[1]);
		double momentum[2] = {0.0, 0.0};
		for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
				momentum[a] += joints[j].mass.m * joints[j].mass.v[a];
		printf("momentum=<%.9le %.9le>\n", momentum[0], momentum[1]);
		double energy = 0.0;
		for(int j = 0; j < jcount; j++)
		{
			energy -= gacceleration * joints[j].mass.m * joints[j].mass.p[1];
			double speed = sqrt(pow(joints[j].mass.v[0], 2.0) + pow(joints[j].mass.v[1], 2.0));
			energy += 0.5 * joints[j].mass.m * pow(speed, 2.0);
		}
		for(int m = 0; m < mcount; m++)
			energy += 0.5 * members[m].spring.k * pow(sdisplacement(&members[m].spring), 2.0);
		printf("energy=%.9le\n", energy);
		free_truss_problem();
	}
	else if(strcmp(argv[1], "transform") == 0)
	{
		if(scan_truss_problem() != 0) return 1;
		for(int a = 2; a < argc; a++)
		{
			if(strncmp(argv[a], "translate", strlen("translate")) == 0)
			{
				double translation[2];
				if(sscanf(argv[a], "translate=<%le %le>", &translation[0], &translation[1]) != 2)
				{
					fprintf(stderr, "error: parse: translate argument [%d]\n", a);
					fprintf(stderr, "usage: translate argument: translate=<float float>\n");
					return 1;
				}
				for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
						joints[j].mass.p[a] += translation[a];
			}
			else if(strncmp(argv[a], "rotate", strlen("rotate")) == 0)
			{
				double rotation;
				if(sscanf(argv[a], "rotate=%le", &rotation) != 1)
				{
					fprintf(stderr, "error: parse: rotate argument [%d]\n", a);
					fprintf(stderr, "usage: rotate argument: rotate=float\n");
					return 1;
				}
				double radians = PI / 180.0 * rotation;
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
			else if(strncmp(argv[a], "scale", strlen("scale")) == 0)
			{
				double scale;
				if(sscanf(argv[a], "scale=%le", &scale) != 1)
				{
					fprintf(stderr, "error: parse: scale argument [%d]\n", a);
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
				fprintf(stderr, "error: parse: argument [%d]\n", a);
				fprintf(stderr, "usage: transform argument: translate...|rotate...|scale...\n");
				return 1;
			}
		}
		print_truss_problem();
		free_truss_problem();
	}
	else if(strcmp(argv[1], "undeform") == 0)
	{
		if(scan_truss_problem() != 0) return 1;
		for(int m = 0; m < mcount; m++)
			members[m].spring.l0 = mdistance(members[m].spring.m1, members[m].spring.m2);
		print_truss_problem();
		free_truss_problem();
	}
	else if(strcmp(argv[1], "feedback") == 0)
	{
		if(scan_truss_problem() != 0) return 1;
		if(scan_truss_solution(true) != 0) return 1;
		print_truss_problem();
		free_truss_problem();
	}
	else if(strcmp(argv[1], "textart") == 0)
	{
		double fcenter[2];
		if(argc < 3 || sscanf(argv[2], "fcenter=(%le %le)", &fcenter[0], &fcenter[1]) != 2)
		{
			fprintf(stderr, "error: parse: fcenter argument\n");
			fprintf(stderr, "usage: fcenter argument: fcenter=(float float)\n");
			return 1;
		}
		double fzoom;
		if(argc < 4 || sscanf(argv[3], "fzoom=%le", &fzoom) != 1)
		{
			fprintf(stderr, "error: parse: fzoom argument\n");
			fprintf(stderr, "usage: fzoom argument: fzoom=float\n");
			return 1;
		}
		if(fzoom < EPSILON)
		{
			fprintf(stderr, "error: limit: fzoom argument: %.1le not greater than %.1le\n", fzoom, EPSILON);
			return 1;
		}
		char colorarg[101];
		if(argc < 5 || sscanf(argv[4], "color=%100s", colorarg) != 1)
		{
			fprintf(stderr, "error: parse: color argument\n");
			fprintf(stderr, "usage: color argument: color=true|false\n");
			return 1;
		}
		bool usecolor;
		if(strcmp(colorarg, "true") == 0)
			usecolor = true;
		else if(strcmp(colorarg, "false") == 0)
			usecolor = false;
		else
		{
			fprintf(stderr, "error: parse: color argument: not an option\n");
			fprintf(stderr, "usage: color argument: color=true|false\n");
			return 1;
		}
		char croparg[101];
		if(argc < 6 || sscanf(argv[5], "vcrop=%100s", croparg) != 1)
		{
			fprintf(stderr, "error: parse: vcrop argument\n");
			fprintf(stderr, "usage: vcrop argument: vcrop=true|false\n");
			return 1;
		}
		bool docrop;
		if(strcmp(croparg, "true") == 0)
			docrop = true;
		else if(strcmp(croparg, "false") == 0)
			docrop = false;
		else
		{
			fprintf(stderr, "error: parse: vcrop argument: not an option\n");
			fprintf(stderr, "usage: vcrop argument: vcrop=true|false\n");
			return 1;
		}
		char title[51];
		if(argc < 7 || sscanf(argv[6], "title=%50[^\n]", title) != 1)
		{
			fprintf(stderr, "error: parse: title argument\n");
			fprintf(stderr, "usage: title argument: title=string\n");
			return 1;
		}
		if(scan_truss_problem() != 0) return 1;
		if(usecolor)
			if(scan_truss_solution(false) != 0) return 1;
		char textart[25][52];
		char colors[25][50];
		for(int r = 0; r < 25; r++)
		{
			for(int c = 0; c < 50; c++)
			{
				textart[r][c] = ' ';
				if(usecolor) colors[r][c] = ' ';
			}
			textart[r][50] = '\n', textart[r][51] = '\0';
		}
		void setchar(char ch, int r, int c, char clr)
		{
			if(r < 0 || c < 0 || r >= 25 || c >= 50) return;
			textart[r][c] = ch;
			if(usecolor)
				colors[r][c] = clr;
		}
		double cutoff_force;
		if(usecolor)
		{
			cutoff_force = 0.0;
			for(int l = 0; l < lcount; l++)
			{
				double force = sqrt(pow(loads[l].action.f[0], 2.0) + pow(loads[l].action.f[1], 2.0));
				if(force > cutoff_force)
					cutoff_force = force;
			}
			for(int s = 0; s < scount; s++)
			{
				double force = sqrt(pow(sreactions[s][0], 2.0) + pow(sreactions[s][1], 2.0));
				if(force > cutoff_force)
					cutoff_force = force;
			}
			for(int m = 0; m < mcount; m++)
			{
				double force = fabs(mforces[m]);
				if(force > cutoff_force)
					cutoff_force = force;
			}
		}
		for(int m = 0; m < mcount; m++)
		{
			struct member *member = &members[m];
			double force;
			char color;
			if(usecolor)
			{
				force = mforces[m];
				if(cutoff_force < EPSILON)
				{
					color = 'G';
					goto bresenham;
				}
				double balance = force / cutoff_force;
				balance = balance < 1.0 ? (balance > -1.0 ? balance : -1.0) : 1.0;
				if(fabs(balance) < 0.2)
					color = 'G';
				else if(balance < 0.0)
					color = 'r';
				else
					color = 'b';
			}
			bresenham:
			int rowcol1[2] = {
				(int) round(24.0 * (0.5 - fzoom * (member->spring.m1->p[1] - fcenter[1]))),
				(int) round(49.0 * (0.5 + fzoom * (member->spring.m1->p[0] - fcenter[0])))
			};
			int rowcol2[2] = {
				(int) round(24.0 * (0.5 - fzoom * (member->spring.m2->p[1] - fcenter[1]))),
				(int) round(49.0 * (0.5 + fzoom * (member->spring.m2->p[0] - fcenter[0])))
			};
			int r1 = rowcol1[0], c1 = rowcol1[1];
			int r2 = rowcol2[0], c2 = rowcol2[1];
			int dc = abs(c2 - c1), sc = c1 < c2 ? 1 : -1;
			int dr = -abs(r2 - r1), sr = r1 < r2 ? 1 : -1;
			int error1 = dc + dr, error2;
			while(true)
			{
				bool bc = error2 >= dr, br = error2 <= dc;
				if(bc && br)
				{
					if(sc == sr)
						setchar('\\', r1, c1, color);
					else
						setchar('/', r1, c1, color);
				}
				else if(bc)
					setchar('-', r1, c1, color);
				else if(br)
					setchar('|', r1, c1, color);
				else
					setchar('*', r1, c1, color);
				if(c1 == c2 && r1 == r2) break;
				error2 = 2 * error1;
				if(error2 >= dr)
				{
					error1 += dr;
					c1 += sc;
				}
				if(error2 <= dc)
				{
					error1 += dc;
					r1 += sr;
				}
			}
		}
		for(int l = 0; l < lcount; l++)
		{
			struct load *load = &loads[l];
			double magnitude = sqrt(pow(load->action.f[0], 2.0) + pow(load->action.f[1], 2.0));
			if(magnitude < EPSILON) continue;
			double angle = fmod(atan2(load->action.f[1], load->action.f[0]) + TAU, TAU);
			int rowcol[2] = {
				(int) round(24.0 * (0.5 - fzoom * (load->action.m->p[1] - fcenter[1]))),
				(int) round(49.0 * (0.5 + fzoom * (load->action.m->p[0] - fcenter[0])))
			};
			if(angle < 0.25 * PI || angle > 1.75 * PI) setchar('>', rowcol[0], rowcol[1] + 1, 'y');
			else if(angle < 0.75 * PI) setchar('^', rowcol[0] - 1, rowcol[1], 'y');
			else if(angle < 1.25 * PI) setchar('<', rowcol[0], rowcol[1] - 1, 'y');
			else setchar('v', rowcol[0] + 1, rowcol[1], 'y');
		}
		for(int j = 0; j < jcount; j++)
		{
			struct joint *joint = &joints[j];
			int rowcol[2] = {
				(int) round(24.0 * (0.5 - fzoom * (joint->mass.p[1] - fcenter[1]))),
				(int) round(49.0 * (0.5 + fzoom * (joint->mass.p[0] - fcenter[0])))
			};
			setchar('*', rowcol[0], rowcol[1], ' ');
		}
		for(int s = 0; s < scount; s++)
		{
			struct support *support = &supports[s];
			int count = support->constraint.a[0] + support->constraint.a[1];
			double ncenter[2] = {0.0, 0.0};
			int ncount = 0;
			for(int m = 0; m < mcount; m++)
			{
				struct member *member = &members[m];
				if(member->spring.m1 == support->constraint.m)
				{
					for(int a = 0; a < 2; a++)
						ncenter[a] += member->spring.m2->p[a];
					ncount++;
				}
				if(member->spring.m2 == support->constraint.m)
				{
					for(int a = 0; a < 2; a++)
						ncenter[a] += member->spring.m1->p[a];
					ncount++;
				}
			}
			for(int a = 0; a < 2; a++)
			{
				if(ncount > 0)
					ncenter[a] /= ncount;
				else
					ncenter[a] = support->constraint.m->p[a];
			}
			double polarity;
			if(count == 2 || (count == 1 && support->constraint.a[1]))
				polarity = ncenter[1] >= support->constraint.m->p[1] ? 1.0 : -1.0;
			if(count == 1 && support->constraint.a[0])
				polarity = ncenter[0] >= support->constraint.m->p[0] ? 1.0 : -1.0;
			int rowcol[2] = {
				(int) round(24.0 * (0.5 - fzoom * (support->constraint.m->p[1] - fcenter[1]))),
				(int) round(49.0 * (0.5 + fzoom * (support->constraint.m->p[0] - fcenter[0])))
			};
			if(count == 2 || (count == 1 && support->constraint.a[1]))
			{
				if(polarity > 0.0)
				{
					setchar('[', rowcol[0], rowcol[1] - 1, ' ');
					setchar(']', rowcol[0], rowcol[1] + 1, ' ');
					setchar('/', rowcol[0] + 1, rowcol[1] - 1, ' ');
					setchar(' ', rowcol[0] + 1, rowcol[1], ' ');
					setchar('\\', rowcol[0] + 1, rowcol[1] + 1, ' ');
					if(count == 2)
					{
						setchar('=', rowcol[0] + 2, rowcol[1] - 2, ' ');
						setchar('=', rowcol[0] + 2, rowcol[1] - 1, ' ');
						setchar('=', rowcol[0] + 2, rowcol[1], ' ');
						setchar('=', rowcol[0] + 2, rowcol[1] + 1, ' ');
						setchar('=', rowcol[0] + 2, rowcol[1] + 2, ' ');
					}
					if(count == 1)
					{
						setchar('o', rowcol[0] + 2, rowcol[1] - 2, ' ');
						setchar('o', rowcol[0] + 2, rowcol[1] - 1, ' ');
						setchar('o', rowcol[0] + 2, rowcol[1], ' ');
						setchar('o', rowcol[0] + 2, rowcol[1] + 1, ' ');
						setchar('o', rowcol[0] + 2, rowcol[1] + 2, ' ');
					}
				}
				else
				{
					setchar('[', rowcol[0], rowcol[1] - 1, ' ');
					setchar(']', rowcol[0], rowcol[1] + 1, ' ');
					setchar('\\', rowcol[0] - 1, rowcol[1] - 1, ' ');
					setchar(' ', rowcol[0] - 1, rowcol[1], ' ');
					setchar('/', rowcol[0] - 1, rowcol[1] + 1, ' ');
					if(count == 2)
					{
						setchar('=', rowcol[0] - 2, rowcol[1] - 2, ' ');
						setchar('=', rowcol[0] - 2, rowcol[1] - 1, ' ');
						setchar('=', rowcol[0] - 2, rowcol[1], ' ');
						setchar('=', rowcol[0] - 2, rowcol[1] + 1, ' ');
						setchar('=', rowcol[0] - 2, rowcol[1] + 2, ' ');
					}
					if(count == 1)
					{
						setchar('o', rowcol[0] - 2, rowcol[1] - 2, ' ');
						setchar('o', rowcol[0] - 2, rowcol[1] - 1, ' ');
						setchar('o', rowcol[0] - 2, rowcol[1], ' ');
						setchar('o', rowcol[0] - 2, rowcol[1] + 1, ' ');
						setchar('o', rowcol[0] - 2, rowcol[1] + 2, ' ');
					}
				}
			}
			if(count == 1 && support->constraint.a[0])
			{
				if(polarity > 0.0)
				{
					setchar('-', rowcol[0] - 1, rowcol[1], ' ');
					setchar('-', rowcol[0] + 1, rowcol[1], ' ');
					setchar('\\', rowcol[0] - 1, rowcol[1] - 1, ' ');
					setchar(' ', rowcol[0], rowcol[1] - 1, ' ');
					setchar('/', rowcol[0] + 1, rowcol[1] - 1, ' ');
					setchar('o', rowcol[0] - 2, rowcol[1] - 2, ' ');
					setchar('o', rowcol[0] - 1, rowcol[1] - 2, ' ');
					setchar('o', rowcol[0], rowcol[1] - 2, ' ');
					setchar('o', rowcol[0] + 1, rowcol[1] - 2, ' ');
					setchar('o', rowcol[0] + 2, rowcol[1] - 2, ' ');
				}
				else
				{
					setchar('-', rowcol[0] - 1, rowcol[1], ' ');
					setchar('-', rowcol[0] + 1, rowcol[1], ' ');
					setchar('/', rowcol[0] - 1, rowcol[1] + 1, ' ');
					setchar(' ', rowcol[0], rowcol[1] + 1, ' ');
					setchar('\\', rowcol[0] + 1, rowcol[1] + 1, ' ');
					setchar('o', rowcol[0] - 2, rowcol[1] + 2, ' ');
					setchar('o', rowcol[0] - 1, rowcol[1] + 2, ' ');
					setchar('o', rowcol[0], rowcol[1] + 2, ' ');
					setchar('o', rowcol[0] + 1, rowcol[1] + 2, ' ');
					setchar('o', rowcol[0] + 2, rowcol[1] + 2, ' ');
				}
			}
		}
		int rowstart = 0, rowend = 24;
		if(docrop)
		{
			for(int r = 0; r < 25; r++)
			{
				bool blankline = true;
				for(int c = 0; c < 50; c++)
				{
					if(textart[r][c] != ' ')
					{
						blankline = false;
						break;
					}
				}
				if(!blankline) break;
				rowstart++;
			}
			for(int r = 24; r > rowstart; r--)
			{
				bool blankline = true;
				for(int c = 0; c < 50; c++)
				{
					if(textart[r][c] != ' ')
					{
						blankline = false;
						break;
					}
				}
				if(!blankline) break;
				rowend--;
			}
		}
		if(strcmp(title, "") != 0)
		{
			int padding = (50 - strlen(title)) / 2;
			if(usecolor)
			{
				printf("\e[0;30m\e[47m%*s%s%*s\e[0m\n", padding, "", title, padding + ((int) strlen(title)) % 2, "");
			}
			else
				printf("%*s%s%*s\n", padding, "", title, padding + ((int) strlen(title)) % 2, "");
			printf("%*s\n", 50, "");
		}
		for(int r = rowstart; r <= rowend; r++)
			if(usecolor)
			{
				for(int c = 0; c < 51; c++)
				{
					if(c < 50)
					{
						switch(colors[r][c])
						{
							case 'r':
								printf("\e[0;31m");
								break;
							case 'g':
								printf("\e[0;32m");
								break;
							case 'y':
								printf("\e[0;33m");
								break;
							case 'b':
								printf("\e[0;34m");
								break;
							case 'm':
								printf("\e[0;35m");
								break;
							case 'G':
								printf("\e[38;5;238m");
								break;
						}
					}
					printf("%c", textart[r][c]);
					if(c < 50)
						printf("\e[0m");
				}
			}
			else
				printf("%s", textart[r]);
		free_truss_problem();
		if(usecolor)
			free_truss_solution();
	}
	else {
		utilargerr:
		fprintf(stderr, "error: parse: utility argument\n");
		fprintf(stderr, "usage: utility argument: genpipeline|properties|transform|undeform|feedback|textart\n");
		return 1;
	}
	return 0;
}
