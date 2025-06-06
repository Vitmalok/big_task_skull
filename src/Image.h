#include "lodepng.h"



typedef unsigned Error;

typedef struct Image {
	unsigned width, height;
	LodePNGColorType colortype;
	unsigned char *data;
} Image;

Error Image_new(Image **out, const int width, const int height, const LodePNGColorType colortype);

Error Image_open(Image **out, const char *filename, const LodePNGColorType colortype);

Error Image_save(const Image *image, const char *filename);

void Image_free(Image *image);

int Image_xy_to_i(const Image *image, const int x, const int y);



Error gaussian_blur(Image **out, const Image *image, const double sigma);