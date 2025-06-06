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
	return 1;\
}



Error Image_open(Image **out, const char *filename, const LodePNGColorType colortype) {
	Image *image = malloc(sizeof(Image));
	unsigned err = lodepng_decode_file(&image->data, &image->width, &image->height, filename, colortype, 8);
	_HANDLE_LODEPNG_ERR("Image_open", err)
	*out = image;
	return 0;
}

Error Image_save(const Image *image, const char *filename) {
	unsigned err = lodepng_encode_file(filename, image->data, image->width, image->height, image->colortype, 8);
	_HANDLE_LODEPNG_ERR("Image_save", err)
	return 0;
}

unsigned Image_xy_to_i(const Image *image, const unsigned x, const unsigned y) {
	if (x > image->width || y > image->height)
		_print_Image_error("Image_xy_to_i", "(x, y) not in range");
	return image->width*y + x;
}