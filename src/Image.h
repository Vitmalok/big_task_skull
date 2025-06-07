#include "lodepng.h"



typedef unsigned Error;



typedef struct Image {
	unsigned width, height;
	unsigned char *data;
} Image;

Error Image_new(Image **out, const int width, const int height);

Error Image_copy(Image **out, const Image *image);

Error Image_open(Image **out, const char *filename);

Error Image_save(const Image *image, const char *filename);

void Image_free(Image *image);

int Image_xy_to_i(const Image *image, const int x, const int y);



typedef struct DoubleImage {
	unsigned width, height;
	double *data;
} DoubleImage;

Error DoubleImage_new(DoubleImage **out, const int width, const int height);

Error DoubleImage_copy(DoubleImage **out, const DoubleImage *d_image);

void DoubleImage_free(DoubleImage *d_image);

int DoubleImage_xy_to_i(const DoubleImage *d_image, const int x, const int y);

Error DoubleImage_vector_call(DoubleImage **out, double func(double, double), const DoubleImage *d_image1, const DoubleImage *d_image2);



Error Image_to_DoubleImage(DoubleImage **out, const Image *image);

Error DoubleImage_to_Image(Image **out, const DoubleImage *d_image);



Error Image_gaussian_blur(Image *image, const double sigma);

Error apply_kernel(DoubleImage *d_image, const int kernel_width, const int kernel_height, const double *kernel);
