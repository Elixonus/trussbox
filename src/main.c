#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include "dampspring.h"

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#ifndef M_TAU
#define M_TAU 6.283185307
#endif

struct joint {
	struct mass mass;
};

struct member {
	struct spring spring;
	struct damper damper;
};

struct support {
	struct mass *mass;
	struct constraint {
		double n[3];
		double p0[3];
	} constraints[3];
};

#define jcount 2
struct joint joints[jcount] = {
	{
		.mass = {
			.m = 1.0,
			.p = {0.0, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
	{
		.mass = {
			.m = 1.0,
			.p = {1.0, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	}
};
double jforces[jcount][3];
double jaccelerations[jcount][3];

#define mcount 1
struct member members[mcount] = {
	{
		.spring = {
			.k = 1.0,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass,
			.l0 = 0.75
		},
		.damper = {
			.c = 1.0,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass
		}
	}
};

#define scount 0
struct support supports[scount] = {};

int iteration;
double delta_time = 0.01;

void step(void)
{
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int c = 0; c < 3; c++)
		{
			joint->mass.v[c] += 0.5 * jaccelerations[j][c] * delta_time;
			joint->mass.p[c] += joint->mass.v[c] * delta_time;
		}
	}

	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 3; c++)
		{
			jforces[j][c] = 0.0;
			jaccelerations[j][c] = 0.0;
		}
	}

	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		double mforce = sforce(&member->spring) + dforce(&member->damper);
		double mlength = mdistance(member->spring.m1, member->spring.m2);
		double mdirection[3];
		int j1, j2;
		for(int j = 0; j < jcount; j++)
		{
			if(&joints[j].mass == member->spring.m1)
			{
				j1 = j;
			}
			if(&joints[j].mass == member->spring.m2)
			{
				j2 = j;
			}
		}
		for(int c = 0; c < 3; c++)
		{
			mdirection[c] = (joints[j2].mass.p[c] - joints[j1].mass.p[c]) / mlength;
			jforces[j1][c] -= mforce * mdirection[c];
			jforces[j2][c] += mforce * mdirection[c];
		}
	}

	for(int j = 0; j < jcount; j++)
	{
		double jacceleration[3];
		for(int c = 0; c < 3; c++)
		{
			jacceleration[c] = jforces[j][c] / joints[j].mass.m;
			jaccelerations[j][c] = jacceleration[c];
		}
	}

	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		for(int c = 0; c < 3; c++)
		{
			joint->mass.v[c] += 0.5 * jaccelerations[j][c] * delta_time;
		}
	}
}

int picture;
int width = 1000, height = 1000;
double zoom = 0.8;
cairo_surface_t *surface;
cairo_t *context;

void draw(void)
{
	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
	context = cairo_create(surface);

	cairo_save(context);
	cairo_translate(context, 0.0, 0.5 * ((double) height));
	cairo_scale(context, 1.0, -1.0);
	cairo_translate(context, 0.0, -0.5 * ((double) height));

	cairo_rectangle(context, 0.0, 0.0, (double) width, (double) height);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_fill(context);

	cairo_save(context);
	cairo_translate(context, 0.5 * ((double) width), 0.5 * ((double) height));
	cairo_scale(context, (double) width, (double) height);
	cairo_scale(context, zoom, zoom);
	cairo_translate(context, -0.5, -0.5);

	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		cairo_arc(context, joint->mass.p[0], joint->mass.p[1], 0.03, 0, M_TAU);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill_preserve(context);
		cairo_set_line_width(context, 0.003);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke(context);
	}





	cairo_restore(context);
	cairo_restore(context);

	cairo_destroy(context);
	char filename[99];
	sprintf(filename, "out/img/%05d.png", picture);
	cairo_surface_write_to_png(surface, filename);
	cairo_surface_destroy(surface);
	picture++;
}

void init(void)
{
	iteration = 0;
	picture = 0;

	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 3; c++)
		{
			jforces[j][c] = 0.0;
			jaccelerations[j][c] = 0.0;
		}
	}
}

int iterations = 1000;
int pictures = 100;

int main(void)
{
	printf("main program started.\n");
	init();
	int substeps = iterations / pictures;
	for(int p = 0; p < pictures; p++)
	{
		draw();
		for(int s = 0; s < substeps; s++)
		{
			step();
		}
	}
	return EXIT_SUCCESS;
}
