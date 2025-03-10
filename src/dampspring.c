#include <math.h>

struct mass {
	double m;
	double p[2];
	double v[2];
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

double mdistance(struct mass *m1, struct mass *m2)
{
	double d = 0.0;
	for(int i = 0; i < 2; i++)
	{
		double dp = m1->p[i] - m2->p[i];
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
	for(int i = 0; i < 2; i++)
	{
		double dv = d->m1->v[i] - d->m2->v[i];
		double dp = d->m1->p[i] - d->m2->p[i];
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
