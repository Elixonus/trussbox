#include "lib/trussdata.h"

int main(int argc, char **argv)
{
	if(scan_truss_problem() != 0) return 1;
	if(scan_truss_solution(true) != 0) return 1;
	print_truss_problem();
	free_truss_problem();
	return 0;
}
