#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
#include "Image.h"



void _print_lodepng_error(const char *context, const Error error) {
	printf("LodepngError %u in %s: %s\n", error, context, lodepng_error_text(error));
}
void _print_Image_error(const char *context, const char *message) {
	printf("ImageError in %s: %s\n", context, message);
}

#define _HANDLE_LODEPNG_ERR(context, err) \
if (err != 0) {\
	_print_lodepng_error(context, err);\
	return err;\
}

#define _MIN(a, b) a < b ? a : b
#define _MAX(a, b) a > b ? a : b



Error Image_new(Image **out, const int width, const int height, const LodePNGColorType colortype) {
	if (width < 0 || height < 0) {
		_print_Image_error("Image_new", "Negative image size");
		return 1;
	}
	Image *image = malloc(sizeof(Image));
	image->width = (unsigned)width;
	image->height = (unsigned)height;
	image->colortype = colortype;
	image->data = malloc((unsigned)width*(unsigned)height*sizeof(char));
	if (image->data == NULL) {
		_print_Image_error("Image_new", "Memory error");
		return 1;
	}
	*out = image;
	return 0;
}

Error Image_open(Image **out, const char *filename, const LodePNGColorType colortype) {
	Image *image = malloc(sizeof(Image));
	image->colortype = colortype;
	Error err = lodepng_decode_file(&image->data, &image->width, &image->height, filename, colortype, 8);
	_HANDLE_LODEPNG_ERR("Image_open", err)
	*out = image;
	return 0;
}

Error Image_save(const Image *image, const char *filename) {
	Error err = lodepng_encode_file(filename, image->data, image->width, image->height, image->colortype, 8);
	_HANDLE_LODEPNG_ERR("Image_save", err)
	return 0;
}

void Image_free(Image *image) {
	if (image == NULL) {
		printf("Warning: NULL passed to Image_free");
		return;
	}
	free(image->data);
	free(image);
}

int Image_xy_to_i(const Image *image, const int x, const int y) {
	if (x < 0 || x > (int)image->width || y < 0 || y > (int)image->height) {
		_print_Image_error("Image_xy_to_i", "(x, y) not in range");
		return 0;
	}
	return (int)image->width*y + x;
}



#define GAUSS_THRESHOLD 0.01

void *_make_gaussian_kernel1d(double **out, int *radius, const double sigma) {
	const double two_sigma_2 = 2*sigma*sigma;
	int r = ceil(sqrt(-two_sigma_2*log(sigma*GAUSS_THRESHOLD)));
	double *kernel = malloc((2*r+1)*sizeof(double));
	
	double sum=0, val;
	int x;
	for (x=-r; x<=r; x++) {
		val = exp(-x*x/two_sigma_2);
		kernel[r+x] = val;
		sum += val;
	}
	int i;
	for (i=0; i<=2*r; i++) {
		kernel[i] /= sum;
	}
	
	*out = kernel;
	*radius = r;
}

Error gaussian_blur(Image **out, const Image *image, const double sigma) {
	double *kernel1d;
	int r;
	_make_gaussian_kernel1d(&kernel1d, &r, sigma);
	int w = image->width, h = image->height;
	Image *image_hor;
	Image_new(&image_hor, image->width, image->height, image->colortype);
	
	int x, y, x0, y0, mx, my;
	int i=0;
	double sum, val;
	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			val = 0.;
			if (x >= r && x+r < w) {
				for (x0 = -r; x0 <= r; x0++)
					val += image->data[i+x0]*kernel1d[r+x0];
			} else {
				sum = 0.;
				mx = _MIN(r, w-x-1);
				for (x0 = _MAX(-r, -x); x0 <= mx; x0++) {
					val += image->data[i+x0]*kernel1d[r+x0];
					sum += kernel1d[r+x0];
				}
				val /= sum;
			}
			image_hor->data[i++] = (unsigned char)val;
		}
	}
	
	Image *image_out;
	Image_new(&image_out, image->width, image->height, image->colortype);
	
	i=0;
	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			val = 0.;
			if (y >= r && y+r < h) {
				for (y0 = -r; y0 <= r; y0++)
					val += image_hor->data[i+w*y0]*kernel1d[r+y0];
			} else {
				sum = 0.;
				my = _MIN(r, h-y-1);
				for (y0 = _MAX(-r, -y); y0 <= my; y0++) {
					val += image_hor->data[i+w*y0]*kernel1d[r+y0];
					sum += kernel1d[r+y0];
				}
				val /= sum;
			}
			image_out->data[i++] = (unsigned char)val;
		}
	}
	
	Image_free(image_hor);
	*out = image_out;
	return 0;
}