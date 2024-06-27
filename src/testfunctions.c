#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "dampspring.h"

// find if energy of dampened floating system reaches 0
// find how closely oscillation matches dampened wave equation

constexpr double pi = 4.0 * atan(1.0);
constexpr double tau = 2.0 * pi;

double randd()
{
	double r = ((double) rand()) / RAND_MAX;
	return r;
}

int main(void)
{
	srand(time(NULL));

	for(int s = 0; s < 100; s++)
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
			.m1 = &mass,
			.m2 = &wall
		};
		double w2 = spring.k / mass.m;
		if(w2 < 1e-9)
		{
			s--;
			continue;
		}
		double t = 0.0;
		double dt = 1e-6;
		int nt = 1000000;
		for(int i = 0; i < nt; i++)
		{
			double f = (sforce(&spring) + dforce(&damper)) * mass.p[0] / fabs(mass.p[0]);
			double a = f / mass.m;
			mass.v[0] += a * dt;
			mass.p[0] += mass.v[0] * dt;
			t += dt;
			//printf("x=%f, p[0]=%f, f=%f, a=%f, v[0]=%f\n", x, mass.p[0], f, a, mass.v[0]);
		}
		double w = sqrt(w2);
		double x = x0 * exp(-0.5 * damper.c / mass.m * t) * cos(w * t);
		double error = fabs(mass.p[0] - x) / x0;
		if(fabs(error) > 1e-2 || true)
		{
			fprintf(stderr, "warning: harmonic oscillation test failed\n");
			fprintf(stderr, "parameters: x0=%lf, m=%lf, k=%lf, c=%lf, dt=%lf, nt=%d\n", x0, mass.m, spring.k, damper.c, dt, nt);
			fprintf(stderr, "results: error=%.15lf, p[0]=%lf, x=%lf, t=%lf\n", error, mass.p[0], x, t);
		}
	}
}
