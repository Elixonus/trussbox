#include <stdio.h>
#include <stdlib.h>
#include "dampspring.h"

int main(void)
{
	int mcount = 0, scount = 0, dcount = 0;
	int mlimit = 1, slimit = 1, dlimit = 1;
	struct mass *masses = malloc(mlimit * sizeof(struct mass));
	struct spring *springs = malloc(slimit * sizeof(struct spring));
	struct damper *dampers = malloc(dlimit * sizeof(struct damper));
	if (masses == nullptr || springs == nullptr || dampers == nullptr)
	{
		return 1;
	}

	while (true)
	{
		struct mass mass;
		if (scanf("m %lf (%lf %lf %lf) <%lf %lf %lf>\n", &mass.m, &mass.p[0], &mass.p[1], &mass.p[2], &mass.v[0], &mass.v[1], &mass.v[2]) == 7)
		{
			mcount++;
			if (mcount > mlimit)
			{
				int quotient = mlimit;
				mlimit *= 2;
				if (mlimit / 2 != quotient)
				{
					return 1;
				}

				masses = realloc(masses, mlimit * sizeof(struct mass));
				if(masses == nullptr)
				{
					return 1;
				}
			}
			masses[mcount - 1] = mass;
			continue;
		}

		int index1, index2;

		struct spring spring;
		if (scanf("s %lf %d %d %lf\n", &spring.k, &index1, &index2, &spring.l0) == 4)
		{
			scount++;
			if (scount > slimit)
			{
				int quotient = slimit;
				slimit *= 2;
				if (slimit / 2 != quotient)
				{
					return 1;
				}

				springs = realloc(springs, slimit * sizeof(struct spring));
				if(springs == nullptr)
				{
					return 1;
				}
			}

			if (index1 >= mcount || index1 < 0 || index2 >= mcount || index2 < 0)
			{
				return 1;
			}
			spring.m1 = &masses[index1];
			spring.m2 = &masses[index2];
			springs[scount - 1] = spring;
			continue;
		}

		struct damper damper;
		if (scanf("d %lf %d %d\n", &damper.c, &index1, &index2) == 3)
		{
			dcount++;
			if (dcount > dlimit)
			{
				int quotient = dlimit;
				dlimit *= 2;
				if (dlimit / 2 != quotient)
				{
					return 1;
				}

				dampers = realloc(dampers, dlimit * sizeof(struct damper));
				if(dampers == nullptr)
				{
					return 1;
				}
			}

			if (index1 >= mcount || index1 < 0 || index2 >= mcount || index2 < 0)
			{
				return 1;
			}
			damper.m1 = &masses[index1];
			damper.m2 = &masses[index2];
			dampers[dcount - 1] = damper;
			continue;
		}

		break;
	}

	double (*mforces)[3] = malloc(mcount * sizeof(double[3]));
	double *sforces = malloc(scount * sizeof(double));
	double *dforces = malloc(dcount * sizeof(double));
	if (mforces == nullptr || sforces == nullptr || dforces == nullptr)
	{
		return 1;
	}

	printf("mcount: %d\n", mcount);
	printf("scount: %d\n", scount);
	printf("dcount: %d\n", dcount);
	printf("ended\n");
	return 0;
}