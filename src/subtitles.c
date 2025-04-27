#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>

int scount;
char **stexts;
double **scenters;
double *slineheights;
char filename[1005];
int fsize[2];

int render(void)
{
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, fsize[0], fsize[1]);
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_t *context = cairo_create(surface);
	if(cairo_status(context) != CAIRO_STATUS_SUCCESS) return 1;
	double flength = fsize[0] < fsize[1] ? (double) fsize[0] : (double) fsize[1];
	for(int s = 0; s < scount; s++)
	{
		cairo_select_font_face(context, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(context, slineheights[s] * flength);
		cairo_text_extents_t extents;
		cairo_text_extents(context, stexts[s], &extents);
		cairo_new_path(context);
		cairo_rectangle(context,
		                0.5 * ((double) fsize[0]) + scenters[s][0] * ((double) fsize[0]) - 0.5 * extents.width - 0.025 * flength,
		                0.5 * ((double) fsize[1]) - scenters[s][1] * ((double) fsize[1]) - 0.5 * extents.height - 0.025 * flength,
		                extents.width + 0.05 * flength,
		                extents.height + 0.05 * flength);
		cairo_close_path(context);
		cairo_set_line_width(context, 0.005 * flength);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_stroke_preserve(context);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_fill(context);
		cairo_new_path(context);
		cairo_move_to(context,
		              0.5 * ((double) fsize[0]) + scenters[s][0] * ((double) fsize[0]) - 0.5 * extents.width,
		              0.5 * ((double) fsize[1]) - scenters[s][1] * ((double) fsize[1]) + 0.5 * extents.height);
		cairo_text_path(context, stexts[s]);
		cairo_set_line_width(context, 0.15 * slineheights[s] * flength);
		cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
		cairo_stroke_preserve(context);
		cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
		cairo_fill(context);
	}
	cairo_destroy(context);
	if(cairo_surface_write_to_png(surface, filename) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_surface_destroy(surface);
	return 0;
}

int main(int argc, char **argv)
{
	if(argc != 3) return 1;
	if(sscanf(argv[1], "%1000s", filename) != 1) return 1;
	char *extension = strrchr(filename, '.');
	if(!extension || strcmp(extension, ".png") != 0)
		strcat(filename, ".png");
	if(sscanf(argv[2], "fsize=%dx%d", &fsize[0], &fsize[1]) != 2) return 1;
	if(scanf("subtitles=%d\n", &scount) != 1) return 1;
	if(scount < 0) return 1;
	stexts = malloc(scount * sizeof(char *));
	if(!stexts) return 1;
	scenters = malloc(scount * sizeof(double *));
	if(!scenters) return 1;
	slineheights = malloc(scount * sizeof(double));
	if(!slineheights) return 1;
	for(int s = 0; s < scount; s++)
	{
		stexts[s] = malloc(1001 * sizeof(char));
		scenters[s] = malloc(2 * sizeof(double));
		if(scanf("center=(%lf %lf) lineheight=%lf text=%1000[^\n]\n",
		         &scenters[s][0], &scenters[s][1], &slineheights[s], stexts[s]) != 4) return 1;
	}
	if(render() != 0) return 1;
	for(int s = 0; s < scount; s++)
	{
		free(stexts[s]);
		free(scenters[s]);
	}
	free(stexts);
	free(scenters);
	free(slineheights);
	return 0;
}
