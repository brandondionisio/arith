/*************************************************************
 *
 *                     convert_rgb_cv.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the functions for converting from rgb
 *              color space to component video color space and vice versa.
 *              
 **************************************************************/

#include <stdint.h>
#include <stdlib.h>

#include "a2blocked.h"
#include "uarray2b.h"
#include "bitpack.h"
#include "assert.h"
#include "pnm.h"
#include "math.h"
#include "mem.h"
#include "convert_types.h"
#include "structs.c"

/**************** apply_convert_rgb_to_cv ****************
 * 
 * apply function for converting pixels from rgb color space to cv color space
 *
 * Parameters:
 *      int col:    column index of an rgb pixel
 *      int row:    row index of an rgb pixel
 *      A2 array:   array to be transformed
 *      void *elem: pointer to an rgb pixel
 *      void *cl:   closure pointer to struct of new cv array, methods object,
 *                  and pixel denominator
 * Returns:
 *      None
 * Expects:
 *      Row or column of rgb space array is not out of bounds of the new cv
 *      space array
 *
 ********************************************/
void apply_rgb_to_cv(int col, int row, A2 array, void *elem, void *cl)
{
        struct CV_RGB_Closure *closure = cl;

        /* in bounds */
        if ((col > closure->methods->width(closure->array)) || 
            (row > closure->methods->height(closure->array))) {
                return;
        }

        Pnm_ybr new_pixel = closure->methods->at(closure->array, col, row);
        Pnm_rgb rgb = elem;

        float r = (rgb->red * 1.0) / (closure->denominator);
        float g = (rgb->green * 1.0) / (closure->denominator);
        float b = (rgb->blue * 1.0) / (closure->denominator);

        new_pixel->y = 0.299 * r + 0.587 * g + 0.114 * b;
        new_pixel->pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        new_pixel->pr = 0.5 * r - 0.418688 * g - 0.081312 * b;

        (void)array;
}

/**************** rgb_to_cv ****************
 * 
 * Given an Pnm_ppm object containing an rgb pixel image, returns a new A2
 * image that contains the corresponding pixels in cv color space.
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
A2 rgb_to_cv(Pnm_ppm rgb_image, A2Methods_mapfun *map, A2Methods_T methods)
{
        assert(rgb_image != NULL);
        assert(map != NULL);
        assert(methods != NULL);

        int width = rgb_image->width;
        int height = rgb_image->height;

        A2 pbr_pixels = methods->new_with_blocksize(width, height,
                                                    sizeof(struct Pnm_ybr), 2);

        CV_RGB_Closure closure;
        NEW(closure);

        closure->array = pbr_pixels;
        closure->methods = methods;
        closure->denominator = rgb_image->denominator;

        map(rgb_image->pixels, apply_rgb_to_cv, closure);

        FREE(closure);

        Pnm_ppmfree(&rgb_image);

        return pbr_pixels;
}

/**************** apply_cv_to_rgb ****************
 * 
 * apply function for converting pixels from cv color space to rgb color space
 *
 * Parameters:
 *      int col:    column index of an cv pixel
 *      int row:    row index of an cv pixel
 *      A2 array:   array to be transformed
 *      void *elem: pointer to an cv pixel
 *      void *cl:   closure pointer to struct of new rgb array, methods object,
 *                  and pixel denominator
 * Returns:
 *      None
 * Expects:
 *      Row or column of rgb space array is not out of bounds of the new rgb
 *      space array
 *
 ********************************************/
void apply_cv_to_rgb(int col, int row, A2 array, void *elem, void *cl)
{       
        CV_RGB_Closure closure = cl;

        if ((col > closure->methods->width(closure->array)) ||
            (row > closure->methods->height(closure->array))) {
                return;
        }

        Pnm_rgb new_pixel = closure->methods->at(closure->array, col, row);
        Pnm_ybr cvc = elem;
        unsigned d = closure->denominator;

        new_pixel->red = d * set_range(1.0 * cvc->y + 0.0 * cvc->pb + 1.402 *
                                                                      cvc->pr);
        new_pixel->green = d * set_range(1.0 * cvc->y - 0.344136 * cvc->pb -
                                                           0.714136 * cvc->pr);
        new_pixel->blue = d * set_range(1.0 * cvc->y + 1.772 * cvc->pb + 0.0 *
                                                                      cvc->pr);

        (void)array;
}

/**************** cv_to_rgb ****************
 * 
 * Given an A2 image in cv color space, returns a new Pnm_ppm image object
 * that contains the corresponding image information in rgb color space.
 *
 * Parameters:
 *      A2 cv_image:           pointer to the image pixels cv color space to
 *                             be converted to rgb color space
 *      A2Methods_mapfun *map: pointer to the mapping function with which we
 *                             will parse through the cv image
 *      A2Methods_T methods:   the methods object with which we will use to
 *                             create the new Pnm_ppm image for the cv color
 *                             space image.
 * Returns:
 *      New Pnm_ppm image object for the image converted into rgb color space
 * Expects:
 *      passed-in cv_image is not NULL (throws a CRE if not)
 *      passed-in map is not NULL (throws a CRE if not)
 *      passed-in methods is not NULL (throws a CRE if not)
 *
 ********************************************/
Pnm_ppm cv_to_rgb(A2 cv_image, A2Methods_mapfun *map, A2Methods_T methods)
{
        assert(cv_image != NULL);
        assert(map != NULL);
        assert(methods != NULL);

        int width = methods->width(cv_image);
        int height = methods->height(cv_image);
        unsigned denominator = 255;

        Pnm_ppm rgb_image;
        NEW(rgb_image);

        rgb_image->width = width;
        rgb_image->height = height;
        rgb_image->denominator = denominator;
        rgb_image->methods = methods;
        rgb_image->pixels = methods->new(width, height,
                                                       sizeof(struct Pnm_rgb));

        CV_RGB_Closure closure;
        NEW(closure);

        closure->array = rgb_image->pixels;
        closure->methods = methods;
        closure->denominator = denominator;

        map(cv_image, apply_cv_to_rgb, closure);

        FREE(closure);

        return rgb_image;
}