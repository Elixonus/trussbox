#include <math.h>
#ifdef MSDAXES

struct mass
{
	double m;
	double p[MSDAXES];
	double v[MSDAXES];
};

struct spring
{
	double k;
	struct mass *m1;
	struct mass *m2;
	double l0;
};

struct damper
{
	double c;
	struct mass *m1;
	struct mass *m2;
};

double mdistance(struct mass *m1, struct mass *m2)
{
	double d = 0.0;
	for(int a = 0; a < MSDAXES; a++)
	{
		double dp = m1->p[a] - m2->p[a];
		d += dp * dp;
	}
	d = sqrt(d);
	return d;
}

double sdisplacement(struct spring *s)
{
	double l = mdistance(s->m1, s->m2);
	double l0 = s->l0;
	double d = l - l0;
	return d;
}

double dvelocity(struct damper *d)
{
	double l = mdistance(d->m1, d->m2);
	double v = 0.0;
	for(int a = 0; a < MSDAXES; a++)
	{
		double dv = d->m1->v[a] - d->m2->v[a];
		double dp = d->m1->p[a] - d->m2->p[a];
		v += dv * dp;
	}
	v /= l;
	return v;
}

double sforce(struct spring *s)
{
	double k = s->k;
	double d = sdisplacement(s);
	double f = -k * d;
	return f;
}

double dforce(struct damper *d)
{
	double c = d->c;
	double v = dvelocity(d);
	double f = -c * v;
	return f;
}

#endif
