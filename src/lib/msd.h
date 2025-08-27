#pragma once

#ifdef MSDAXES

struct mass {
	double m;
	double p[MSDAXES];
	double v[MSDAXES];
};

struct spring {
	double k;
	struct mass *m1;
	struct mass *m2;
	double l0;
};

struct damper {
	double c;
	struct mass *m1;
	struct mass *m2;
};

double mdistance(struct mass *m1, struct mass *m2);

double sdisplacement(struct spring *s);

double dvelocity(struct damper *d);

double sforce(struct spring *s);

double dforce(struct damper *d);

#endif
