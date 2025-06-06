#include "lodepng.h"



typedef unsigned Error;

void _print_lodepng_error(const char *context, const Error error);

void _print_Image_error(const char *context, const char *message);

typedef struct Image {
	unsigned width, height;
	LodePNGColorType colortype;
	unsigned char *data;
} Image;

Error Image_open(Image **out, const char *filename, const LodePNGColorType colortype);

Error Image_save(const Image *image, const char *filename);

unsigned Image_xy_to_i(const Image *image, const unsigned x, const unsigned y);