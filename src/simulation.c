#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include "dampspring.h"

#ifndef M_PI
#define M_PI 3.141592654
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
		int c;
		double p0[3];
		union {
			double t[3];
			double n[3];
		};
	} constraint;
};

#define jcount 9
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
			.p = {0.25, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
	{
		.mass = {
			.m = 1.0,
			.p = {0.5, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.75, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {1.0, 0.0, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.125, 0.25, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.375, 0.25, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.625, 0.25, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.875, 0.25, 0.0},
			.v = {0.0, 0.0, 0.0}
		}
	}
};
double jforces[jcount][3];
double jaccelerations[jcount][3];

#define mcount 15
struct member members[mcount] = {
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[1].mass,
			.m2 = &joints[2].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[1].mass,
			.m2 = &joints[2].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[2].mass,
			.m2 = &joints[3].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[2].mass,
			.m2 = &joints[3].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[3].mass,
			.m2 = &joints[4].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[3].mass,
			.m2 = &joints[4].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[5].mass,
			.m2 = &joints[6].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[5].mass,
			.m2 = &joints[6].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[6].mass,
			.m2 = &joints[7].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[6].mass,
			.m2 = &joints[7].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[7].mass,
			.m2 = &joints[8].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[7].mass,
			.m2 = &joints[8].mass
		}
	},
	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[0].mass,
			.m2 = &joints[5].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[0].mass,
			.m2 = &joints[5].mass
		}
	},
 	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[1].mass,
			.m2 = &joints[6].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[1].mass,
			.m2 = &joints[6].mass
		}
	},
  	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[2].mass,
			.m2 = &joints[7].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[2].mass,
			.m2 = &joints[7].mass
		}
	},
   	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[3].mass,
			.m2 = &joints[8].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[3].mass,
			.m2 = &joints[8].mass
		}
	},
   	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[1].mass,
			.m2 = &joints[5].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[1].mass,
			.m2 = &joints[5].mass
		}
	},
   	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[2].mass,
			.m2 = &joints[6].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[2].mass,
			.m2 = &joints[6].mass
		}
	},
   	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[3].mass,
			.m2 = &joints[7].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[3].mass,
			.m2 = &joints[7].mass
		}
	},
   	{
		.spring = {
			.k = 5e0,
			.m1 = &joints[4].mass,
			.m2 = &joints[8].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 0.1,
			.m1 = &joints[4].mass,
			.m2 = &joints[8].mass
		}
	}
};

#define scount 2
struct support supports[scount] = {
	{
		.mass = &joints[0].mass,
		.constraint = {
			.c = 3,
			.p0 = {0.0, 0.0, 0.0}
		}
	},
	{
		.mass = &joints[4].mass,
		.constraint = {
			.c = 2,
			.p0 = {1.0, 0.0, 0.0},
			.t = {1.0, -1.0, 0.0}
		}
	}
};

int iteration = 0;
double time = 0.0;
double delta_time = 0.001;
double gravity = 0.01;
double epsilon = 1e-9;

void step(void)
{
	// supports[1].constraint.n[0] = cos(1e-4 * iteration);
	// supports[1].constraint.n[1] = sin(1e-4 * iteration);

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
	for(int j = 0; j < jcount; j++)
	{
		jaccelerations[j][1] = -gravity;
		jforces[j][1] = joints[j].mass.m * jaccelerations[j][1];
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
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		if(support->constraint.c == 0)
		{
			continue;
		}
		else if(support->constraint.c == 1)
		{
			double product1 = 0.0;
			double product2 = 0.0;
			double product3 = 0.0;
			for(int c = 0; c < 3; c++)
			{
				product1 += (support->mass->p[c] - support->constraint.p0[c]) * support->constraint.n[c];
				product2 += support->mass->v[c] * support->constraint.n[c];
				product3 += support->constraint.n[c] * support->constraint.n[c];
			}
			if(fabs(product3) < fabs(epsilon))
			{
				fprintf(stderr, "error: plane constraint's normal vector square magnitude is below epsilon.\n");
				exit(EXIT_FAILURE);
			}
			double coefficient1 = product1 / product3;
			double coefficient2 = product2 / product3;
			for(int c = 0; c < 3; c++)
			{
				support->mass->p[c] = support->mass->p[c] - coefficient1 * support->constraint.n[c];
				support->mass->v[c] = support->mass->v[c] - coefficient2 * support->constraint.n[c];
			}
		}
		else if(support->constraint.c == 2)
		{
			double product1 = 0.0;
			double product2 = 0.0;
			double product3 = 0.0;
			for(int c = 0; c < 3; c++)
			{
				product1 += (support->mass->p[c] - support->constraint.p0[c]) * support->constraint.t[c];
				product2 += support->mass->v[c] * support->constraint.t[c];
				product3 += support->constraint.t[c] * support->constraint.t[c];
			}
			if(fabs(product3) < fabs(epsilon))
			{
				fprintf(stderr, "error: line constraint's tangent vector square magnitude is below epsilon.\n");
				exit(EXIT_FAILURE);
			}
			double coefficient1 = product1 / product3;
			double coefficient2 = product2 / product3;
			for(int c = 0; c < 3; c++)
			{
				support->mass->p[c] = support->constraint.p0[c] + coefficient1 * support->constraint.t[c];
				support->mass->v[c] = coefficient2 * support->constraint.t[c];
			}
		}
		else if(support->constraint.c == 3)
		{
			for(int c = 0; c < 3; c++)
			{
				support->mass->p[c] = support->constraint.p0[c];
				support->mass->v[c] = 0.0;
			}
		}
		else
		{
			fprintf(stderr, "error: line constraint's count not in range.\n");
			exit(EXIT_FAILURE);
		}
	}
	iteration++;
	time += delta_time;
}

int picture = 0;
int width = 1920, height = 1080;
double center[2] = {0.5, 0.125};
double zoom = 1.0;
double scale = 1.0;

void draw(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
	cairo_t *context = cairo_create(surface);
	cairo_save(context);
	cairo_translate(context, 0.0, 0.5 * ((double) height));
	cairo_scale(context, 1.0, -1.0);
	cairo_translate(context, 0.0, -0.5 * ((double) height));
	cairo_save(context);
	cairo_translate(context, 0.5 * ((double) width), 0.5 * ((double) height));
	cairo_save(context);
	cairo_scale(context, (double) width, (double) height);
	cairo_pattern_t *background = cairo_pattern_create_radial(0.0, 0.0, 0.0, 0.0, 0.0, 0.5);
	cairo_pattern_add_color_stop_rgb(background, 0.0, 0.047, 0.357, 0.71);
	cairo_pattern_add_color_stop_rgb(background, 1.0, 0.0, 0.0, 0.0);
	cairo_set_source(context, background);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_paint(context);
	cairo_restore(context);
	double length = width < height ? (double) width : (double) height;
	cairo_scale(context, length, length);
	cairo_scale(context, zoom, zoom);
	cairo_translate(context, -center[0], -center[1]);
	cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_ROUND);
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		cairo_save(context);
		cairo_translate(context, support->mass->p[0], support->mass->p[1]);
		cairo_scale(context, scale, scale);
		if(support->constraint.c == 0)
		{
			continue;
		}
		else if(support->constraint.c == 1)
		{
			double angle = atan2(support->constraint.n[1], support->constraint.n[0]);
			angle = angle > 0.0 ? angle - 0.5 * M_PI : angle + 0.5 * M_PI;
			cairo_rotate(context, angle);
		}
		else if(support->constraint.c == 2)
		{
			double angle = atan2(-support->constraint.t[0], support->constraint.t[1]);
			angle = angle > 0 ? angle - 0.5 * M_PI : angle + 0.5 * M_PI;
			cairo_rotate(context, angle);
		}
		cairo_move_to(context, 0.0, 0.0);
		cairo_line_to(context, 0.035, -0.05);
		cairo_line_to(context, -0.035, -0.05);
		cairo_close_path(context);
		cairo_new_sub_path(context);
		cairo_rectangle(context, -0.075, -0.06, 0.15, 0.005);
		if(support->constraint.c == 1 || support->constraint.c == 2)
		{
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0675, -0.0725, 0.0075, 0.0, M_TAU);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.03375, -0.0725, 0.0075, 0.0, M_TAU);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, 0.0, -0.0725, 0.0075, 0.0, M_TAU);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.03375, -0.0725, 0.0075, 0.0, M_TAU);
			cairo_close_path(context);
			cairo_new_sub_path(context);
			cairo_arc(context, -0.0675, -0.0725, 0.0075, 0.0, M_TAU);
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
		cairo_scale(context, scale, scale);
		cairo_arc(context, 0.0, 0.0, 0.02, 0.0, M_TAU);
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
		cairo_scale(context, scale, scale);
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
		cairo_scale(context, scale, scale);
		cairo_arc(context, 0.0, 0.0, 0.0025, 0, M_TAU);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
		cairo_restore(context);
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
	time = 0.0;
	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 3; c++)
		{
			jforces[j][c] = 0.0;
			jaccelerations[j][c] = 0.0;
		}
	}
	for(int j = 0; j < jcount; j++)
	{
		jaccelerations[j][1] = -gravity;
		jforces[j][1] = joints[j].mass.m * jaccelerations[j][1];
	}
}

int pictures = 100;
int substeps = 500;

int main(void)
{
	double mass_system = 0.0;
	for(int j = 0; j < jcount; j++)
	{
		mass_system += joints[j].mass.m;
	}
	printf("mass of system: %lf\n", mass_system);
	double center_system[3] = {0.0, 0.0, 0.0};
	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 3; c++)
		{
			center_system[c] += joints[j].mass.m * joints[j].mass.p[c];
		}
	}
	for(int c = 0; c < 3; c++)
	{
		center_system[c] /= mass_system;
	}
	printf("center of mass of system: (%lf, %lf, %lf)\n", center_system[0], center_system[1], center_system[2]);
	double momentum_system[3] = {0.0, 0.0, 0.0};
	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 3; c++)
		{
			momentum_system[c] += joints[j].mass.m * joints[j].mass.v[c];
		}
	}
	printf("momentum of system: <%lf, %lf, %lf>\n", momentum_system[0], momentum_system[1], momentum_system[2]);
	double energy_system = 0.0;
	for(int j = 0; j < jcount; j++)
	{
		double speed = 0.0;
		for(int c = 0; c < 3; c++)
		{
			speed += pow(joints[j].mass.v[c], 2);
		}
		speed = sqrt(speed);
		energy_system += 0.5 * joints[j].mass.m * pow(speed, 2);
		energy_system += joints[j].mass.m * gravity * joints[j].mass.v[1];
	}
	for(int m = 0; m < mcount; m++)
	{
		energy_system += 0.5 * members[m].spring.k * pow(sdisplacement(&members[m].spring), 2);
	}
	printf("energy of system: %lf\n", energy_system);
	init();
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
