#include <stdio.h>
#include "Image.h"

int main() {
	Image *image;
	Image_open(&image, "images/skull_1.png", LCT_GREY);
	
	Image *image_blurred;
	
	gaussian_blur(&image_blurred, image, 3);
	Image_save(image_blurred, "images/skull_1_blur.png");
	Image_free(image_blurred);
	
	Image_free(image);
	
	return 0;
}