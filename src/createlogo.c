#include <stdio.h>
#include <string.h>
#include <cairo.h>

int size;
char filename[1005];

int main(int argc, char **argv)
{
	if(argc != 3) return 1;
	if(sscanf(argv[1], "%1000s", filename) != 1) return 1;
	char *extension = strrchr(filename, '.');
	if(!extension || strcmp(extension, ".png") != 0)
		strcat(filename, ".png");
	if(sscanf(argv[2], "size=%d", &size) != 1) return 1;
	if(size < 8) return 1;
	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, size, size);
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_t *context = cairo_create(surface);
	if(cairo_status(context) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_save(context);
	cairo_new_path(context);
	cairo_rectangle(context, 0.0, 0.0, (double) size, (double) size);
	cairo_close_path(context);
	cairo_clip(context);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_paint(context);
	cairo_restore(context);
	cairo_save(context);
	cairo_scale(context, (double) size, (double) size);
	cairo_set_line_cap(context, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join(context, CAIRO_LINE_JOIN_ROUND);
	cairo_new_path(context);
	cairo_rectangle(context, 0.05, 0.05, 0.9, 0.35);
	cairo_close_path(context);
	cairo_set_line_width(context, 0.05);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_stroke_preserve(context);
	cairo_set_line_width(context, 0.02);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_stroke_preserve(context);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_fill(context);
	cairo_new_path(context);
	cairo_line_to(context, 0.05, 0.95);
	cairo_line_to(context, 0.95, 0.95);
	cairo_set_line_width(context, 0.05);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_stroke_preserve(context);
	cairo_set_line_width(context, 0.02);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_stroke(context);
	cairo_new_path(context);
	cairo_line_to(context, 0.2, 0.95);
	cairo_line_to(context, 0.4, 0.88125);
	cairo_line_to(context, 0.2, 0.8125);
	cairo_line_to(context, 0.4, 0.74375);
	cairo_line_to(context, 0.2, 0.675);
	cairo_line_to(context, 0.4, 0.60625);
	cairo_line_to(context, 0.2, 0.5375);
	cairo_line_to(context, 0.4, 0.46875);
	cairo_line_to(context, 0.2, 0.4);
	cairo_set_line_width(context, 0.05);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_stroke_preserve(context);
	cairo_set_line_width(context, 0.02);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_stroke(context);
	cairo_new_path(context);
	cairo_line_to(context, 0.7, 0.95);
	cairo_line_to(context, 0.7, 0.725);
	cairo_new_sub_path(context);
	cairo_line_to(context, 0.7, 0.575);
	cairo_line_to(context, 0.7, 0.4);
	cairo_new_sub_path(context);
	cairo_line_to(context, 0.625, 0.725);
	cairo_line_to(context, 0.775, 0.725);
	cairo_new_sub_path(context);
	cairo_line_to(context, 0.7, 0.575);
	cairo_line_to(context, 0.625, 0.575);
	cairo_line_to(context, 0.625, 0.8);
	cairo_new_sub_path(context);
	cairo_line_to(context, 0.7, 0.575);
	cairo_line_to(context, 0.775, 0.575);
	cairo_line_to(context, 0.775, 0.8);
	cairo_set_line_width(context, 0.05);
	cairo_set_source_rgb(context, 0.0, 0.0, 0.0);
	cairo_stroke_preserve(context);
	cairo_set_line_width(context, 0.02);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_stroke(context);
	cairo_restore(context);
	cairo_destroy(context);
	if(cairo_surface_write_to_png(surface, filename) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_surface_destroy(surface);
	return 0;
}
