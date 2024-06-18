// find if energy of dampened floating system reaches 0
// find how closely oscillation matches dampened wave equation

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cairo.h>
#include "dampspring.h"

double randd()
{
	double r = ((double) rand()) / RAND_MAX;
	return r;
}

int main(void)
{
	srand(time(NULL));

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 1000, 1000);
	cairo_t *context1 = cairo_create(surface);
	cairo_t *context2 = cairo_create(surface);

	double dt = 1e-8;
	int nt = 1000000000;

	for(int s = 0; s < 1; s++)
	{
		double x0 = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5));
		struct mass wall = {};
		struct mass mass = {
			.m = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5)),
			.p = {x0}
		};
		struct spring spring = {
			.k = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5)),
			.m1 = &mass,
			.m2 = &wall,
			.l0 = 0.0
		};
		struct damper damper = {
			.c = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5)),
			.m1 = &mass,
			.m2 = &wall
		};

		mass.m = 0.378132;
		x0 = 9.507635;
		mass.p[0] = x0;
		spring.k = 66.794948;
		damper.c = 1.917927;
		damper.c = 0.0;

		double w2 = spring.k / mass.m - pow(0.5 * damper.c / mass.m, 2);
		if(w2 < 1e-9)
		{
			s--;
			continue;
		}
		double w = sqrt(w2);
		double a;
		double t = 0.0;
		double x;
		for(int i = 0; i < nt; i++)
		{
			mass.v[0] += 0.5 * a * dt;
			mass.p[0] += mass.v[0] * dt;
			double f = (sforce(&spring) + dforce(&damper)) * mass.p[0] / fabs(mass.p[0]);
			a = f / mass.m;
			mass.v[0] += 0.5 * a * dt;
			t += dt;
			x = x0 * exp(-0.5 * damper.c / mass.m * t) * cos(w * t);
			//printf("x=%f, p[0]=%f, f=%f, a=%f, v[0]=%f\n", x, mass.p[0], f, a, mass.v[0]);
			cairo_line_to(context1, 10000.0 * t, 500.0 - 20.0 * mass.p[0]);
			cairo_line_to(context2, 10000.0 * t, 500.0 - 20.0 * x);
			if(fabs(x / x0) < 0.5)
			{
				break;
			}
		}
		double error = fabs(mass.p[0] - x) / x;
		if(fabs(error) > 1e-6 || 1==1)
		{
			fprintf(stderr, "warning: harmonic oscillation test failed\n");
			fprintf(stderr, "parameters: x0=%lf, m=%lf, k=%lf, c=%lf\n", x0, mass.m, spring.k, damper.c);
			fprintf(stderr, "results: error=%.15lf, p[0]=%lf, x=%lf, t=%lf\n", error, mass.p[0], x, t);
		}
	}

	cairo_set_source_rgb(context1, 1.0, 0.0, 0.0);
	cairo_set_line_width(context1, 10);
	cairo_stroke(context1);
	cairo_set_source_rgb(context2, 0.0, 0.0, 1.0);
	cairo_set_line_width(context2, 10);
	cairo_stroke(context2);

	cairo_destroy(context1);
	cairo_destroy(context2);
	cairo_surface_write_to_png(surface, "image.png");
	cairo_surface_destroy(surface);
}
