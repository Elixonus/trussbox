#include <stdio.h>
#include <math.h>
#include <string.h>

#include "lib/trussdata.h"

constexpr double PI = 4.0 * atan(1.0);

int main(int argc, char **argv)
{
	if(scan_truss_problem() != 0) return 1;
	for(int a = 1; a < argc; a++)
	{
		if(strncmp(argv[a], "translate", strlen("translate")) == 0)
		{
			double translation[2];
			if(sscanf(argv[a], "translate=<%le %le>", &translation[0], &translation[1]) != 2)
			{
				fprintf(stderr, "error: parse: translate argument [%d]\n", a);
				fprintf(stderr, "usage: translate argument: translate=<float float>\n");
				return 1;
			}
			for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
					joints[j].mass.p[a] += translation[a];
		}
		else if(strncmp(argv[a], "rotate", strlen("rotate")) == 0)
		{
			double rotation;
			if(sscanf(argv[a], "rotate=%le", &rotation) != 1)
			{
				fprintf(stderr, "error: parse: rotate argument [%d]\n", a);
				fprintf(stderr, "usage: rotate argument: rotate=float\n");
				return 1;
			}
			double radians = PI / 180.0 * rotation;
			for(int j = 0; j < jcount; j++)
			{
				double position[2] = {joints[j].mass.p[0], joints[j].mass.p[1]};
				joints[j].mass.p[0] = position[0] * cos(radians) - position[1] * sin(radians);
				joints[j].mass.p[1] = position[0] * sin(radians) + position[1] * cos(radians);
			}
			for(int l = 0; l < lcount; l++)
			{
				double force[2] = {loads[l].action.f[0], loads[l].action.f[1]};
				loads[l].action.f[0] = force[0] * cos(radians) - force[1] * sin(radians);
				loads[l].action.f[1] = force[0] * sin(radians) + force[1] * cos(radians);
			}
		}
		else if(strncmp(argv[a], "scale", strlen("scale")) == 0)
		{
			double scale;
			if(sscanf(argv[a], "scale=%le", &scale) != 1)
			{
				fprintf(stderr, "error: parse: scale argument [%d]\n", a);
				fprintf(stderr, "usage: scale argument: scale=float\n");
				return 1;
			}
			for(int j = 0; j < jcount; j++) for(int a = 0; a < 2; a++)
					joints[j].mass.p[a] *= scale;
			for(int m = 0; m < mcount; m++)
				members[m].spring.l0 *= scale;
		}
		else
		{
			fprintf(stderr, "error: parse: argument [%d]\n", a);
			fprintf(stderr, "usage: transform argument: translate...|rotate...|scale...\n");
			return 1;
		}
	}
	print_truss_problem();
	free_truss_problem();
	return 0;
}
