#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dampspring.h"

int main(int argc, char **argv) {
	if (argc != 4) {
		return 1;
	}
	int mcount, scount, dcount;
	sscanf(argv[1], "%d", &mcount);
	sscanf(argv[2], "%d", &scount);
	sscanf(argv[3], "%d", &dcount);
	struct mass masses[mcount];
	struct spring springs[scount];
	struct damper dampers[dcount];
	{
		struct mass mass;
		int index = 0;
		while (scanf("m %lf (%lf %lf %lf) <%lf %lf %lf>\n", &mass.m, &mass.p[0], &mass.p[1], &mass.p[2], &mass.v[0], &mass.v[1], &mass.v[2]) == 7) {
			masses[index++] = mass;
		}
	}
	{
		struct spring spring;
		int index1, index2;
		int index = 0;
		while (scanf("s %lf %d %d %lf\n", &spring.k, &index1, &index2, &spring.l0) == 4) {
			if ((index1 >= mcount || index1 < 0) || (index2 >= mcount || index2 < 0)) {
				return 1;
			}
			spring.m1 = &masses[index1];
			spring.m2 = &masses[index2];
			springs[index++] = spring;
		}
	}
	{
		struct damper damper;
		int index1, index2;
		int index = 0;
		while (scanf("d %lf %d %d\n", &damper.c, &index1, &index2) == 3) {
			if ((index1 >= mcount || index1 < 0) || (index2 >= mcount || index2 < 0)) {
				return 1;
			}
			damper.m1 = &masses[index1];
			damper.m2 = &masses[index2];
			dampers[index++] = damper;
		}
	}
	double mforces[mcount][3];
	double sforces[scount];
	double dforces[dcount];
	for (int s = 0; s < scount; s++) {
		struct spring *spring = &springs[s];
		double force = sforce(spring);
		sforces[s] = force;
		int index1, index2;
		for (int m = 0; m < mcount; m++) {
			if (&masses[m] == spring->m1) {
				index1 = m;
			}
			if (&masses[m] == spring->m2) {
				index2 = m;
			}
		}
		for (int c = 0; c < 3; c++) {
			mforces[index1][c] += force * (spring->m1->p[c] - spring->m2->p[c]) / mdistance(spring->m1, spring->m2);
			mforces[index2][c] += force * (spring->m2->p[c] - spring->m1->p[c]) / mdistance(spring->m1, spring->m2);
		}
	}
	for (int d = 0; d < dcount; d++) {
		struct damper *damper = &dampers[d];
		double force = dforce(damper);
		dforces[d] = force;
		int index1, index2;
		for (int m = 0; m < mcount; m++) {
			if (&masses[m] == damper->m1) {
				index1 = m;
			}
			if (&masses[m] == damper->m2) {
				index2 = m;
			}
		}
		for (int c = 0; c < 3; c++) {
			mforces[index1][c] += force * (damper->m1->p[c] - damper->m2->p[c]) / mdistance(damper->m1, damper->m2);
			mforces[index2][c] += force * (damper->m2->p[c] - damper->m1->p[c]) / mdistance(damper->m1, damper->m2);
		}
	}
	for (int m = 0; m < mcount; m++) {
		double *force = mforces[m];
		printf("m <%lf %lf %lf>\n", force[0], force[1], force[2]);
	}
	for (int s = 0; s < scount; s++) {
		double force = sforces[s];
		printf("s %lf\n", force);
	}
	for (int d = 0; d < dcount; d++) {
		double force = dforces[d];
		printf("d %lf\n", force);
	}
	return 0;
}
