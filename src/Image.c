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



Error Image_new(Image **out, const int width, const int height) {
	if (width < 0 || height < 0) {
		_print_Image_error("Image_new", "Negative image size");
		return 1;
	}
	Image *image = malloc(sizeof(Image));
	image->width = (unsigned)width;
	image->height = (unsigned)height;
	image->data = malloc((unsigned)width*(unsigned)height*sizeof(unsigned char));
	if (image->data == NULL) {
		_print_Image_error("Image_new", "Memory error");
		return 1;
	}
	*out = image;
	return 0;
}

Error Image_copy(Image **out, const Image *image) {
	Image *new_image;
	Error err = Image_new(&new_image, image->width, image->height);
	if (err != 0) return err;
	memcpy(new_image->data, image->data, image->width * image->height * sizeof(unsigned char));
	*out = new_image;
	return 0;
}

Error Image_open(Image **out, const char *filename) {
	Image *image = malloc(sizeof(Image));
	Error err = lodepng_decode_file(&image->data, &image->width, &image->height, filename, LCT_GREY, 8);
	_HANDLE_LODEPNG_ERR("Image_open", err)
	*out = image;
	return 0;
}

Error Image_save(const Image *image, const char *filename) {
	Error err = lodepng_encode_file(filename, image->data, image->width, image->height, LCT_GREY, 8);
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



Error DoubleImage_new(DoubleImage **out, const int width, const int height) {
	if (width < 0 || height < 0) {
		_print_Image_error("DoubleImage_new", "Negative image size");
		return 1;
	}
	DoubleImage *d_image = malloc(sizeof(DoubleImage));
	d_image->width = (unsigned)width;
	d_image->height = (unsigned)height;
	d_image->data = malloc((unsigned)width*(unsigned)height*sizeof(double));
	if (d_image->data == NULL) {
		_print_Image_error("DoubleImage_new", "Memory error");
		return 1;
	}
	*out = d_image;
	return 0;
}

Error DoubleImage_copy(DoubleImage **out, const DoubleImage *d_image) {
	DoubleImage *new_d_image;
	Error err = DoubleImage_new(&new_d_image, d_image->width, d_image->height);
	if (err != 0) return err;
	memcpy(new_d_image->data, d_image->data, d_image->width * d_image->height * sizeof(double));
	*out = new_d_image;
	return 0;
}

void DoubleImage_free(DoubleImage *d_image) {
	if (d_image == NULL) {
		printf("Warning: NULL passed to DoubleImage_free");
		return;
	}
	free(d_image->data);
	free(d_image);
}

int DoubleImage_xy_to_i(const DoubleImage *d_image, const int x, const int y) {
	if (x < 0 || x > (int)d_image->width || y < 0 || y > (int)d_image->height) {
		_print_Image_error("DoubleImage_xy_to_i", "(x, y) not in range");
		return 0;
	}
	return (int)d_image->width*y + x;
}

Error DoubleImage_vector_call(DoubleImage **out, double func(double, double), const DoubleImage *d_image1, const DoubleImage *d_image2) {
	if (d_image1->width != d_image2->width || d_image1->height != d_image2->height) {
		_print_Image_error("hypot_DoubleImage", "images should have same dimensions");
		return 1;
	}
	const int w = d_image1->width, h = d_image1->height;
	const int size = w*h;
	
	DoubleImage *d_image_out;
	Error err = DoubleImage_new(&d_image_out, w, h);
	if (err != 0) return err;
	
	int i;
	for (i=0; i<size; i++)
		d_image_out->data[i] = func(d_image1->data[i], d_image2->data[i]);
	
	*out = d_image_out;
	return 0;
}



Error Image_to_DoubleImage(DoubleImage **out, const Image *image) {
	DoubleImage *new_d_image;
	Error err = DoubleImage_new(&new_d_image, image->width, image->height);
	if (err != 0) return err;
	const unsigned size = image->width * image->height;
	unsigned i;
	for (i=0; i<size; i++)
		new_d_image->data[i] = (double)image->data[i];
	*out = new_d_image;
	return 0;
}

Error DoubleImage_to_Image(Image **out, const DoubleImage *d_image) {
	Image *new_image;
	Error err = Image_new(&new_image, d_image->width, d_image->height);
	if (err != 0) return err;
	const unsigned size = d_image->width * d_image->height;
	unsigned i;
	for (i=0; i<size; i++)
		new_image->data[i] = (unsigned char)fmax(0, fmin(255, d_image->data[i]));
	*out = new_image;
	return 0;
}



#define GAUSS_THRESHOLD 0.01

void *_make_gaussian_kernel1d(double **out, int *radius, const double sigma) {
	const double two_sigma_2 = 2*sigma*sigma;
	const int r = ceil(sqrt(-two_sigma_2*log(sigma*GAUSS_THRESHOLD)));
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

Error Image_gaussian_blur(Image *image, const double sigma) {
	double *kernel1d;
	int r;
	_make_gaussian_kernel1d(&kernel1d, &r, sigma);
	const int w = image->width, h = image->height;
	Image *image_hor;
	Error err = Image_new(&image_hor, image->width, image->height);
	if (err != 0) return err;
	
	int x, y, x0, y0, mx, my;
	int i=0;
	double sum, val;
	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			val = 0.;
			if (x >= r && x+r < w) {
				for (x0 = -r; x0 <= r; x0++)
					val += image->data[i+x0] * kernel1d[r+x0];
			} else {
				sum = 0.;
				mx = _MIN(r, w-x-1);
				for (x0 = _MAX(-r, -x); x0 <= mx; x0++) {
					val += image->data[i+x0] * kernel1d[r+x0];
					sum += kernel1d[r+x0];
				}
				val /= sum;
			}
			image_hor->data[i++] = (unsigned char)val;
		}
	}
	
	i=0;
	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			val = 0.;
			if (y >= r && y+r < h) {
				for (y0 = -r; y0 <= r; y0++)
					val += image_hor->data[i+w*y0] * kernel1d[r+y0];
			} else {
				sum = 0.;
				my = _MIN(r, h-y-1);
				for (y0 = _MAX(-r, -y); y0 <= my; y0++) {
					val += image_hor->data[i+w*y0] * kernel1d[r+y0];
					sum += kernel1d[r+y0];
				}
				val /= sum;
			}
			image->data[i++] = (unsigned char)val;
		}
	}
	
	Image_free(image_hor);
	return 0;
}

Error apply_kernel(DoubleImage *d_image, const int kernel_width, const int kernel_height, const double *kernel) {
	if (kernel_width%2 == 0 || kernel_height%2 == 0) {
		_print_Image_error("apply_kernel", "kernel dimensions should be odd numbers");
		return 1;
	}
	
	DoubleImage *d_image_copy;
	Error err = DoubleImage_copy(&d_image_copy, d_image);
	if (err != 0) return err;
	
	const int rw = kernel_width/2, rh = kernel_height/2;
	const int w = d_image->width, h = d_image->height;
	
	int x, y, x0, y0;
	int i=0;
	double val;
	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			val = 0.;
			if (x >= rw && x+rw < w && y >= rh && y+rh < h) {
				for (y0 = -rh; y0 <= rh; y0++) {
					for (x0 = -rw; x0 <= rw; x0++)
						val += d_image_copy->data[i + w*y0 + x0] * kernel[(y0+rh)*kernel_width + x0+rw];
				}
			}
			d_image->data[i++] = val;
		}
	}
	
	DoubleImage_free(d_image_copy);
	return 0;
}
