#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cairo.h>

#define MSDAXES 2
#include "lib/msd.h"

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

struct frame
{
	struct joint *joints;
	int jcount;

	struct member *members;
	int mcount;

	struct support *supports;
	int scount;

	struct load *loads;
	int lcount;
};

struct frame *frames;
int fcount;
int flimit;

int fsize[2];
double fcenter[2];
double fzoom;
double fscale;

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
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_paint(context);
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
	for(int m = 0; m < frames[0].mcount; m++)
	{
		double color[3];
		cairo_push_group(context);
		if(m % 8 == 0) color[0] = 1.0, color[1] = 0.0, color[2] = 0.0;
		if(m % 8 == 1) color[0] = 1.0, color[1] = 0.5, color[2] = 0.0;
		if(m % 8 == 2) color[0] = 1.0, color[1] = 1.0, color[2] = 0.0;
		if(m % 8 == 3) color[0] = 0.0, color[1] = 1.0, color[2] = 0.0;
		if(m % 8 == 4) color[0] = 0.0, color[1] = 1.0, color[2] = 0.85;
		if(m % 8 == 5) color[0] = 0.0, color[1] = 0.85, color[2] = 1.0;
		if(m % 8 == 6) color[0] = 0.63, color[1] = 0.0, color[2] = 1.0;
		if(m % 8 == 7) color[0] = 1.0, color[1] = 0.0, color[2] = 1.0;
		cairo_new_path(context);
		for(int f = 0; f < fcount - 1; f++)
		{
			struct frame *frame1 = &frames[f], *frame2 = &frames[f + 1];
			struct member *member1 = &frame1->members[m], *member2 = &frame2->members[m];
			cairo_new_sub_path(context);
			cairo_line_to(context, member1->spring.m1->p[0], member1->spring.m1->p[1]);
			cairo_line_to(context, member1->spring.m2->p[0], member1->spring.m2->p[1]);
			cairo_line_to(context, member2->spring.m2->p[0], member2->spring.m2->p[1]);
			cairo_line_to(context, member2->spring.m1->p[0], member2->spring.m1->p[1]);
			cairo_close_path(context);
		}
		cairo_save(context);
		cairo_scale(context, fscale / fzoom, fscale / fzoom);
		cairo_set_line_width(context, 0.01);
		cairo_set_source_rgb(context, color[0], color[1], color[2]);
		cairo_stroke_preserve(context);
		cairo_set_line_width(context, 0.005);
		cairo_set_source_rgb(context, 0.5 * color[0], 0.5 * color[1], 0.5 * color[2]);
		cairo_stroke(context);
		cairo_restore(context);
		for(int f = 0; f < fcount - 1; f++)
		{
			struct frame *frame1 = &frames[f], *frame2 = &frames[f + 1];
			struct member *member1 = &frame1->members[m], *member2 = &frame2->members[m];
			cairo_new_path(context);
			cairo_line_to(context, member1->spring.m1->p[0], member1->spring.m1->p[1]);
			cairo_line_to(context, member1->spring.m2->p[0], member1->spring.m2->p[1]);
			cairo_line_to(context, member2->spring.m2->p[0], member2->spring.m2->p[1]);
			cairo_line_to(context, member2->spring.m1->p[0], member2->spring.m1->p[1]);
			cairo_close_path(context);
			cairo_set_source_rgb(context, 0.5 * color[0], 0.5 * color[1], 0.5 * color[2]);
			cairo_fill(context);
		}
		cairo_pop_group_to_source(context);
		cairo_paint_with_alpha(context, 0.5);
	}
	cairo_restore(context);
	cairo_restore(context);
	cairo_destroy(context);
	if(cairo_surface_write_to_png(surface, filename) != CAIRO_STATUS_SUCCESS)
	{
		fprintf(stderr, "error: create: image file\n");
		return 1;
	}
	cairo_surface_destroy(surface);
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 2 || sscanf(argv[1], "%1000s", filename) != 1)
	{
		fprintf(stderr, "error: parse: filename argument\n");
		fprintf(stderr, "usage: filename argument: string\n");
		return 1;
	}
	char *extension = strrchr(filename, '.');
	if(!extension || strcmp(extension, ".png") != 0)
		strcat(filename, ".png");
	if(argc < 3 || sscanf(argv[2], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2)
	{
		fprintf(stderr, "error: parse: fsize argument\n");
		fprintf(stderr, "usage: fsize argument: fsize=widthxheight\n");
		return 1;
	}
	if(fsize[0] < 64 || fsize[1] < 64)
	{
		fprintf(stderr, "error: limit: fsize argument: %dx%d not larger than %dx%d nor matching\n", fsize[0], fsize[1], 64, 64);
		return 1;
	}
	if(argc < 4 || sscanf(argv[3], "fcenter=(%le %le)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument\n");
		fprintf(stderr, "usage: fcenter argument: fcenter=(float float)\n");
		return 1;
	}
	if(argc < 5 || sscanf(argv[4], "fzoom=%le", &fzoom) != 1)
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
	if(argc < 6 || sscanf(argv[5], "fscale=%le", &fscale) != 1)
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
	fcount = 0;
	flimit = 1;
	frames = malloc(flimit * sizeof(struct frame));
	if(!frames)
	{
		fprintf(stderr, "error: create: frames array: %zd bytes allocation\n", flimit * sizeof(struct frame));
		return 1;
	}
	while(true)
	{
		if(fcount >= flimit)
		{
			struct frame *swap = realloc(frames, 2 * flimit * sizeof(struct frame));
			if(swap == NULL)
			{
				fprintf(stderr, "error: resize: frames array: %zd bytes allocation\n", 2 * flimit * sizeof(struct frame));
				return 1;
			}
			frames = swap;
			flimit *= 2;
		}
		struct frame *frame = &frames[fcount];
		int scan = scanf("joints=%d\n", &frame->jcount);
		if(scan == EOF)
			break;
		fcount++;
		if(scan != 1)
		{
			fprintf(stderr, "error: parse: joints parameter\n");
			fprintf(stderr, "usage: joints parameter: joints=count\n");
			return 1;
		}
		if(frame->jcount < 0)
		{
			fprintf(stderr, "error: count: joints parameter: %d not positive nor %d\n", frame->jcount, 0);
			return 1;
		}
		if(fcount > 0 && frame->jcount != frames[fcount - 1].jcount) return 1;
		frame->joints = malloc(frame->jcount * sizeof(struct joint));
		if(!frame->joints)
		{
			fprintf(stderr, "error: create: joints array: %zd bytes allocation\n", frame->jcount * sizeof(struct joint));
			return 1;
		}
		for(int j = 0; j < frame->jcount; j++)
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
			frame->joints[j] = joint;
		}
		if(scanf("members=%d\n", &frame->mcount) != 1)
		{
			fprintf(stderr, "error: parse: members parameter\n");
			fprintf(stderr, "usage: members parameter: members=count\n");
			return 1;
		}
		if(frame->mcount < 0)
		{
			fprintf(stderr, "error: count: members parameter: %d not positive nor %d\n", frame->mcount, 0);
			return 1;
		}
		if(fcount > 0 && frame->mcount != frames[fcount - 1].mcount) return 1;
		frame->members = malloc(frame->mcount * sizeof(struct member));
		if(!frame->members)
		{
			fprintf(stderr, "error: create: members array: %zd bytes allocation\n", frame->mcount * sizeof(struct member));
			return 1;
		}
		for(int m = 0; m < frame->mcount; m++)
		{
			struct member member;
			int jindex1, jindex2;
			if(scanf("joint1=[%d] joint2=[%d] stiffness=%le length0=%le dampening=%le\n",
			         &jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c) != 5)
			{
				fprintf(stderr, "error: parse: member [%d] line\n", m + 1);
				fprintf(stderr, "usage: member line: joint1=[index] joint2=[index] stiffness=float length0=float dampening=float\n");
				return 1;
			}
			jindex1--, jindex2--;
			if(jindex1 < 0 || jindex1 >= frame->jcount)
			{
				fprintf(stderr, "error: index: member [%d] line: joint1 parameter: [%d] does not exist\n", m + 1, jindex1 + 1);
				return 1;
			}
			if(jindex2 < 0 || jindex2 >= frame->jcount)
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
				if(frame->members[m2].spring.m1 == &frame->joints[jindex1].mass &&
				   frame->members[m2].spring.m2 == &frame->joints[jindex2].mass)
				{
					fprintf(stderr, "error: index: member [%d] line: joint1 and joint2 parameters: [%d] and [%d] already in use\n", m + 1, jindex1 + 1, jindex2 + 1);
					return 1;
				}
				if(frame->members[m2].spring.m1 == &frame->joints[jindex2].mass &&
				   frame->members[m2].spring.m2 == &frame->joints[jindex1].mass)
				{
					fprintf(stderr, "error: index: member [%d] line: joint1 and joint2 parameters: [%d] and [%d] already in use\n", m + 1, jindex2 + 1, jindex1 + 1);
					return 1;
				}
			}
			member.spring.m1 = &frame->joints[jindex1].mass, member.spring.m2 = &frame->joints[jindex2].mass;
			member.damper.m1 = &frame->joints[jindex1].mass, member.damper.m2 = &frame->joints[jindex2].mass;
			if(member.spring.l0 < EPSILON)
			{
				fprintf(stderr, "error: limit: member [%d] line: length0 parameter: %.1le not greater than %.1le\n", m + 1, member.spring.l0, EPSILON);
				return 1;
			}
			frame->members[m] = member;
		}
		if(scanf("supports=%d\n", &frame->scount) != 1)
		{
			fprintf(stderr, "error: parse: supports parameter\n");
			fprintf(stderr, "usage: supports parameter: supports=count\n");
			return 1;
		}
		if(frame->scount < 0)
		{
			fprintf(stderr, "error: count: supports parameter: %d not positive nor %d\n", frame->scount, 0);
			return 1;
		}
		if(fcount > 0 && frame->scount != frames[fcount - 1].scount) return 1;
		frame->supports = malloc(frame->scount * sizeof(struct support));
		if(!frame->supports)
		{
			fprintf(stderr, "error: create: supports array: %zd bytes allocation\n", frame->scount * sizeof(struct support));
			return 1;
		}
		for(int s = 0; s < frame->scount; s++)
		{
			struct support support;
			int jindex;
			char axes[3];
			if(scanf("joint=[%d] axes={%2[^}]}\n", &jindex, axes) != 2)
			{
				fprintf(stderr, "error: parse: support [%d] line\n", s + 1);
				fprintf(stderr, "usage: support line: joint=[index] axes={xy|x|y}\n");
				return 1;
			}
			jindex--;
			if(jindex < 0 || jindex >= frame->jcount)
			{
				fprintf(stderr, "error: index: support [%d] line: joint parameter: [%d] does not exist\n", s + 1, jindex + 1);
				return 1;
			}
			for(int s2 = 0; s2 < s; s2++)
				if(frame->supports[s2].constraint.m == &frame->joints[jindex].mass)
				{
					fprintf(stderr, "error: index: support [%d] line: joint parameter: [%d] already in use\n", s + 1, jindex + 1);
					return 1;
				}
			support.constraint.m = &frame->joints[jindex].mass;
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
				support.constraint.p[a] = frame->joints[jindex].mass.p[a];
			frame->supports[s] = support;
		}
		if(scanf("loads=%d\n", &frame->lcount) != 1)
		{
			fprintf(stderr, "error: parse: loads parameter\n");
			fprintf(stderr, "usage: loads parameter: loads=count\n");
			return 1;
		}
		if(frame->lcount < 0)
		{
			fprintf(stderr, "error: count: loads parameter: %d not positive nor %d\n", frame->lcount, 0);
			return 1;
		}
		if(fcount > 0 && frame->lcount != frames[fcount - 1].lcount) return 1;
		frame->loads = malloc(frame->lcount * sizeof(struct load));
		if(!frame->loads)
		{
			fprintf(stderr, "error: create: loads array: %zd bytes allocation\n", frame->lcount * sizeof(struct load));
			return 1;
		}
		for(int l = 0; l < frame->lcount; l++)
		{
			struct load load;
			int jindex;
			if(scanf("joint=[%d] force=<%le %le>\n",
					 &jindex, &load.action.f[0], &load.action.f[1]) != 3)
			{
				fprintf(stderr, "error: parse: load [%d] line\n", l + 1);
				fprintf(stderr, "usage: load line: joint=[index] force=<float float>\n");
				return 1;
			}
			jindex--;
			if(jindex < 0 || jindex >= frame->jcount)
			{
				fprintf(stderr, "error: index: load [%d] line: joint parameter: [%d] does not exist\n", l + 1, jindex + 1);
				return 1;
			}
			load.action.m = &frame->joints[jindex].mass;
			frame->loads[l] = load;
		}
	}
	if(render() != 0) return 1;
	for(int f = 0; f < fcount; f++)
	{
		free(frames[f].joints);
		free(frames[f].members);
		free(frames[f].supports);
		free(frames[f].loads);
	}
	free(frames);
	return 0;
}
