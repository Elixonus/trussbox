struct joint {
	struct mass mass;
};

struct member {
	struct spring spring;
	struct damper damper;
};

struct support {
	struct constraint {
		bool a[2];
		struct mass *m;
		double p[2];
	} constraint;
};

struct load {
	struct action {
		double f[2];
		struct mass *m;
	} action;
};

struct joint *joints;
double **jforces;
int jcount;

struct member *members;
double *mlengths;
double *mdisplacements;
double *mvelocities;
double *mforces;
int mcount;

struct support *supports;
double **sreactions;
int scount;

struct load *loads;
int lcount;

double gravity;

int psize[2];
double pcenter[2];
double pzoom;
double pscale;

double epsilon = 1.0e-18;
char *filename;

void render(void)
{

}

int main(int argc, char **argv)
{
    if(argc != 2) return 1;
    if(sscanf(argv[1], "%1000s", filename) != 1) return 1;
    if(sscanf(argv[2], "gravity=%lf", &gravity) != 1) return 1;
    if(sscanf(argv[6], "psize=%dx%d", &psize[0], &psize[1]) != 2) return 1;
	if(psize[0] < 64 || psize[1] < 64) return 1;
	if(sscanf(argv[7], "pcenter=(%lf %lf)", &pcenter[0], &pcenter[1]) != 2) return 1;
	if(sscanf(argv[8], "pzoom=%lf", &pzoom) != 1) return 1;
    if(pzoom < epsilon) return 1;
	if(sscanf(argv[9], "fscale=%lf", &pscale) != 1) return 1;
	if(pscale < epsilon) return 1;
    if(scanf("joints=%d\n", &jcount) != 1) return 1;
	if(jcount < 0) return 1;
	joints = malloc(jcount * sizeof(struct joint));
    for(int j = 0; j < jcount; j++)
	{
		struct joint joint;
        if(scanf(
		    "mass=%lf position=(%lf %lf) velocity=<%lf %lf>\n",
		    &joint.mass.m, &joint.mass.p[0], &joint.mass.p[1], &joint.mass.v[0], &joint.mass.v[1]
		) != 5) return 1;
		if(joint.mass.m < epsilon) return 1;
		joints[j] = joint;
	}
    if(scanf("members=%d\n", &mcount) != 1) return 1;
	members = malloc(mcount * sizeof(struct member));
	for(int m = 0; m < mcount; m++)
	{
		int jindex1, jindex2;
		struct member member;
		if(scanf(
			"joint1=%d joint2=%d stiffness=%lf length0=%lf dampening=%lf\n",
			&jindex1, &jindex2, &member.spring.k, &member.spring.l0, &member.damper.c
		) != 5) return 1;
		jindex1--, jindex2--;
		if(jindex1 < 0 || jindex1 >= jcount || jindex2 < 0 || jindex2 >= jcount) return 1;
		for(int m2 = 0; m2 < m; m2++)
			if(
				(
					members[m2].spring.m1 == &joints[jindex1].mass &&
					members[m2].spring.m2 == &joints[jindex2].mass
				) ||
				(
					members[m2].spring.m1 == &joints[jindex2].mass &&
					members[m2].spring.m2 == &joints[jindex1].mass
				)
			) return 1;
		member.spring.m1 = &joints[jindex1].mass;
		member.spring.m2 = &joints[jindex2].mass;
		member.damper.m1 = &joints[jindex1].mass;
		member.damper.m2 = &joints[jindex2].mass;
		if(member.spring.l0 < epsilon) return 1;
		members[m] = member;
	}
	if(scanf("supports=%d\n", &scount) != 1) return 1;
	supports = malloc(scount * sizeof(struct support));
	for(int s = 0; s < scount; s++)
	{
		int jindex;
		char axes[3];
		struct support support;
        if(scanf("joint=%d axes=%2s\n", &jindex, axes) != 2) return 1;
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		for(int s2 = 0; s2 < s; s2++) if(supports[s2].constraint.m == &joints[jindex].mass) return 1;
		support.constraint.m = &joints[jindex].mass;
		if(strcmp(axes, "xy") == 0 || strcmp(axes, "yx") == 0)
		{
			support.constraint.a[0] = true;
			support.constraint.a[1] = true;
		}
		else if(strcmp(axes, "x") == 0)
		{
			support.constraint.a[0] = true;
			support.constraint.a[1] = false;
		}
		else if(strcmp(axes, "y") == 0)
		{
			support.constraint.a[0] = false;
			support.constraint.a[1] = true;
		}
		else return 1;
		for(int c = 0; c < 2; c++)
			support.constraint.p[c] = joints[jindex].mass.p[c];
		supports[s] = support;
	}
	if(scanf("loads=%d\n", &lcount) != 1) return 1;
	loads = malloc(lcount * sizeof(struct load));
	for(int l = 0; l < lcount; l++)
	{
		int jindex;
		struct load load;
		if(scanf(
			"joint=%d force=<%lf %lf>\n",
			&jindex, &load.action.f[0], &load.action.f[1]
		) != 3) return 1;
		jindex--;
		if(jindex < 0 || jindex >= jcount) return 1;
		load.action.m = &joints[jindex].mass;
		loads[l] = load;
	}
    int jcount2;
    if(scanf("joints=%d\n", &jcount2) != 1) return 1;
	if(jcount2 != jcount) return 1;
	jforces = malloc(jcount * sizeof(double *));
    for(int j = 0; j < jcount; j++)
    {
		jforces[j] = malloc(2 * sizeof(double));
        if(scanf(
            "force=<%lf %lf> position=(%lf %lf) velocity=<%lf %lf>\n",
            &jforces[j][0], &jforces[j][1],
            &joints[j].mass.p[0], &joints[j].mass.p[1],
            &joints[j].mass.v[0], &joints[j].mass.v[1]
        ) != 6) return 1;
    }
    int mcount2;
    if(scanf("members=%d\n", &mcount2) != 1) return 1;
    if(mcount2 != mcount) return 1;
	mlengths = malloc(mcount * sizeof(double));
	mdisplacements = malloc(mcount * sizeof(double));
	mvelocities = malloc(mcount * sizeof(double));
	mforces = malloc(mcount * sizeof(double));
    for(int m = 0; m < mcount; m++)
    {
        if(scanf(
            "force=%lf displacement=%lf length=%lf velocity=%lf",
            &mforces[m], &mdisplacements[m], &mlengths[m], &mvelocities[m]
        ) != 4) return 1;
    }
    int scount2;
    if(scanf("supports=%d\n", &scount2) != 1) return 1;
	sreactions = malloc(scount * sizeof(double *));
    for(int s = 0; s < scount; s++)
    {
        
    }
}
