<<<<<<< HEAD
/*************************************************************
 *
 *                     ppmdiff.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the ppmdiff program which takes in two
 *              images and outputs a number from 0-1 representing their
 *              similarity.
 *              
 **************************************************************/

=======
>>>>>>> master
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "uarray2.h"

int main(int argc, char *argv[])
{
        if (argc != 3) {
                fprintf(stderr, "Format: ./ppmdiff [image 1] [image 2]");
        }

        char *filename_1 = argv[1];
<<<<<<< HEAD
        char *filename_2 = argv[2];
=======
        // printf("filename 1 is: %s\n", filename_1);
        char *filename_2 = argv[2];
        // printf("filename 2 is: %s\n", filename_2);
>>>>>>> master
        
        FILE *file_1 = fopen(filename_1, "r");
        if (file_1 == NULL) {
                fprintf(stderr, "Error: Could not open file %s\n", filename_1);
                exit(EXIT_FAILURE);
        }
        FILE *file_2 = fopen(filename_2, "r");
        if (file_2 == NULL) {
                fprintf(stderr, "Error: Could not open file %s\n", filename_2);
                exit(EXIT_FAILURE);
        }
        
        Pnm_ppm output_1, output_2;

        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        output_1 = Pnm_ppmread(file_1, methods);
        output_2 = Pnm_ppmread(file_2, methods);

        int width_1 = output_1->width;
        int height_1 = output_1->height;

        int width_2 = output_2->width;
        int height_2 = output_2->height;

        int width_diff = width_2 - width_1;
        int height_diff = height_2 - height_1;

        int width, height;
        float sum = 0;

        if (abs(width_diff) > 1 || abs(height_diff > 1)) {
                fprintf(stderr, "Images must differ in width and height by at most 1");
                return EXIT_SUCCESS;
        }

        if (width_diff > 0) {
                width = width_1;
        } else {
                width = width_2;
        }

        if (height_diff > 0) {
                height = height_1;
        } else {
                height = height_2;
        }

        A2Methods_UArray2 pixels_1 = output_1->pixels;
        A2Methods_UArray2 pixels_2 = output_2->pixels;

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        assert(methods->at(pixels_1, col, row) != NULL);
                        assert(methods->at(pixels_2, col, row) != NULL);

<<<<<<< HEAD
                        struct Pnm_rgb rgb_1 = *(struct Pnm_rgb *)methods->at
                                                          (pixels_1, col, row);
                        struct Pnm_rgb rgb_2 = *(struct Pnm_rgb *)methods->at
                                                          (pixels_2, col, row);

                        float red_diff = ((float)rgb_2.red) / 255.0 - 
                                                    ((float)rgb_1.red) / 255.0;
                        float blue_diff = ((float)rgb_2.blue) / 255.0 - 
                                                   ((float)rgb_1.blue) / 255.0;
                        float green_diff = ((float)rgb_2.green) / 255.0 -
                                                  ((float)rgb_1.green) / 255.0;

                        sum += (pow(red_diff, 2) + pow(blue_diff, 2) +
                                                           pow(green_diff, 2));
=======
                        struct Pnm_rgb rgb_1 = *(struct Pnm_rgb *)methods->at(pixels_1, col, row);
                        struct Pnm_rgb rgb_2 = *(struct Pnm_rgb *)methods->at(pixels_2, col, row);

                        float red_diff = ((float)rgb_2.red) / 255.0 - ((float)rgb_1.red) / 255.0;
                        float blue_diff = ((float)rgb_2.blue) / 255.0 - ((float)rgb_1.blue) / 255.0;
                        float green_diff = ((float)rgb_2.green) / 255.0 - ((float)rgb_1.green) / 255.0;

                        sum += (pow(red_diff, 2) + pow(blue_diff, 2) + pow(green_diff, 2));
>>>>>>> master
                } 
        }

        float E = (float)sqrt((double)sum / (double)(3.0 * width * height));

        printf("%.4f\n", (float)(E));

        Pnm_ppmfree(&output_1);
        Pnm_ppmfree(&output_2);

        fclose(file_1);
        fclose(file_2);
}