#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dampspring.h"

int mcount = 0;
constexpr int mlimit = 1024;
struct mass masses[mlimit] = {};
double maccelerations[mlimit][3] = {};
double mforces[mlimit][3] = {};

int scount = 0;
constexpr int slimit = 4096;
struct spring springs[slimit] = {};

int dcount = 0;
constexpr int dlimit = 4096;
struct damper dampers[dlimit] = {};

double time = 0.0;
void step(double delta_time)
{
	return 0;
}

int main(void)
{
	while(true)
	{

	}
	return 0;
}