#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cairo.h>
#include "dampspring.h"

constexpr double pi = 4.0 * atan(1.0);
constexpr double tau = 2.0 * pi;

struct joint {
	struct mass mass;
};

struct member {
	struct spring spring;
	struct damper damper;
};

struct support {
	struct constraint {
		bool a[2];
		struct mass *m;
		double p0[2];
	} constraint;
};

struct load {
	struct extenal {
		double f[2];
		struct mass *m;
	}
}

int jcount;
struct joint *joints;
double **jaccelerations;
double **jforces;

int mcount;
struct member *members;
double *mlengths;
double *mdisplacements;
double *mvelocities;
double *mforces;

int scount;
struct support *supports;
double **sreactions;

int lcount;
struct load *loads;

double epsilon = 1e-9;
double gravity;
double dtime;
double time;
int step;

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
		for(int c = 0; c < 2; c++)
		{
			jforces[j][c] = 0.0;
		}
		jforces[j][1] = -gravity * joints[j].mass.m;
	}
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == load->external.m) jindex = j;
		for(int c = 0; c < 2; c++)
		{
			jforces[jindex][c] += load->external.f[c];
		}
	}
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		double mforce = sforce(&member->spring) + dforce(&member->damper);
		mforces[m] = mforce;
		double mlength = mdistance(member->spring.m1, member->spring.m2);
		mlengths[m] = mlength;
		if(mlength < epsilon) continue;
		double mdirection[2];
		int jindex1, jindex2;
		for(int j = 0; j < jcount; j++)
		{
			if(&joints[j].mass == member->spring.m1) jindex1 = j;
			if(&joints[j].mass == member->spring.m2) jindex2 = j;
		}
		for(int c = 0; c < 2; c++)
		{
			mdirection[c] = (joints[jindex2].mass.p[c] - joints[jindex1].mass.p[c]) / mlength;
			jforces[jindex1][c] -= mdirection[c] * mforce;
			jforces[jindex2][c] += mdirection[c] * mforce;
		}
		mdisplacements[m] = sdisplacement(&member->spring);
		mvelocities[m] = dvelocity(&member->damper);
	}
	for(int j = 0; j < jcount; j++)
	{
		double jacceleration[2];
		for(int c = 0; c < 2; c++)
		{
			jacceleration[c] = jforces[j][c] / (joints[j].mass.m > epsilon ? joints[j].mass.m : epsilon);
			jaccelerations[j][c] = jacceleration[c];
		}
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int c = 0; c < 2; c++)
		{
			joint->mass.v[c] += 0.5 * jaccelerations[j][c] * dtime;
		}
	}
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		int jindex;
		for(int j = 0; j < jcount; j++) if(&joints[j].mass == support->constraint.m) jindex = j;
		for(int c = 0; c < 2; c++)
		{
			if(support->constraint.a[c])
			{
				support->constraint.m->p[c] = support->constraint.p0[c];
				support->constraint.m->v[c] = 0.0;
				sreactions[s][c] = -jforces[jindex][c];
			}
		}
	}
	time += dtime;
	step++;
	return 0;
}

int fsize[2];
double fcenter[2];
double fzoom;
double fscale;
int frame;

void render(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, fsize[0], fsize[1]);
	cairo_t *context = cairo_create(surface);
	cairo_save(context);
	cairo_translate(context, 0.0, 0.5 * ((double) fsize[1]));
	cairo_scale(context, 1.0, -1.0);
	cairo_translate(context, 0.0, -0.5 * ((double) fsize[1]));
	cairo_save(context);
	cairo_translate(context, 0.5 * ((double) fsize[0]), 0.5 * ((double) fsize[1]));
	cairo_save(context);
	cairo_scale(context, (double) fsize[0], (double) fsize[1]);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_paint(context);
	cairo_restore(context);
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
		cairo_scale(context, fscale, fscale);
		int count = support->constraint.a[0] + support->constraint.a[1];
		if(count == 0) continue;
		if(support->constraint.a[0] && !support->constraint.a[1])
		{
			cairo_rotate(context, 0.5 * pi);
		}
		int ncount = 0;
		double ncenter[2] = {0.0, 0.0};
		for(int m = 0; m < mcount; m++)
		{
			struct member *member = &members[m];
			if(member->spring.m1 == support->constraint.m)
			{
				for(int c = 0; c < 2; c++) ncenter[c] += member->spring.m2->p[c];
				ncount++;
			}
			if(member->spring.m2 == support->constraint.m)
			{
				for(int c = 0; c < 2; c++) ncenter[c] += member->spring.m1->p[c];
				ncount++;
			}
		}
		for(int c = 0; c < 2; c++) ncenter[c] /= ncount;
		if(
			(ncount == 1 && ncenter[0] - support->constraint.m->p[0] < 0) ||
			(ncount == 2 && ncenter[1] - support->constraint.m->p[1] < 0)
		)
		{
			cairo_scale(context, 1.0, -1.0);
		}
		cairo_move_to(context, 0.0, 0.0);
		cairo_line_to(context, 0.035, -0.05);
		cairo_line_to(context, -0.035, -0.05);
		cairo_close_path(context);
		cairo_new_sub_path(context);
		cairo_rectangle(context, -0.075, -0.06, 0.15, 0.005);
		if(count == 1)
		{
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0675, -0.0725, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.03375, -0.0725, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0, -0.0725, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.03375, -0.0725, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.0675, -0.0725, 0.0075, 0.0, tau);
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
		cairo_scale(context, fscale, fscale);
		cairo_arc(context, 0.0, 0.0, 0.02, 0.0, tau);
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
		cairo_move_to(context, member->spring.m1->p[0], member->spring.m1->p[1]);
		cairo_line_to(context, member->spring.m2->p[0], member->spring.m2->p[1]);
		cairo_save(context);
		cairo_scale(context, fscale, fscale);
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
		cairo_scale(context, fscale, fscale);
		cairo_arc(context, 0.0, 0.0, 0.0025, 0, tau);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
		cairo_restore(context);
	}
	cairo_restore(context);
	cairo_restore(context);
	cairo_destroy(context);
	char filename[100];
	sprintf(filename, "out/img/%05d.png", frame + 1);
	cairo_surface_write_to_png(surface, filename);
	cairo_surface_destroy(surface);
	frame++;
}

double timef;
double srate;
int stepf;
double dtime;
double frate;
int framef;

int main(int argc, char **argv)
{
	for(int a = 1; a < argc; a++)
	{
		if(sscanf(argv[a], "gravity=%lf", &gravity) == 1) continue;
		if(sscanf(argv[a], "timef=%lf", &timef) == 1) continue;
		if(sscanf(argv[a], "srate=%lf", &srate) == 1) continue;
		if(sscanf(argv[a], "frate=%lf", &frate) == 1) continue;
		if(sscanf(argv[a], "fsize=<%d %d>", &fsize[0], &fsize[1]) == 2) continue;
		if(sscanf(argv[a], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) == 2) continue;
		if(sscanf(argv[a], "fzoom=%lf", &fzoom) == 1) continue;
		if(sscanf(argv[a], "fscale=%lf", &fscale) == 1) continue;
	}
	if(timef < epsilon) return 1;
	if(srate < epsilon) return 1;
	stepf = ((int) (srate * timef)) - 1;
	if(stepf < 0) return 1;
	dtime = 1.0 / srate;
	if(dtime < epsilon) return 1;
	if(frate < epsilon) return 1;
	framef = ((int) (frate * timef)) - 1;
	if(framef < 0) return 1;
	if(fsize[0] < 64 || fsize[1] < 64) return 1;
	if(fzoom < epsilon) return 1;
	if(fscale < epsilon) return 1;
	scanf("jcount=%d\n", &jcount);
	if(jcount < 0) return 1;
	joints = malloc(jcount * sizeof(struct joint));
	jaccelerations = malloc(jcount * sizeof(double *));
	jforces = malloc(jcount * sizeof(double *));
	for(int j = 0; j < jcount; j++)
	{
		struct joint joint;
		scanf(
			"mass=%lf position=(%lf %lf) velocity=<%lf %lf>\n",
			&joint.mass.m, &joint.mass.p[0], &joint.mass.p[1], &joint.mass.v[0], &joint.mass.v[1]
		);
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
	scanf("mcount=%d\n", &mcount);
	members = malloc(mcount * sizeof(struct member));
	mlengths = malloc(mcount * sizeof(double));
	mdisplacements = malloc(mcount * sizeof(double));
	mvelocities = malloc(mcount * sizeof(double));
	mforces = malloc(mcount * sizeof(double));
	for(int m = 0; m < mcount; m++)
	{
		int jindex1, jindex2;
		struct member member;
		scanf(
			"joint1=%d joint2=%d stiffness=%lf length0=%lf dampening=%lf\n",
			&jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c
		);
		jindex1--; jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount || jindex2 < 0 || jindex2 >= jcount) return 1;
		for(int m2 = 0; m2 < m; m2++)
			if(
				members[m2].spring.m1 == &joints[jindex1].mass &&
				members[m2].spring.m2 == &joints[jindex2].mass
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
	scanf("scount=%d\n", &scount);
	supports = malloc(scount * sizeof(struct support));
	sreactions = malloc(scount * sizeof(double *));
	for(int s = 0; s < scount; s++)
	{
		int jindex;
		char axes[2];
		struct support support;
		scanf(
			"joint=%d axes=%2s position0=(%lf %lf)\n",
			&jindex, axes, &support.constraint.p0[0], &support.constraint.p0[1]
		);
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		for(int s2 = 0; s2 < s; s2++)
			if(supports[s2].constraint.m == &joints[jindex].mass) return 1;
		support.constraint.m = &joints[jindex].mass;
		if(strchr(axes, 'x') != nullptr) support.constraint.a[0] = true;
		else support.constraint.a[0] = false;
		if(strchr(axes, 'y') != nullptr) support.constraint.a[1] = true;
		else support.constraint.a[1] = false;
		if(!support.constraint.a[0] && !support.constraint.a[1]) return 1;
		supports[s] = support;
		sreactions[s] = malloc(2 * sizeof(double));
		for(int c = 0; c < 2; c++) sreactions[s][c] = 0.0;
	}
	scanf("lcount=%d\n", &lcount);
	loads = malloc(lcount * sizeof(struct load));
	for(int l = 0; l < lcount; l++)
	{
		int jindex;
		struct load load;
		scanf(
			"joint=%d force=<%lf %lf>\n",
			&jindex, &load.external.f[0], &load.external.f[1]
		);
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		for(int l2 = 0; l2 < l; l2++)
			if(loads[l2].external.m == &joints[jindex].mass) return 1;
		load.external.m = &joints[jindex].mass;
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
	printf("jcount=%d\n", jcount);
	for(int j = 0; j < jcount; j++)
	{
		printf(
			"force=<%.9lf %.9lf> position=(%.9lf %.9lf) velocity=<%.9lf %.9lf>\n",
			jforces[j][0], jforces[j][1],
			joints[j].mass.p[0], joints[j].mass.p[1],
			joints[j].mass.v[0], joints[j].mass.v[1]
		);
	}
	printf("mcount=%d\n", mcount);
	for(int m = 0; m < mcount; m++)
	{
		printf(
			"force=%.9lf displacement=%.9lf length=%.9lf velocity=%.9lf\n",
			mforces[m], mdisplacements[m], mlengths[m], mvelocities[m]
		);
	}
	printf("scount=%d\n", scount);
	for(int s = 0; s < scount; s++)
	{
		printf("reaction=<%.9lf %.9lf %.9lf>\n", sreactions[s][0], sreactions[s][1], sreactions[s][2]);
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
