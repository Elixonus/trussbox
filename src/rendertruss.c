#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cairo.h>
#include "msd.h"

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

struct joint *joints;
int jcount;

struct member *members;
int mcount;

struct support *supports;
int scount;

int fsize[2];
double fcenter[2];
double fzoom;
double fscale;

double epsilon = 1.0e-18;
char filename[1005];

int render(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, fsize[0], fsize[1]);
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
	{
		fprintf(stderr, "error: create: image surface\n");
		return 1;
	}
	cairo_t *context = cairo_create(surface);
	if(cairo_status(context) != CAIRO_STATUS_SUCCESS)
	{
		fprintf(stderr, "error: create: image context\n");
		return 1;
	}
	cairo_save(context);
	cairo_new_path(context);
	cairo_rectangle(context, 0.0, 0.0, (double) fsize[0], (double) fsize[1]);
	cairo_close_path(context);
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
	cairo_set_line_cap(context, CAIRO_LINE_CAP_SQUARE);
	double corner1[2] = {0.0, (double) fsize[1]};
	cairo_device_to_user(context, &corner1[0], &corner1[1]);
	double corner2[2] = {(double) fsize[0], 0.0};
	cairo_device_to_user(context, &corner2[0], &corner2[1]);
	double gstride1 = 0.125 * pow(4.0, round(0.5 * log2(1.0 / fzoom)));
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
	cairo_set_source_rgb(context, 0.1, 0.1, 0.1);
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
	cairo_set_source_rgb(context, 0.15, 0.15, 0.15);
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
	cairo_set_source_rgb(context, 0.22, 0.22, 0.22);
	cairo_stroke(context);
	cairo_restore(context);
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
			cairo_rotate(context, -0.5 * pi);
		cairo_new_path(context);
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
		if(polarity > 0.0)
			cairo_arc(context, 0.0, 0.0, 0.02, 0.0, pi);
		else
			cairo_arc_negative(context, 0.0, 0.0, 0.02, 0.0, pi);
		cairo_scale(context, 1.0, polarity);
		cairo_line_to(context, -0.02, -0.02);
		cairo_line_to(context, 0.02, -0.02);
		cairo_close_path(context);
		cairo_new_sub_path(context);
		cairo_rectangle(context, -0.0625, -0.03, 0.125, 0.005);
		cairo_close_path(context);
		if(count == 1)
		{
			cairo_new_sub_path(context);
			cairo_arc(context, 0.055, -0.0425, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0275, -0.0425, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0, -0.0425, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.0275, -0.0425, 0.0075, 0.0, tau);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.055, -0.0425, 0.0075, 0.0, tau);
			cairo_close_path(context);
		}
		cairo_set_line_width(context, 0.01);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(context);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill(context);
		cairo_new_path(context);
		cairo_arc(context, 0.0, 0.0, 0.0035, 0.0, tau);
		cairo_close_path(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
		cairo_restore(context);
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int s = 0; s < scount; s++)
		{
			struct support *support = &supports[s];
			if(support->constraint.m == &joint->mass)
				goto end;
		}
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
		cairo_new_path(context);
		cairo_arc(context, 0.0, 0.0, 0.0035, 0.0, tau);
		cairo_close_path(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
		cairo_restore(context);
		end:
	}
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		cairo_new_path(context);
		cairo_line_to(context, member->spring.m1->p[0], member->spring.m1->p[1]);
		cairo_line_to(context, member->spring.m2->p[0], member->spring.m2->p[1]);
		cairo_save(context);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_set_line_width(context, 0.0225);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(context);
		cairo_set_line_width(context, 0.0125);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_stroke(context);
		cairo_restore(context);
		cairo_new_path(context);
		cairo_save(context);
		cairo_translate(context, member->spring.m1->p[0], member->spring.m1->p[1]);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_new_sub_path(context);
		cairo_arc(context, 0.0, 0.0, 0.0035, 0.0, tau);
		cairo_close_path(context);
		cairo_restore(context);
		cairo_save(context);
		cairo_translate(context, member->spring.m2->p[0], member->spring.m2->p[1]);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_new_sub_path(context);
		cairo_arc(context, 0.0, 0.0, 0.0035, 0.0, tau);
		cairo_close_path(context);
		cairo_restore(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
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
	if(argc != 6)
	{
		fprintf(stderr, "error: count: arguments: %d of 5 provided\n", argc - 1);
		fprintf(stderr, "usage: arguments: filename fsize=widthxheight fcenter=(float float) fzoom=float fscale=float\n");
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
	if(sscanf(argv[2], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
	{
		fprintf(stderr, "error: parse: fsize argument [2]\n");
		fprintf(stderr, "usage: fsize argument [2]: fsize=widthxheight\n");
		return 1;
	}
	if(fsize[0] < 64 || fsize[1] < 64)
	{
		fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than 64x64 nor matching\n", fsize[0], fsize[1]);
		return 1;
	}
	if(sscanf(argv[3], "fcenter=(%lf %lf)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument [3]\n");
		fprintf(stderr, "usage: fcenter argument [3]: fcenter=(float float)\n");
		return 1;
	}
	if(sscanf(argv[4], "fzoom=%lf", &fzoom) != 1)
	{
		fprintf(stderr, "error: parse: fzoom argument [4]\n");
		fprintf(stderr, "usage: fzoom argument [4]: fzoom=float\n");
		return 1;
	}
	if(fzoom < epsilon)
	{
		fprintf(stderr, "error: limit: fzoom argument: %.1e not greater than %.1e\n", fzoom, epsilon);
		return 1;
	}
	if(sscanf(argv[5], "fscale=%lf", &fscale) != 1)
	{
		fprintf(stderr, "error: parse: fscale argument [5]\n");
		fprintf(stderr, "usage: fscale argument [5]: fscale=float\n");
		return 1;
	}
	if(fscale < epsilon)
	{
		fprintf(stderr, "error: limit: fscale argument: %.1e not greater than %.1e\n", fscale, epsilon);
		return 1;
	}
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
			fprintf(stderr, "error: parse: joint line [%d]\n", j + 1);
			fprintf(stderr, "usage: joint line: mass=float position=(float float) velocity=(float float)\n");
			return 1;
		}
		if(joint.mass.m < epsilon)
		{
			fprintf(stderr, "error: limit: joint line [%d]: mass parameter: %.1e not greater than %.1e\n", j + 1, joint.mass.m, epsilon);
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
		if(scanf("joint1=[%d] joint2=[%d] stiffness=%lf length0=%lf dampening=%lf\n",
		         &jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c) != 5)
		{
			fprintf(stderr, "error: parse: member line [%d]\n", m + 1);
			fprintf(stderr, "usage: member line: joint1=[index] joint2=[index] stiffness=float length0=float dampening=float\n");
			return 1;
		}
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount)
		{
			fprintf(stderr, "error: index: member line [%d]: joint1 parameter: [%d] does not exist\n", m + 1, jindex1 + 1);
			return 1;
		}
		if(jindex2 < 0 || jindex2 >= jcount)
		{
			fprintf(stderr, "error: index: member line [%d]: joint2 parameter: [%d] does not exist\n", m + 1, jindex2 + 1);
			return 1;
		}
		for(int m2 = 0; m2 < m; m2++)
			if((members[m2].spring.m1 == &joints[jindex1].mass &&
			    members[m2].spring.m2 == &joints[jindex2].mass) ||
			   (members[m2].spring.m1 == &joints[jindex2].mass &&
			    members[m2].spring.m2 == &joints[jindex1].mass))
			{
				fprintf(stderr, "error: index: member line [%d]: joint1 and joint2 parameters: ([%d] and [%d]) or ([%d] and [%d]) already in use\n", m + 1, jindex1 + 1, jindex2 + 1, jindex2 + 1, jindex1 + 1);
				return 1;
			}
		member.spring.m1 = &joints[jindex1].mass, member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass, member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < epsilon)
		{
			fprintf(stderr, "error: limit: member line [%d]: length0 parameter: %.1e not greater than %.1e\n", m + 1, member.spring.l0, epsilon);
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
		if(scanf("joint=[%d] axes=%100s\n", &jindex, axes) != 2)
		{
			fprintf(stderr, "error: parse: support line [%d]\n", s + 1);
			fprintf(stderr, "usage: support line: joint=[index] axes=xy|x|y\n");
			return 1;
		}
		jindex--;
		if(jindex < 0 || jindex >= jcount)
		{
			fprintf(stderr, "error: index: support line [%d]: joint parameter: [%d] does not exist\n", s + 1, jindex + 1);
			return 1;
		}
		for(int s2 = 0; s2 < s; s2++)
			if(supports[s2].constraint.m == &joints[jindex].mass)
			{
				fprintf(stderr, "error: index: support line [%d]: joint parameter: [%d] already in use\n", s + 1, jindex + 1);
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
			fprintf(stderr, "error: parse: support line [%d]: axes parameter: not an option\n", s + 1);
			fprintf(stderr, "usage: support line: axes parameter: axes=xy|x|y\n");
			return 1;
		}
		for(int a = 0; a < 2; a++)
			support.constraint.p[a] = joints[jindex].mass.p[a];
		supports[s] = support;
	}
	if(render() != 0) return 1;
	free(joints);
	free(members);
	free(supports);
	return 0;
}
