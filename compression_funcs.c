/*************************************************************
 *
 *                     compression_funcs.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the functions for image manipulations
 *              used in the compression part of our program. The functions in
 *              this file are used to trim a given image when the length or
 *              width are odd and print out the packed 32-bit words at the
 *              end of compression.
 *              
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pnm.h"
#include "a2methods.h"
#include "seq.h"

#define A2 A2Methods_UArray2

/**************** trim_image ****************
 * 
 * Given a pointer to a Pnm_ppm image object, trims down an odd width or height
 * by one to make an even-dimensional image. Also, updates the passed-in width
 * and height variables.
 *
 * Parameters:
 *      Pnm_ppm rgb_image:     pointer to the information in rgb color space to
 *                             be converted to cv color space
 *      A2Methods_mapfun *map: pointer to the mapping function with which we
 *                             will parse through the rgb image
 *      A2Methods_T methods:   the methods object with which we will use to
 *                             create the new A2 array for cv color space
 *                             pixels.
 * Returns:
 *      Then new cv color space image
 * Expects:
 *      passed-in rgb_image is not NULL (throws a CRE if not)
 *      passed-in map is not NULL (throws a CRE if not)
 *      passed-in methods is not NULL (throws a CRE if not)
 *
 ********************************************/
void trim_image(Pnm_ppm image, A2Methods_T methods, int *width, int *height)
{
        if ((*width % 2) == 1) {
                (*width) -= 1;
                image->width = *(int *)width;
        }    

        if ((*height % 2) == 1) {
                (*height) -= 1;
                image->height = *(int *)height;
        }

        A2 trimmed_image = methods->new_with_blocksize(*(int *)width,
                                    *(int *)height, sizeof(struct Pnm_rgb), 2);

        assert(trimmed_image != NULL);

        for (int row = 0; row < *(int *)height; row++) {
                for (int col = 0; col < *(int *)width; col++) {
                        Pnm_rgb new_pixel = methods->at(trimmed_image, col,
                                                                          row);
                        Pnm_rgb old_pixel = methods->at(image->pixels, col,
                                                                          row);

                        new_pixel->red = old_pixel->red;
                        new_pixel->green = old_pixel->green;
                        new_pixel->blue = old_pixel->blue;
                }
        }

        methods->free(&image->pixels);

        image->pixels = trimmed_image;
}

/**************** print_image ****************
 * 
 * Given a pointer to the sequence containing all the 32-bit words, prints out
 * all of its contents.
 *
 * Parameters:
 *      Seq_T words: pointer to a sequence of 32-bit packed words
 *      int width:   integer representing the width of the compressed image
 *      int height:  integer representing the height of the compressed image
 * Returns:
 *      None
 * Expects:
 *      passed-in sequence of words is not NULL (throws a CRE if not)
 *
 ********************************************/
void print_image(Seq_T words, int width, int height) 
{
        assert(words != NULL);
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        for (int i = 0; i < Seq_length(words); i++) {
                uint64_t *word = (uint64_t *)Seq_get(words, i);
                uint64_t codeword = *word;
                
                for (int j = 24; j >= 0; j = j - 8) {
                        unsigned char byte = (codeword >> j);
                        putchar(byte);
                } 
                
                /* Free dynamically allocated memory */
                free(word);
        }
}