#include <stdio.h>
#include "Image.h"

int main() {
	Image *image;
	Image_open(&image, "images/skull_1.png", LCT_GREY);
	
	int x, y;
	for (y=100; y<200; y++) {
		for (x=150; x<400; x++) {
			image->data[Image_xy_to_i(image, x, y)] = 128;
		}
	}
	
	Image_save(image, "images/skull_1_1.png");
	Image_free(image);
	
	return 0;
}