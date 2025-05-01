#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cairo.h>
#include "msd.h"

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
		double p[2];
	} constraint;
};

struct load {
	struct action {
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

double gacceleration;

int fsize[2];
double fcenter[2];
double fzoom;
double fscale;

double epsilon = 1.0e-18;
char filename[1005];

void map_mforce_to_color(double force, double *color, double cutoff_force)
{
	if(cutoff_force < epsilon) return;
	double balance = force / cutoff_force;
	balance = balance < 1.0 ? (balance > -1.0 ? balance : -1.0) : 1.0;
	if(fabs(balance) < 0.1)
		color[0] = 0.9, color[1] = 0.9, color[2] = 0.9;
	else if(balance > 0.0)
		color[0] = 1.0, color[1] = 1.0 - balance, color[2] = 1.0 - balance;
	else
		color[0] = 1.0 - fabs(balance), color[1] = 1.0 - fabs(balance), color[2] = 1.0;
}

void render_force(
	cairo_t *context, double *force, double *point, double ref_force,
	double color[3], bool draw_head_at_point
)
{
	if(ref_force < epsilon) return;
	double magnitude = 0.0;
	for(int a = 0; a < 2; a++)
		magnitude += pow(force[a], 2.0);
	magnitude = sqrt(magnitude);
	if(magnitude < 0.05 * ref_force) return;
	cairo_save(context);
	cairo_translate(context, point[0], point[1]);
	cairo_rotate(context, atan2(force[1], force[0]));
	cairo_scale(context, fscale / fzoom, fscale / fzoom);
	if(draw_head_at_point == true)
		cairo_translate(context, -0.07 * magnitude / ref_force - 0.01, 0.0);
	cairo_new_path(context);
	cairo_line_to(context, 0.0, 0.0);
	cairo_translate(context, 0.07 * magnitude / ref_force + 0.005, 0.0);
	cairo_line_to(context, -0.01, 0.0);
	cairo_line_to(context, -0.01, 0.005);
	cairo_line_to(context, 0.0, 0.0);
	cairo_line_to(context, -0.01, -0.005);
	cairo_line_to(context, -0.01, 0.0);
	cairo_close_path(context);
	cairo_set_line_width(context, 0.008);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_stroke_preserve(context);
	cairo_set_line_width(context, 0.005);
	cairo_set_source_rgb(context, color[0], color[1], color[2]);
	cairo_stroke_preserve(context);
	cairo_set_source_rgb(context, color[0], color[1], color[2]);
	cairo_fill(context);
	cairo_restore(context);
}

int render(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, fsize[0], fsize[1]);
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_t *context = cairo_create(surface);
	if(cairo_status(context) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_save(context);
	cairo_new_path(context);
	cairo_rectangle(context, 0.0, 0.0, (double) fsize[0], (double) fsize[1]);
	cairo_clip(context);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
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
	cairo_set_line_cap(context, CAIRO_LINE_CAP_SQUARE);
	double corner1[2] = {0.0, (double) fsize[1]};
	cairo_device_to_user(context, &corner1[0], &corner1[1]);
	double corner2[2] = {(double) fsize[0], 0.0};
	cairo_device_to_user(context, &corner2[0], &corner2[1]);
	double gstride1 = 0.1 * pow(4.0, round(0.5 * log2(1.0 / fzoom)));
	double gstride2 = 0.5 * gstride1;
	double gstride3 = 0.25 * gstride1;
	double gpoint1[2];
	double gpoint2[2];
	cairo_new_path(context);
	gpoint1[1] = corner1[1], gpoint2[1] = corner2[1];
	for(gpoint1[0] = gstride3 * floor((corner1[0] - 0.001) / gstride3); gpoint1[0] <= corner2[0] + 0.001; gpoint1[0] += gstride3)
	{
		gpoint2[0] = gpoint1[0];
		cairo_new_sub_path(context);
		cairo_line_to(context, gpoint1[0], gpoint1[1]);
		cairo_line_to(context, gpoint2[0], gpoint2[1]);
	}
	gpoint1[0] = corner1[0], gpoint2[0] = corner2[0];
	for(gpoint1[1] = gstride3 * floor((corner1[1] - 0.001) / gstride3); gpoint1[1] <= corner2[1] + 0.001; gpoint1[1] += gstride3)
	{
		gpoint2[1] = gpoint1[1];
		cairo_new_sub_path(context);
		cairo_line_to(context, gpoint1[0], gpoint1[1]);
		cairo_line_to(context, gpoint2[0], gpoint2[1]);
	}
	cairo_save(context);
	cairo_scale(context, 1.0 / fzoom, 1.0 / fzoom);
	cairo_set_line_width(context, 0.002);
	cairo_set_source_rgb(context, 0.98, 0.98, 0.98);
	cairo_stroke(context);
	cairo_restore(context);
	cairo_new_path(context);
	gpoint1[1] = corner1[1], gpoint2[1] = corner2[1];
	for(gpoint1[0] = gstride2 * floor((corner1[0] - 0.001) / gstride2); gpoint1[0] <= corner2[0] + 0.001; gpoint1[0] += gstride2)
	{
		gpoint2[0] = gpoint1[0];
		cairo_new_sub_path(context);
		cairo_line_to(context, gpoint1[0], gpoint1[1]);
		cairo_line_to(context, gpoint2[0], gpoint2[1]);
	}
	gpoint1[0] = corner1[0], gpoint2[0] = corner2[0];
	for(gpoint1[1] = gstride2 * floor((corner1[1] - 0.001) / gstride2); gpoint1[1] <= corner2[1] + 0.001; gpoint1[1] += gstride2)
	{
		gpoint2[1] = gpoint1[1];
		cairo_new_sub_path(context);
		cairo_line_to(context, gpoint1[0], gpoint1[1]);
		cairo_line_to(context, gpoint2[0], gpoint2[1]);
	}
	cairo_save(context);
	cairo_scale(context, 1.0 / fzoom, 1.0 / fzoom);
	cairo_set_line_width(context, 0.002);
	cairo_set_source_rgb(context, 0.95, 0.95, 0.95);
	cairo_stroke(context);
	cairo_restore(context);
	cairo_new_path(context);
	gpoint1[1] = corner1[1], gpoint2[1] = corner2[1];
	for(gpoint1[0] = gstride1 * floor((corner1[0] - 0.001) / gstride1); gpoint1[0] <= corner2[0] + 0.001; gpoint1[0] += gstride1)
	{
		gpoint2[0] = gpoint1[0];
		cairo_new_sub_path(context);
		cairo_line_to(context, gpoint1[0], gpoint1[1]);
		cairo_line_to(context, gpoint2[0], gpoint2[1]);
	}
	gpoint1[0] = corner1[0], gpoint2[0] = corner2[0];
	for(gpoint1[1] = gstride1 * floor((corner1[1] - 0.001) / gstride1); gpoint1[1] <= corner2[1] + 0.001; gpoint1[1] += gstride1)
	{
		gpoint2[1] = gpoint1[1];
		cairo_new_sub_path(context);
		cairo_line_to(context, gpoint1[0], gpoint1[1]);
		cairo_line_to(context, gpoint2[0], gpoint2[1]);
	}
	cairo_save(context);
	cairo_scale(context, 1.0 / fzoom, 1.0 / fzoom);
	cairo_set_line_width(context, 0.002);
	cairo_set_source_rgb(context, 0.9, 0.9, 0.9);
	cairo_stroke(context);
	cairo_restore(context);
	double ref_force = 0.0;
	for(int j = 0; j < jcount; j++)
	{
		double force = fabs(gacceleration * joints[j].mass.m);
		if(force > ref_force)
			ref_force = force;
	}
	for(int l = 0; l < lcount; l++)
	{
		double force = sqrt(pow(loads[l].action.f[0], 2.0) + pow(loads[l].action.f[1], 2.0));
		if(force > ref_force)
			ref_force = force;
	}
	for(int s = 0; s < scount; s++)
	{
		double force = sqrt(pow(sreactions[s][0], 2.0) + pow(sreactions[s][1], 2.0));
		if(force > ref_force)
			ref_force = force;
	}
	for(int m = 0; m < mcount; m++)
	{
		double force = fabs(mforces[m]);
		if(force > ref_force)
			ref_force = force;
	}
	cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_ROUND);
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		cairo_new_path(context);
		cairo_line_to(context, member->spring.m1->p[0], member->spring.m1->p[1]);
		cairo_line_to(context, member->spring.m2->p[0], member->spring.m2->p[1]);
		cairo_save(context);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_set_line_width(context, 0.02);
		double color[3] = {1.0, 1.0, 1.0};
		cairo_set_source_rgb(context, color[0], color[1], color[2]);
		cairo_stroke(context);
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
		cairo_set_line_width(context, 0.01);
		double color[3];
		map_mforce_to_color(mforces[m], color, ref_force);
		cairo_set_source_rgb(context, color[0], color[1], color[2]);
		cairo_stroke(context);
		cairo_restore(context);
	}
	cairo_set_line_cap(context, CAIRO_LINE_CAP_BUTT);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_MITER);
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		if(mlengths[m] < epsilon) continue;
		double direction[2];
		double forces[2][2];
		for(int a = 0; a < 2; a++)
		{
			direction[a] = (member->spring.m2->p[a] - member->spring.m1->p[a]) / mlengths[m];
			forces[0][a] = -mforces[m] * direction[a];
			forces[1][a] = mforces[m] * direction[a];
		}
		double color[3] = {0.0, 0.0, 0.0};
		render_force(context, forces[0], member->spring.m1->p, ref_force, color, false);
		render_force(context, forces[1], member->spring.m2->p, ref_force, color, false);
	}
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		for(int a1 = 0; a1 < 2; a1++)
		{
			double force[2] = {0.0, 0.0};
			for(int a2 = 0; a2 < 2; a2++)
				if(a2 == a1)
					force[a2] = sreactions[s][a2];
			double color[3] = {0.0, 0.0, 0.0};
			render_force(context, force, support->constraint.m->p, ref_force, color, true);
		}
	}
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		double color[3] = {0.0, 0.0, 0.0};
		render_force(context, load->action.f, load->action.m->p, ref_force, color, false);
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		double force[2] = {0.0, gacceleration * joint->mass.m};
		double color[3] = {0.0, 0.0, 0.0};
		render_force(context, force, joint->mass.p, ref_force, color, true);
	}
	cairo_restore(context);
	cairo_restore(context);
	cairo_destroy(context);
	if(cairo_surface_write_to_png(surface, filename) != CAIRO_STATUS_SUCCESS)
	{
		fprintf(stderr, "error: create: image file: %s\n", filename);
		return 1;
	}
	cairo_surface_destroy(surface);
	return 0;
}

int main(int argc, char **argv)
{
	if(argc != 7)
	{
		fprintf(stderr, "error: count: arguments: %d of 6 provided\n", argc - 1);
		fprintf(stderr, "usage: arguments: filename gacceleration=float fsize=widthxheight fcenter=(float float) fzoom=float fscale=float\n");
		return 1;
	}
	if(sscanf(argv[1], "%1000s", filename) != 1)
	{
		fprintf(stderr, "error: parse: filename argument [1]\n");
		fprintf(stderr, "usage: filename argument [1]: string\n");
		return 1;
	}
	char *extension = strrchr(filename, '.');
	if(!extension || strcmp(extension, ".png") != 0)
		strcat(filename, ".png");
	if(sscanf(argv[2], "gacceleration=%lf", &gacceleration) != 1)
	{
		fprintf(stderr, "error: parse: gacceleration argument [2]\n");
		fprintf(stderr, "usage: gacceleration argument [2]: gacceleration=float\n");
		return 1;
	}
	if(sscanf(argv[3], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
	{
		fprintf(stderr, "error: parse: fsize argument [3]\n");
		fprintf(stderr, "usage: fsize argument [3]: fsize=widthxheight\n");
		return 1;
	}
	if(fsize[0] < 64 || fsize[1] < 64)
	{
		fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than 64x64 nor matching\n", fsize[0], fsize[1]);
		return 1;
	}
	if(sscanf(argv[4], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument [4]\n");
		fprintf(stderr, "usage: fcenter argument [4]: fcenter=(float float)\n");
		return 1;
	}
	if(sscanf(argv[5], "fzoom=%lf", &fzoom) != 1)
	{
		fprintf(stderr, "error: parse: fzoom argument [5]\n");
		fprintf(stderr, "usage: fzoom argument [5]: fzoom=float\n");
		return 1;
	}
	if(fzoom < epsilon)
	{
		fprintf(stderr, "error: limit: fzoom argument: %.1e not greater than %.1e\n", fzoom, epsilon);
		return 1;
	}
	if(sscanf(argv[6], "fscale=%lf", &fscale) != 1)
	{
		fprintf(stderr, "error: parse: fscale argument [6]\n");
		fprintf(stderr, "usage: fscale argument [6]: fscale=float\n");
		return 1;
	}
	if(fscale < epsilon)
	{
		fprintf(stderr, "error: limit: fscale argument: %.1e not greater than %.1e\n", fscale, epsilon);
		return 1;
	}
	if(scanf("joints=%d\n", &jcount) != 1) return 1;
	if(jcount < 0) return 1;
	joints = malloc(jcount * sizeof(struct joint));
	if(!joints) return 1;
	for(int j = 0; j < jcount; j++)
	{
		struct joint joint;
		if(scanf("mass=%lf position=(%lf %lf) velocity=<%lf %lf>\n",
		         &joint.mass.m, &joint.mass.p[0], &joint.mass.p[1], &joint.mass.v[0], &joint.mass.v[1]) != 5) return 1;
		if(joint.mass.m < epsilon) return 1;
		joints[j] = joint;
	}
	if(scanf("members=%d\n", &mcount) != 1) return 1;
	members = malloc(mcount * sizeof(struct member));
	if(!members) return 1;
	for(int m = 0; m < mcount; m++)
	{
		int jindex1, jindex2;
		struct member member;
		if(scanf("joint1=[%d] joint2=[%d] stiffness=%lf length0=%lf dampening=%lf\n",
		         &jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c) != 5) return 1;
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount || jindex2 < 0 || jindex2 >= jcount) return 1;
		for(int m2 = 0; m2 < m; m2++)
			if((members[m2].spring.m1 == &joints[jindex1].mass &&
			    members[m2].spring.m2 == &joints[jindex2].mass) ||
			   (members[m2].spring.m1 == &joints[jindex2].mass &&
			    members[m2].spring.m2 == &joints[jindex1].mass)) return 1;
		member.spring.m1 = &joints[jindex1].mass, member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass, member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < epsilon) return 1;
		members[m] = member;
	}
	if(scanf("supports=%d\n", &scount) != 1) return 1;
	supports = malloc(scount * sizeof(struct support));
	if(!supports) return 1;
	for(int s = 0; s < scount; s++)
	{
		int jindex;
		char axes[3];
		struct support support;
		if(scanf("joint=[%d] axes=%2s\n", &jindex, axes) != 2) return 1;
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		for(int s2 = 0; s2 < s; s2++) if(supports[s2].constraint.m == &joints[jindex].mass) return 1;
		support.constraint.m = &joints[jindex].mass;
		if(strcmp(axes, "xy") == 0 || strcmp(axes, "yx") == 0)
			support.constraint.a[0] = true, support.constraint.a[1] = true;
		else if(strcmp(axes, "x") == 0)
			support.constraint.a[0] = true, support.constraint.a[1] = false;
		else if(strcmp(axes, "y") == 0)
			support.constraint.a[0] = false, support.constraint.a[1] = true;
		else return 1;
		for(int a = 0; a < 2; a++)
			support.constraint.p[a] = joints[jindex].mass.p[a];
		supports[s] = support;
	}
	if(scanf("loads=%d\n", &lcount) != 1) return 1;
	loads = malloc(lcount * sizeof(struct load));
	if(!loads) return 1;
	for(int l = 0; l < lcount; l++)
	{
		int jindex;
		struct load load;
		if(scanf("joint=[%d] force=<%lf %lf>\n",
		         &jindex, &load.action.f[0], &load.action.f[1]) != 3) return 1;
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		load.action.m = &joints[jindex].mass;
		loads[l] = load;
	}
	int jcount2;
	if(scanf("joints=%d\n", &jcount2) != 1) return 1;
	if(jcount2 != jcount) return 1;
	jforces = malloc(jcount * sizeof(double *));
	if(!jforces) return 1;
	for(int j = 0; j < jcount; j++)
	{
		jforces[j] = malloc(2 * sizeof(double));
		if(!jforces[j]) return 1;
		if(scanf("force=<%lf %lf> position=(%*f %*f) velocity=<%*f %*f>\n",
		         &jforces[j][0], &jforces[j][1]) != 2) return 1;
	}
	int mcount2;
	if(scanf("members=%d\n", &mcount2) != 1) return 1;
	if(mcount2 != mcount) return 1;
	mlengths = malloc(mcount * sizeof(double));
	if(!mlengths) return 1;
	mdisplacements = malloc(mcount * sizeof(double));
	if(!mdisplacements) return 1;
	mvelocities = malloc(mcount * sizeof(double));
	if(!mvelocities) return 1;
	mforces = malloc(mcount * sizeof(double));
	if(!mforces) return 1;
	for(int m = 0; m < mcount; m++)
	{
		if(scanf("force=%lf displacement=%lf length=%lf velocity=%lf\n",
		         &mforces[m], &mdisplacements[m], &mlengths[m], &mvelocities[m]) != 4) return 1;
	}
	int scount2;
	if(scanf("supports=%d\n", &scount2) != 1) return 1;
	sreactions = malloc(scount * sizeof(double *));
	if(!sreactions) return 1;
	for(int s = 0; s < scount; s++)
	{
		sreactions[s] = malloc(2 * sizeof(double));
		if(!sreactions[s]) return 1;
		if(scanf("reaction=<%lf %lf>\n", &sreactions[s][0], &sreactions[s][1]) != 2) return 1;
	}
	if(render() != 0) return 1;
	free(joints);
	for(int j = 0; j < jcount; j++)
		free(jforces[j]);
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
