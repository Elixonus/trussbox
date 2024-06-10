// find if energy of dampened floating system reaches 0
// find how closely oscillation matches dampened wave equation

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "lib/dampspring.h"

double randd()
{
	double r = ((double) rand()) / RAND_MAX;
	return r;
}

int main(void)
{
	srand(time(NULL));

	double dt = 1e-3;
	int nt = 1000;

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

		double w2 = spring.k / mass.m - pow(0.5 * damper.c / mass.m, 2);
		if(w2 < 1e-9)
		{
			s--;
			continue;
		}
		double w = sqrt(w2);

		double t = 0.0;
		for(int i = 0; i < nt; i++)
		{
			double f = sforce(&spring) + dforce(&damper);
			double a = f / mass.m;
			mass.v[0] += a * dt;
			mass.p[0] += mass.v[0] * dt;
			t += dt;
		}

		double x = x0 * exp(-0.5 * damper.c / mass.m * t) * cos(w * t);
		double e = abs((mass.p[0] - x) / x);
		printf("m=%f, x0=%f, k=%f, c=%f\n", mass.m, x0, spring.k, damper.c);
		printf("p[0]=%f, x=%f, e=%f\n", mass.p[0], x, e);
	}
}
