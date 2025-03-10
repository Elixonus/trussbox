#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>
#include "dampspring.h"

// -f truss.txt -t 10.0 -dt 0.01
// -f system.txt -t 10.0 -i 1000 -g 9.8
// using dynamic malloc or bounded array

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
	struct mass *mass;
	struct constraint {
		bool a[2];
		double p0[2];
	} constraint;
};

constexpr int jcount = 12;
struct joint joints[jcount] = {
	{
		.mass = {
			.m = 1.0,
			.p = {0.0, 0.0},
			.v = {0.0, 0.0}
		}
	},
	{
		.mass = {
			.m = 1.0,
			.p = {0.25, 0.0},
			.v = {0.0, 0.0}
		}
	},
	{
		.mass = {
			.m = 1.0,
			.p = {0.5, 0.0},
			.v = {0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.75, 0.0},
			.v = {0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {1.0, 0.0},
			.v = {0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.125, 0.25},
			.v = {0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.375, 0.25},
			.v = {0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.625, 0.25},
			.v = {0.0, 0.0}
		}
	},
   	{
		.mass = {
			.m = 1.0,
			.p = {0.875, 0.25},
			.v = {0.0, 0.0}
		}
	},
    {
    	.mass = {
        	.m = 1.0,
            .p = {0.25, 0.5},
            .v = {0.0, 0.0}
        }
    },
    {
    	.mass = {
        	.m = 1.0,
            .p = {0.50, 0.5},
            .v = {0.0, 0.0}
        }
    },
    {
    	.mass = {
        	.m = 1.0,
            .p = {0.75, 0.5},
            .v = {0.0, 0.0}
        }
    }
};
double jaccelerations[jcount][2];
double jforces[jcount][2];

constexpr int mcount = 23;
struct member members[mcount] = {
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[0].mass,
			.m2 = &joints[1].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[1].mass,
			.m2 = &joints[2].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[1].mass,
			.m2 = &joints[2].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[2].mass,
			.m2 = &joints[3].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[2].mass,
			.m2 = &joints[3].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[3].mass,
			.m2 = &joints[4].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[3].mass,
			.m2 = &joints[4].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[5].mass,
			.m2 = &joints[6].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[5].mass,
			.m2 = &joints[6].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[6].mass,
			.m2 = &joints[7].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[6].mass,
			.m2 = &joints[7].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[7].mass,
			.m2 = &joints[8].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[7].mass,
			.m2 = &joints[8].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[9].mass,
			.m2 = &joints[10].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[9].mass,
			.m2 = &joints[10].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[10].mass,
			.m2 = &joints[11].mass,
			.l0 = 0.25
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[10].mass,
			.m2 = &joints[11].mass
		}
	},
	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[0].mass,
			.m2 = &joints[5].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[0].mass,
			.m2 = &joints[5].mass
		}
	},
 	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[1].mass,
			.m2 = &joints[6].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[1].mass,
			.m2 = &joints[6].mass
		}
	},
  	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[2].mass,
			.m2 = &joints[7].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[2].mass,
			.m2 = &joints[7].mass
		}
	},
   	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[3].mass,
			.m2 = &joints[8].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[3].mass,
			.m2 = &joints[8].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[5].mass,
			.m2 = &joints[9].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[5].mass,
			.m2 = &joints[9].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[6].mass,
			.m2 = &joints[10].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[6].mass,
			.m2 = &joints[10].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[7].mass,
			.m2 = &joints[11].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[7].mass,
			.m2 = &joints[11].mass
		}
	},
   	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[1].mass,
			.m2 = &joints[5].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[1].mass,
			.m2 = &joints[5].mass
		}
	},
   	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[2].mass,
			.m2 = &joints[6].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[2].mass,
			.m2 = &joints[6].mass
		}
	},
   	{
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[3].mass,
			.m2 = &joints[7].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[3].mass,
			.m2 = &joints[7].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[4].mass,
			.m2 = &joints[8].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[4].mass,
			.m2 = &joints[8].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[6].mass,
			.m2 = &joints[9].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[6].mass,
			.m2 = &joints[9].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[7].mass,
			.m2 = &joints[10].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[7].mass,
			.m2 = &joints[10].mass
		}
	},
    {
		.spring = {
			.k = 3.0e3,
			.m1 = &joints[8].mass,
			.m2 = &joints[11].mass,
			.l0 = 0.2795084972
		},
		.damper = {
			.c = 3.0e2,
			.m1 = &joints[8].mass,
			.m2 = &joints[11].mass
		}
	}
};

constexpr int scount = 2;
struct support supports[scount] = {
	{
		.mass = &joints[0].mass,
		.constraint = {
			.a = {false, true},
			.p0 = {0.0, 0.0}
		}
	},
	{
		.mass = &joints[4].mass,
		.constraint = {
			.a = {false, true},
			.p0 = {1.0, 0.0}
		}
	}
};

double gravity;
double epsilon;
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
			jaccelerations[j][c] = 0.0;
			jforces[j][c] = 0.0;
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
        if(mlength < epsilon)
        {
        	continue;
        }
		double mdirection[2];
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
		for(int c = 0; c < 2; c++)
		{
			mdirection[c] = (joints[j2].mass.p[c] - joints[j1].mass.p[c]) / mlength;
			jforces[j1][c] -= mdirection[c] * mforce;
			jforces[j2][c] += mdirection[c] * mforce;
		}
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
        for(int c = 0; c < 2; c++)
        {
        	if(support->constraint.a[c])
            {
            	support->mass->p[c] = support->constraint.p0[c];
                support->mass->v[c] = 0.0;
            }
        }
	}
	step++;
	time += dtime;
	return 0;
}

int fwidth = 1920, fheight = 1080;
double fcenter[2];
double fzoom;
double fscale;
int frame;

void render(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, fwidth, fheight);
	cairo_t *context = cairo_create(surface);
	cairo_save(context);
	cairo_translate(context, 0.0, 0.5 * ((double) fheight));
	cairo_scale(context, 1.0, -1.0);
	cairo_translate(context, 0.0, -0.5 * ((double) fheight));
	cairo_save(context);
	cairo_translate(context, 0.5 * ((double) fwidth), 0.5 * ((double) fheight));
	cairo_save(context);
	cairo_scale(context, (double) fwidth, (double) fheight);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_paint(context);
	cairo_restore(context);
	double length = fwidth < fheight ? (double) fwidth : (double) fheight;
	cairo_scale(context, length, length);
	cairo_scale(context, fzoom, fzoom);
	cairo_translate(context, -fcenter[0], -fcenter[1]);
	cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_ROUND);
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		cairo_save(context);
		cairo_translate(context, support->mass->p[0], support->mass->p[1]);
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
            if(member->spring.m1 == support->mass)
            {
            	for(int c = 0; c < 2; c++)
                {
                	ncenter[c] += member->spring.m2->p[c];
                }
                ncount++;
            }
            if(member->spring.m2 == support->mass)
            {
            	for(int c = 0; c < 2; c++)
            	{
            		ncenter[c] += member->spring.m1->p[c];
            	}
                ncount++;
            }
        }
        for(int c = 0; c < 2; c++)
        {
        	ncenter[c] /= ncount;
        }
		if((ncount == 1 && ncenter[0] - support->mass->p[0] < 0) || (ncount == 2 && ncenter[1] - support->mass->p[1] < 0))
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
	sprintf(filename, "out/img/%05d.png", frame);
	cairo_surface_write_to_png(surface, filename);
	cairo_surface_destroy(surface);
	frame++;
}

double timef;
double srate;
double stepf;
double dtime;
double frate;
double framef;

int main(void)
{

	timef = 2.0;
    srate = 60000.0;
    stepf = srate * timef;
    dtime = 1.0 / srate;
    frate = 60.0;
    framef = frate * timef;
    gravity = 9.8;
    epsilon = 1e-9;
    fcenter[0] = 0.5; fcenter[1] = 0.125;
    fzoom = 1.0;
    fscale = 1.0;
	time = 0.0;
	step = 0;
	frame = 0;
	double smass = 0.0;
	for(int j = 0; j < jcount; j++)
	{
		smass += joints[j].mass.m;
	}
	printf("mass of system: %.3lf kg\n", smass);
	double scenter[2] = {0.0, 0.0};
	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 2; c++)
		{
			scenter[c] += joints[j].mass.m * joints[j].mass.p[c];
		}
	}
	for(int c = 0; c < 2; c++)
	{
		scenter[c] /= smass;
	}
	printf("center of mass of system: (%.3lf %.3lf) m\n", scenter[0], scenter[1]);
	double smomentum[2] = {0.0, 0.0};
	for(int j = 0; j < jcount; j++)
	{
		for(int c = 0; c < 2; c++)
		{
			smomentum[c] += joints[j].mass.m * joints[j].mass.v[c];
		}
	}
	printf("momentum of system: <%.3lf %.3lf> kg*m/s\n", smomentum[0], smomentum[1]);
	double senergy = 0.0;
	for(int j = 0; j < jcount; j++)
	{
		double speed = 0.0;
		for(int c = 0; c < 2; c++)
		{
			speed += pow(joints[j].mass.v[c], 2);
		}
		speed = sqrt(speed);
		senergy += 0.5 * joints[j].mass.m * pow(speed, 2);
		senergy += joints[j].mass.m * gravity * joints[j].mass.p[1];
	}
	for(int m = 0; m < mcount; m++)
	{
		senergy += 0.5 * members[m].spring.k * pow(sdisplacement(&members[m].spring), 2);
	}
	printf("energy of system: %.3lf J\n", senergy);
	while(((double) frame) / framef < 1.0)
	{
        render();
        while(((double) step) / stepf < ((double) frame) / framef)
        {
    		solve();
        }
	}
	return 0;
}
