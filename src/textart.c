#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "lib/trussdata.h"

#define EPSILON 1.0e-18

constexpr double PI = 4.0 * atan(1.0);
constexpr double TAU = 2.0 * PI;

int main(int argc, char **argv)
{
	double fcenter[2];
	if(argc < 2 || sscanf(argv[1], "fcenter=(%le %le)", &fcenter[0], &fcenter[1]) != 2)
	{
		fprintf(stderr, "error: parse: fcenter argument\n");
		fprintf(stderr, "usage: fcenter argument: fcenter=(float float)\n");
		return 1;
	}
	double fzoom;
	if(argc < 3 || sscanf(argv[2], "fzoom=%le", &fzoom) != 1)
	{
		fprintf(stderr, "error: parse: fzoom argument\n");
		fprintf(stderr, "usage: fzoom argument: fzoom=float\n");
		return 1;
	}
	if(fzoom < EPSILON)
	{
		fprintf(stderr, "error: limit: fzoom argument: %.1le not greater than %.1le\n", fzoom, EPSILON);
		return 1;
	}

	int linecount;
	if(argc < 4 || sscanf(argv[3], "lines=%d", &linecount) != 1)
	{
		fprintf(stderr, "error: parse: lines argument\n");
		fprintf(stderr, "usage: lines argument: lines=count\n");
		return 1;
	}
	if(linecount < 1 || linecount > 100)
	{
		fprintf(stderr, "error: limit: lines argument: not between %d and %d\n", 1, 100);
		return 1;
	}
	char colorarg[101];
	if(argc < 5 || sscanf(argv[4], "color=%100s", colorarg) != 1)
	{
		fprintf(stderr, "error: parse: color argument\n");
		fprintf(stderr, "usage: color argument: color=true|false\n");
		return 1;
	}
	bool usecolor;
	if(strcmp(colorarg, "true") == 0)
		usecolor = true;
	else if(strcmp(colorarg, "false") == 0)
		usecolor = false;
	else
	{
		fprintf(stderr, "error: parse: color argument: not an option\n");
		fprintf(stderr, "usage: color argument: color=true|false\n");
		return 1;
	}
	char croparg[101];
	if(argc < 6 || sscanf(argv[5], "vcrop=%100s", croparg) != 1)
	{
		fprintf(stderr, "error: parse: vcrop argument\n");
		fprintf(stderr, "usage: vcrop argument: vcrop=true|false\n");
		return 1;
	}
	bool docrop;
	if(strcmp(croparg, "true") == 0)
		docrop = true;
	else if(strcmp(croparg, "false") == 0)
		docrop = false;
	else
	{
		fprintf(stderr, "error: parse: vcrop argument: not an option\n");
		fprintf(stderr, "usage: vcrop argument: vcrop=true|false\n");
		return 1;
	}
	char title[2 * linecount + 1];
	char format[100];
	sprintf(format, "title=%%%d[^\n]", 2 * linecount);
	if(argc < 7 || sscanf(argv[6], format, title) != 1)
	{
		fprintf(stderr, "error: parse: title argument\n");
		fprintf(stderr, "usage: title argument: title=string\n");
		return 1;
	}
	if(scan_truss_problem() != 0) return 1;
	if(usecolor)
		if(scan_truss_solution(false) != 0) return 1;
	char textart[linecount][2 * linecount + 2];
	char colors[linecount][2 * linecount];
	for(int r = 0; r < linecount; r++)
	{
		for(int c = 0; c < 2 * linecount; c++)
		{
			textart[r][c] = ' ';
			if(usecolor) colors[r][c] = ' ';
		}
		textart[r][2 * linecount] = '\n', textart[r][2 * linecount + 1] = '\0';
	}
	void setchar(char ch, int r, int c, char clr)
	{
		if(r < 0 || c < 0 || r >= linecount || c >= 2 * linecount) return;
		textart[r][c] = ch;
		if(usecolor)
			colors[r][c] = clr;
	}
	double cutoff_force;
	if(usecolor)
	{
		cutoff_force = 0.0;
		for(int l = 0; l < lcount; l++)
		{
			double force = sqrt(pow(loads[l].action.f[0], 2.0) + pow(loads[l].action.f[1], 2.0));
			if(force > cutoff_force)
				cutoff_force = force;
		}
		for(int s = 0; s < scount; s++)
		{
			double force = sqrt(pow(sreactions[s][0], 2.0) + pow(sreactions[s][1], 2.0));
			if(force > cutoff_force)
				cutoff_force = force;
		}
		for(int m = 0; m < mcount; m++)
		{
			double force = fabs(mforces[m]);
			if(force > cutoff_force)
				cutoff_force = force;
		}
	}
	for(int m = 0; m < mcount; m++)
	{
		struct member *member = &members[m];
		double force;
		char color;
		if(usecolor)
		{
			force = mforces[m];
			if(cutoff_force < EPSILON)
			{
				color = 'G';
				goto bresenham;
			}
			double balance = force / cutoff_force;
			balance = balance < 1.0 ? (balance > -1.0 ? balance : -1.0) : 1.0;
			if(fabs(balance) < 0.2)
				color = 'G';
			else if(balance < 0.0)
				color = 'r';
			else
				color = 'b';
		}
		bresenham:
		int rowcol1[2] = {
			(int) round(((double) (linecount - 1)) * (0.5 - fzoom * (member->spring.m1->p[1] - fcenter[1]))),
			(int) round(((double) (2 * linecount - 1)) * (0.5 + fzoom * (member->spring.m1->p[0] - fcenter[0])))
		};
		int rowcol2[2] = {
			(int) round(((double) (linecount - 1)) * (0.5 - fzoom * (member->spring.m2->p[1] - fcenter[1]))),
			(int) round(((double) (2 * linecount - 1)) * (0.5 + fzoom * (member->spring.m2->p[0] - fcenter[0])))
		};
		int r1 = rowcol1[0], c1 = rowcol1[1];
		int r2 = rowcol2[0], c2 = rowcol2[1];
		int dc = abs(c2 - c1), sc = c1 < c2 ? 1 : -1;
		int dr = -abs(r2 - r1), sr = r1 < r2 ? 1 : -1;
		int error1 = dc + dr, error2;
		while(true)
		{
			bool bc = error2 >= dr, br = error2 <= dc;
			if(bc && br)
			{
				if(sc == sr)
					setchar('\\', r1, c1, color);
				else
					setchar('/', r1, c1, color);
			}
			else if(bc)
				setchar('-', r1, c1, color);
			else if(br)
				setchar('|', r1, c1, color);
			else
				setchar('*', r1, c1, color);
			if(c1 == c2 && r1 == r2) break;
			error2 = 2 * error1;
			if(error2 >= dr)
			{
				error1 += dr;
				c1 += sc;
			}
			if(error2 <= dc)
			{
				error1 += dc;
				r1 += sr;
			}
		}
	}
	for(int l = 0; l < lcount; l++)
	{
		struct load *load = &loads[l];
		double magnitude = sqrt(pow(load->action.f[0], 2.0) + pow(load->action.f[1], 2.0));
		if(magnitude < EPSILON) continue;
		double angle = fmod(atan2(load->action.f[1], load->action.f[0]) + TAU, TAU);
		int rowcol[2] = {
			(int) round(((double) (linecount - 1)) * (0.5 - fzoom * (load->action.m->p[1] - fcenter[1]))),
			(int) round(((double) (2 * linecount - 1)) * (0.5 + fzoom * (load->action.m->p[0] - fcenter[0])))
		};
		if(angle < 0.25 * PI || angle > 1.75 * PI) setchar('>', rowcol[0], rowcol[1] + 1, 'y');
		else if(angle < 0.75 * PI) setchar('^', rowcol[0] - 1, rowcol[1], 'y');
		else if(angle < 1.25 * PI) setchar('<', rowcol[0], rowcol[1] - 1, 'y');
		else setchar('v', rowcol[0] + 1, rowcol[1], 'y');
	}
	for(int j = 0; j < jcount; j++)
	{
		struct joint *joint = &joints[j];
		int rowcol[2] = {
			(int) round(((double) (linecount - 1)) * (0.5 - fzoom * (joint->mass.p[1] - fcenter[1]))),
			(int) round(((double) (2 * linecount - 1)) * (0.5 + fzoom * (joint->mass.p[0] - fcenter[0])))
		};
		setchar('*', rowcol[0], rowcol[1], ' ');
	}
	for(int s = 0; s < scount; s++)
	{
		struct support *support = &supports[s];
		if(!support->constraint.a[0] && !support->constraint.a[1]) continue;
		double ncenter[2] = {0.0, 0.0};
		int ncount = 0;
		for(int m = 0; m < mcount; m++)
		{
			struct member *member = &members[m];
			if(member->spring.m1 == support->constraint.m)
			{
				for(int a = 0; a < 2; a++)
					ncenter[a] += member->spring.m2->p[a];
				ncount++;
			}
			if(member->spring.m2 == support->constraint.m)
			{
				for(int a = 0; a < 2; a++)
					ncenter[a] += member->spring.m1->p[a];
				ncount++;
			}
		}
		for(int a = 0; a < 2; a++)
		{
			if(ncount > 0)
				ncenter[a] /= ncount;
			else
				ncenter[a] = support->constraint.m->p[a];
		}
		double nside;
		if(support->constraint.a[1])
			nside = ncenter[1] >= support->constraint.m->p[1] ? 1.0 : -1.0;
		if(support->constraint.a[0] && !support->constraint.a[1])
			nside = ncenter[0] >= support->constraint.m->p[0] ? 1.0 : -1.0;
		int rowcol[2] = {
			(int) round(((double) (linecount - 1)) * (0.5 - fzoom * (support->constraint.m->p[1] - fcenter[1]))),
			(int) round(((double) (2 * linecount - 1)) * (0.5 + fzoom * (support->constraint.m->p[0] - fcenter[0])))
		};
		if(support->constraint.a[1])
		{
			if(nside > 0.0)
			{
				setchar('[', rowcol[0], rowcol[1] - 1, ' ');
				setchar(']', rowcol[0], rowcol[1] + 1, ' ');
				setchar('/', rowcol[0] + 1, rowcol[1] - 1, ' ');
				setchar(' ', rowcol[0] + 1, rowcol[1], ' ');
				setchar('\\', rowcol[0] + 1, rowcol[1] + 1, ' ');
				if(support->constraint.a[0] && support->constraint.a[1])
				{
					setchar('=', rowcol[0] + 2, rowcol[1] - 2, ' ');
					setchar('=', rowcol[0] + 2, rowcol[1] - 1, ' ');
					setchar('=', rowcol[0] + 2, rowcol[1], ' ');
					setchar('=', rowcol[0] + 2, rowcol[1] + 1, ' ');
					setchar('=', rowcol[0] + 2, rowcol[1] + 2, ' ');
				}
				if((support->constraint.a[0] && !support->constraint.a[1]) || (!support->constraint.a[0] && support->constraint.a[1]))
				{
					setchar('o', rowcol[0] + 2, rowcol[1] - 2, ' ');
					setchar('o', rowcol[0] + 2, rowcol[1] - 1, ' ');
					setchar('o', rowcol[0] + 2, rowcol[1], ' ');
					setchar('o', rowcol[0] + 2, rowcol[1] + 1, ' ');
					setchar('o', rowcol[0] + 2, rowcol[1] + 2, ' ');
				}
			}
			else
			{
				setchar('[', rowcol[0], rowcol[1] - 1, ' ');
				setchar(']', rowcol[0], rowcol[1] + 1, ' ');
				setchar('\\', rowcol[0] - 1, rowcol[1] - 1, ' ');
				setchar(' ', rowcol[0] - 1, rowcol[1], ' ');
				setchar('/', rowcol[0] - 1, rowcol[1] + 1, ' ');
				if(support->constraint.a[0] && support->constraint.a[1])
				{
					setchar('=', rowcol[0] - 2, rowcol[1] - 2, ' ');
					setchar('=', rowcol[0] - 2, rowcol[1] - 1, ' ');
					setchar('=', rowcol[0] - 2, rowcol[1], ' ');
					setchar('=', rowcol[0] - 2, rowcol[1] + 1, ' ');
					setchar('=', rowcol[0] - 2, rowcol[1] + 2, ' ');
				}
				if((support->constraint.a[0] && !support->constraint.a[1]) || (!support->constraint.a[0] && support->constraint.a[1]))
				{
					setchar('o', rowcol[0] - 2, rowcol[1] - 2, ' ');
					setchar('o', rowcol[0] - 2, rowcol[1] - 1, ' ');
					setchar('o', rowcol[0] - 2, rowcol[1], ' ');
					setchar('o', rowcol[0] - 2, rowcol[1] + 1, ' ');
					setchar('o', rowcol[0] - 2, rowcol[1] + 2, ' ');
				}
			}
		}
		if(support->constraint.a[0] && !support->constraint.a[1])
		{
			if(nside > 0.0)
			{
				setchar('-', rowcol[0] - 1, rowcol[1], ' ');
				setchar('-', rowcol[0] + 1, rowcol[1], ' ');
				setchar('\\', rowcol[0] - 1, rowcol[1] - 1, ' ');
				setchar(' ', rowcol[0], rowcol[1] - 1, ' ');
				setchar('/', rowcol[0] + 1, rowcol[1] - 1, ' ');
				setchar('o', rowcol[0] - 2, rowcol[1] - 2, ' ');
				setchar('o', rowcol[0] - 1, rowcol[1] - 2, ' ');
				setchar('o', rowcol[0], rowcol[1] - 2, ' ');
				setchar('o', rowcol[0] + 1, rowcol[1] - 2, ' ');
				setchar('o', rowcol[0] + 2, rowcol[1] - 2, ' ');
			}
			else
			{
				setchar('-', rowcol[0] - 1, rowcol[1], ' ');
				setchar('-', rowcol[0] + 1, rowcol[1], ' ');
				setchar('/', rowcol[0] - 1, rowcol[1] + 1, ' ');
				setchar(' ', rowcol[0], rowcol[1] + 1, ' ');
				setchar('\\', rowcol[0] + 1, rowcol[1] + 1, ' ');
				setchar('o', rowcol[0] - 2, rowcol[1] + 2, ' ');
				setchar('o', rowcol[0] - 1, rowcol[1] + 2, ' ');
				setchar('o', rowcol[0], rowcol[1] + 2, ' ');
				setchar('o', rowcol[0] + 1, rowcol[1] + 2, ' ');
				setchar('o', rowcol[0] + 2, rowcol[1] + 2, ' ');
			}
		}
	}
	int rowstart = 0, rowend = linecount - 1;
	if(docrop)
	{
		for(int r = 0; r < linecount; r++)
		{
			bool blankline = true;
			for(int c = 0; c < 2 * linecount; c++)
			{
				if(textart[r][c] != ' ')
				{
					blankline = false;
					break;
				}
			}
			if(!blankline) break;
			rowstart++;
		}
		for(int r = linecount - 1; r > rowstart; r--)
		{
			bool blankline = true;
			for(int c = 0; c < 2 * linecount; c++)
			{
				if(textart[r][c] != ' ')
				{
					blankline = false;
					break;
				}
			}
			if(!blankline) break;
			rowend--;
		}
	}
	if(strcmp(title, "") != 0)
	{
		int padding = (2 * linecount - strlen(title)) / 2;
		if(usecolor)
		{
			printf("\e[0;30m\e[47m%*s%s%*s\e[0m\n", padding, "", title, padding + ((int) strlen(title)) % 2, "");
		}
		else
			printf("%*s%s%*s\n", padding, "", title, padding + ((int) strlen(title)) % 2, "");
		printf("%*s\n", 2 * linecount, "");
	}
	for(int r = rowstart; r <= rowend; r++)
		if(usecolor)
		{
			for(int c = 0; c < 2 * linecount + 1; c++)
			{
				if(c < 2 * linecount)
				{
					switch(colors[r][c])
					{
						case 'r':
							printf("\e[0;31m");
							break;
						case 'g':
							printf("\e[0;32m");
							break;
						case 'y':
							printf("\e[0;33m");
							break;
						case 'b':
							printf("\e[0;34m");
							break;
						case 'm':
							printf("\e[0;35m");
							break;
						case 'G':
							printf("\e[38;5;238m");
							break;
					}
				}
				printf("%c", textart[r][c]);
				if(c < 2 * linecount)
					printf("\e[0m");
			}
		}
		else
			printf("%s", textart[r]);
	free_truss_problem();
	if(usecolor)
		free_truss_solution();
	return 0;
}
