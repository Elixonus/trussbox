// find if energy of dampened floating system reaches 0
// find how closely oscillation matches dampened wave equation

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "dampspring.h"

double randd()
{
	double r = ((double) rand()) / RAND_MAX;
	return r;
}

int main(void)
{
	srand(time(NULL));

	double dt = 1e-10;
	int nt = 10000000;

	for(int s = 0; s < 1; s++)
	{
		double x0 = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5));

		struct mass wall;
		wall.p[0] = 0.0;
		wall.p[1] = 0.0;
		wall.p[2] = 0.0;
		wall.v[0] = 0.0;
		wall.v[1] = 0.0;
		wall.v[2] = 0.0;

		struct mass mass;
		mass.m = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5));
		mass.p[0] = x0;
		mass.p[1] = 0.0;
		mass.p[2] = 0.0;
		mass.v[0] = 0.0;
		mass.v[1] = 0.0;
		mass.v[2] = 0.0;

		struct spring spring;
		spring.k = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5));
		spring.m1 = &mass;
		spring.m2 = &wall;
		spring.l0 = 0.0;

		struct damper damper;
		damper.c = 10.0 * randd() * pow(10.0, 2.0 * (randd() - 0.5));
		damper.m1 = &mass;
		damper.m2 = &wall;

		mass.m = 0.378132;
		x0 = 9.507635;
		mass.p[0] = x0;
		spring.k = 66.794948;
		damper.c = 1.917927;

		double w2 = spring.k / mass.m - pow(0.5 * damper.c / mass.m, 2);
		if(w2 < 1e-9)
		{
			s--;
			continue;
		}
		double w = sqrt(w2);

		double t = 0.0;
		double x;
		for(int i = 0; i < nt; i++)
		{
			double f = (sforce(&spring) + dforce(&damper)) * mass.p[0] / fabs(mass.p[0]);
			double a = f / mass.m;
			mass.p[0] += (mass.v[0] + 0.5 * a) * dt;
			f = (sforce(&spring) + dforce(&damper)) * mass.p[0] / fabs(mass.p[0]);
			double anew = f / mass.m;
			mass.v[0] += 0.5 * (a + anew) * dt;
			t += dt;
			x = x0 * exp(-0.5 * damper.c / mass.m * t) * cos(w * t);
			//printf("x=%f, p[0]=%f, f=%f, a=%f, v[0]=%f\n", x, mass.p[0], f, a, mass.v[0]);
			if(fabs(x / x0) < 0.1)
			{
				break;
			}
		}

		double error = fabs(mass.p[0] - x) / x;
		if(error > 1e-6 || 1==1)
		{
			fprintf(stderr, "warning: harmonic oscillation test failed\n");
			fprintf(stderr, "parameters: x0=%lf, m=%lf, k=%lf, c=%lf\n", x0, mass.m, spring.k, damper.c);
			fprintf(stderr, "results: error=%.15lf, p[0]=%lf, x=%lf, t=%lf\n", error, mass.p[0], x, t);
		}
	}
}
