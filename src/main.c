#include <stdio.h>
#include <math.h>
#include "Image.h"

double Sobel_kernel_hor[] = {
	-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1,
};
double Sobel_kernel_ver[] = {
	-1,-2,-1,
	 0, 0, 0,
	 1, 2, 1,
};

int main() {
	Image *image;
	Image_open(&image, "images/skull_1.png");
	
	Image_gaussian_blur(image, 2);
	
	DoubleImage *grad_hor, *grad_ver;
	Image_to_DoubleImage(&grad_hor, image);
	Image_to_DoubleImage(&grad_ver, image);
	apply_kernel(grad_hor, 3, 3, Sobel_kernel_hor);
	apply_kernel(grad_ver, 3, 3, Sobel_kernel_ver);
	
	DoubleImage *grad;
	DoubleImage_vector_call(&grad, hypot, grad_hor, grad_ver);
	
	DoubleImage_free(grad_hor);
	DoubleImage_free(grad_ver);
	
	Image_free(image);
	DoubleImage_to_Image(&image, grad);
	DoubleImage_free(grad);
	
	Image_save(image, "images/skull_1_grad_20.png");
	Image_free(image);
	
	return 0;
}