#include "lib/trussdata.h"

int main(int argc, char **argv)
{
	if(scan_truss_problem() != 0) return 1;
	for(int m = 0; m < mcount; m++)
		members[m].spring.l0 = mdistance(members[m].spring.m1, members[m].spring.m2);
	print_truss_problem();
	free_truss_problem();
	return 0;
}
