#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "dampspring.h"

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
	bool harmonic = true;
	for(int s = 0; s < 100; s++)
	{
		double x0 = 10.0 * (2.0 * (randd() - 0.5)) * pow(10.0, 2.0 * (randd() - 0.5));
		struct mass wall = {};
		struct mass mass = {
			.m = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5)),
			.p = {x0, 0.0}
		};
		struct spring spring = {
			.k = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5)),
			.m1 = &mass,
			.m2 = &wall,
			.l0 = 0.0
		};
		double t = 0.0;
		double dt = 1.0e-5 * (randd() + 0.5) * (tau / sqrt(spring.k / mass.m));
		int nt = 1000000;
		for(int i = 0; i < nt; i++)
		{
			double f = sforce(&spring) * mass.p[0] / fabs(mass.p[0]);
			double a = f / mass.m;
			mass.v[0] += a * dt;
			mass.p[0] += mass.v[0] * dt;
			t += dt;
		}
		double w = sqrt(spring.k / mass.m);
		double x = x0 * cos(w * t);
		double error = fabs(mass.p[0] - x) / (fabs(x0) > 1e-9 ? x0 : 1e-9);
		if(fabs(error) > 1.0e-2)
		{
			harmonic = false;
			printf("harmonic oscillation test failed\n");
			printf("x0=%lf, m=%lf, k=%lf, dt=%lf, nt=%d\n", x0, mass.m, spring.k, dt, nt);
			printf("error=%.15lf, xc=%lf, x=%lf, t=%lf\n", error, mass.p[0], x, t);
		}
	}
	if(harmonic == true)
	{
		printf("harmonic oscillation tests passed\n");
	}
}
