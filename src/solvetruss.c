#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cairo.h>
#include "dampspring.h"

constexpr double pi = 4.0 * atan(1.0);
constexpr double tau = 2.0 * pi;

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
double srate;
int stepf;
int step;

double frate;
int framef;
int frame;
int fsize[2];
double fcenter[2];
double fzoom;
double fscale;

double epsilon = 1.0e-18;
char dirname[1001];

int solve(void)
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
		double force = sforce(&member->spring) + dforce(&member->damper);
		double length = mdistance(member->spring.m1, member->spring.m2);
		if(length < epsilon) continue;
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
			direction[c] = (joints[jindex2].mass.p[c] - joints[jindex1].mass.p[c]) / length;
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
			}
		}
	}
	time += dtime;
	step++;
	return 0;
}

void render(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, fsize[0], fsize[1]);
	cairo_t *context = cairo_create(surface);
	cairo_save(context);
	cairo_rectangle(context, 0.0, 0.0, (double) fsize[0], (double) fsize[1]);
	cairo_clip(context);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_paint(context);
	cairo_restore(context);
	cairo_save(context);
	cairo_translate(context, 0.0, 0.5 * ((double) fsize[1]));
	cairo_scale(context, 1.0, -1.0);
	cairo_translate(context, 0.0, -0.5 * ((double) fsize[1]));
	cairo_save(context);
	cairo_translate(context, 0.5 * ((double) fsize[0]), 0.5 * ((double) fsize[1]));
	double length = fsize[0] < fsize[1] ? (double) fsize[0] : (double) fsize[1];
	cairo_scale(context, length, length);
	cairo_scale(context, fzoom, fzoom);
	cairo_translate(context, -fcenter[0], -fcenter[1]);
	cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_ROUND);
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		cairo_save(context);
		cairo_translate(context, support->constraint.m->p[0], support->constraint.m->p[1]);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		int count = support->constraint.a[0] + support->constraint.a[1];
		if(count == 0) continue;
		if(support->constraint.a[0] && !support->constraint.a[1])
		{
			cairo_rotate(context, -0.5 * pi);
		}
		double ncenter[2] = {0.0, 0.0};
		double nradius = 0.0;
		int ncount = 0;
		for(int m = 0; m < mcount; m++)
		{
			struct member *member = &members[m];
			if(member->spring.m1 == support->constraint.m)
			{
				for(int c = 0; c < 2; c++)
					ncenter[c] += member->spring.m2->p[c];
				nradius += mdistance(member->spring.m2, support->constraint.m);
				ncount++;
			}
			if(member->spring.m2 == support->constraint.m)
			{
				for(int c = 0; c < 2; c++)
					ncenter[c] += member->spring.m1->p[c];
				nradius += mdistance(member->spring.m1, support->constraint.m);
				ncount++;
			}
		}
		for(int c = 0; c < 2; c++)
		{
			ncenter[c] /= ncount;
			nradius /= ncount;	
		}
		cairo_new_path(context);
		cairo_line_to(context, 0.0, 0.0);
		double factor;
		if(count == 2 || (count == 1 && support->constraint.a[1]))
			factor = (ncenter[1] - support->constraint.m->p[1]) / nradius;
		if(count == 1 && support->constraint.a[0])
			factor = (ncenter[0] - support->constraint.m->p[0]) / nradius;
		factor = 2.0 / (1.0 + exp(-10.0 * factor)) - 1.0;
		cairo_scale(context, 1.0, factor >= 0.0 ? 1.0 : -1.0);
		double height = 0.0575 * fabs(factor);
		cairo_translate(context, 0.0, -height);
		cairo_line_to(context, 0.035, height > 0.0075 ? 0.0075 : height);
		cairo_line_to(context, -0.035, height > 0.0075 ? 0.0075 : height);
		cairo_close_path(context);
		cairo_new_sub_path(context);
		cairo_rectangle(context, -0.075, -0.0025, 0.15, 0.005);
		if(count == 1)
		{
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0675, -0.015, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.03375, -0.015, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0, -0.015, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.03375, -0.015, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.0675, -0.015, 0.0075, 0.0, tau);
			cairo_close_path(context);
		}
		cairo_set_line_width(context, 0.01);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(context);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill(context);
		cairo_restore(context);
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		cairo_save(context);
		cairo_translate(context, joint->mass.p[0], joint->mass.p[1]);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_new_path(context);
		cairo_arc(context, 0.0, 0.0, 0.02, 0.0, tau);
		cairo_close_path(context);
		cairo_set_line_width(context, 0.01);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(context);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill(context);
		cairo_restore(context);
	}
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		cairo_new_path(context);
		cairo_line_to(context, member->spring.m1->p[0], member->spring.m1->p[1]);
		cairo_line_to(context, member->spring.m2->p[0], member->spring.m2->p[1]);
		cairo_save(context);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_set_line_width(context, 0.02);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(context);
		cairo_set_line_width(context, 0.01);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_stroke(context);
		cairo_restore(context);
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		cairo_save(context);
		cairo_translate(context, joint->mass.p[0], joint->mass.p[1]);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_new_path(context);
		cairo_arc(context, 0.0, 0.0, 0.0025, 0, tau);
		cairo_close_path(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
		cairo_restore(context);
	}
	cairo_restore(context);
	cairo_restore(context);
	cairo_destroy(context);
	char filename[1101];
	sprintf(filename, "%s/%09d.png", dirname, frame + 1);
	cairo_surface_write_to_png(surface, filename);
	cairo_surface_destroy(surface);
	frame++;
}

int main(int argc, char **argv)
{
	if(argc != 10) return 1;
	if(sscanf(argv[1], "%1000s", dirname) != 1) return 1;
	if(sscanf(argv[2], "gravity=%lf", &gravity) != 1) return 1;
	if(sscanf(argv[3], "timef=%lf", &timef) != 1) return 1;
	if(timef < epsilon) return 1;
	if(sscanf(argv[4], "srate=%lf", &srate) != 1) return 1;
	if(srate < epsilon) return 1;
	dtime = 1.0 / srate;
	if(dtime < epsilon) return 1;
	stepf = ((int) round(srate * timef)) - 1;
	if(stepf < 0) return 1;
	if(sscanf(argv[5], "frate=%lf", &frate) != 1) return 1;
	if(frate < epsilon) return 1;
	framef = ((int) round(frate * timef)) - 1;
	if(framef < 0) return 1;
	if(sscanf(argv[6], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2) return 1;
	if(fsize[0] < 64 || fsize[1] < 64) return 1;
	if(sscanf(argv[7], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2) return 1;
	if(sscanf(argv[8], "fzoom=%lf", &fzoom) != 1) return 1;
	if(fzoom < epsilon) return 1;
	if(sscanf(argv[9], "fscale=%lf", &fscale) != 1) return 1;
	if(fscale < epsilon) return 1;
	if(scanf("joints=%d\n", &jcount) != 1) return 1;
	if(jcount < 0) return 1;
	joints = malloc(jcount * sizeof(struct joint));
	jaccelerations = malloc(jcount * sizeof(double *));
	jforces = malloc(jcount * sizeof(double *));
	for(int j = 0; j < jcount; j++)
	{
		struct joint joint;
		if(scanf(
			"mass=%lf position=(%lf %lf) velocity=<%lf %lf>\n",
			&joint.mass.m, &joint.mass.p[0], &joint.mass.p[1], &joint.mass.v[0], &joint.mass.v[1]
		) != 5) return 1;
		if(joint.mass.m < epsilon) return 1;
		joints[j] = joint;
		jaccelerations[j] = malloc(2 * sizeof(double));
		jforces[j] = malloc(2 * sizeof(double));
		for(int c = 0; c < 2; c++)
		{
			jaccelerations[j][c] = 0.0;
			jforces[j][c] = 0.0;
		}
	}
	if(scanf("members=%d\n", &mcount) != 1) return 1;
	if(mcount < 0) return 1;
	members = malloc(mcount * sizeof(struct member));
	mlengths = malloc(mcount * sizeof(double));
	mdisplacements = malloc(mcount * sizeof(double));
	mvelocities = malloc(mcount * sizeof(double));
	mforces = malloc(mcount * sizeof(double));
	for(int m = 0; m < mcount; m++)
	{
		int jindex1, jindex2;
		struct member member;
		if(scanf(
			"joint1=%d joint2=%d stiffness=%lf length0=%lf dampening=%lf\n",
			&jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c
		) != 5) return 1;
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount || jindex2 < 0 || jindex2 >= jcount) return 1;
		for(int m2 = 0; m2 < m; m2++)
			if(
				(
					members[m2].spring.m1 == &joints[jindex1].mass &&
					members[m2].spring.m2 == &joints[jindex2].mass
				) ||
				(
					members[m2].spring.m1 == &joints[jindex2].mass &&
					members[m2].spring.m2 == &joints[jindex1].mass
				)
			) return 1;
		member.spring.m1 = &joints[jindex1].mass;
		member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass;
		member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < epsilon) return 1;
		members[m] = member;
		mlengths[m] = 0.0;
		mdisplacements[m] = 0.0;
		mvelocities[m] = 0.0;
		mforces[m] = 0.0;
	}
	if(scanf("supports=%d\n", &scount) != 1) return 1;
	if(scount < 0) return 1;
	supports = malloc(scount * sizeof(struct support));
	sreactions = malloc(scount * sizeof(double *));
	for(int s = 0; s < scount; s++)
	{
		int jindex;
		char axes[3];
		struct support support;
		if(scanf("joint=%d axes=%2s\n", &jindex, axes) != 2) return 1;
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		for(int s2 = 0; s2 < s; s2++) if(supports[s2].constraint.m == &joints[jindex].mass) return 1;
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
		else return 1;
		for(int c = 0; c < 2; c++)
			support.constraint.p[c] = joints[jindex].mass.p[c];
		supports[s] = support;
		sreactions[s] = malloc(2 * sizeof(double));
		for(int c = 0; c < 2; c++) sreactions[s][c] = 0.0;
	}
	if(scanf("loads=%d\n", &lcount) != 1) return 1;
	if(lcount < 0) return 1;
	loads = malloc(lcount * sizeof(struct load));
	for(int l = 0; l < lcount; l++)
	{
		int jindex;
		struct load load;
		if(scanf(
			"joint=%d force=<%lf %lf>\n",
			&jindex, &load.action.f[0], &load.action.f[1]
		) != 3) return 1;
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		load.action.m = &joints[jindex].mass;
		loads[l] = load;
	}
	time = 0.0;
	step = 0;
	frame = 0;
	while(frame <= framef)
	{
		render();
		while(step * (framef + 1) < frame * (stepf + 1))
		{
			solve();
		}
	}
	printf("joints=%d\n", jcount);
	for(int j = 0; j < jcount; j++)
	{
		printf(
			"force=<%.9e %.9e> position=(%.9e %.9e) velocity=<%.9e %.9e>\n",
			jforces[j][0], jforces[j][1],
			joints[j].mass.p[0], joints[j].mass.p[1],
			joints[j].mass.v[0], joints[j].mass.v[1]
		);
	}
	printf("members=%d\n", mcount);
	for(int m = 0; m < mcount; m++)
	{
		printf(
			"force=%.9e displacement=%.9e length=%.9e velocity=%.9e\n",
			mforces[m], mdisplacements[m], mlengths[m], mvelocities[m]
		);
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
