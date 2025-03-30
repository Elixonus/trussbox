#include <stdio.h>
#include <string.h>
#include <cairo.h>

int size;
char filename[1005];

int main(int argc, char **argv)
{
	if(argc != 7) return 1;
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
	cairo_scale(context, (double) size, (double) size);

	cairo_new_path(context);
	cairo_rectangle(context, 0.05, 0.05, 0.9, 0.45);
	cairo_set_line_width(context, 0.05);
	cairo_set_source_rgb(context, 1.0, 1.0, 1.0);
	cairo_stroke(context);

	cairo_restore(context);
	cairo_destroy(context);
	if(cairo_surface_write_to_png(surface, filename) != CAIRO_STATUS_SUCCESS) return 1;
	cairo_surface_destroy(surface);
	return 0;
}
