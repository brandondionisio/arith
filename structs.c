/**************************************************************
 *
 *                     structs.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the structs used within the 40image
 *              program. These structs are used for the closure in the mapping
 *              functions with rgb and cv conversions and holding the y, pb,
 *              and pr values for pixels in cv color space.
 *              
 **************************************************************/

#ifndef STRUCTS_H
#define STRUCTS_H

#include "a2methods.h"

typedef A2Methods_UArray2 A2;

/**************** cv_rgb_closure ***************
 * 
 * Closure struct that stores useful data required for the closure portion of
 * our apply function.
 *
 * Attributes:
 *      A2 array:             the array of pixels
 *      A2Methods_T methods:  choice of methods for the image
 *      unsigned denominator: the denominator of the image
 *
 ********************************************/
typedef struct CV_RGB_Closure {
        A2 array;
        A2Methods_T methods;
        unsigned denominator;
} *CV_RGB_Closure;

/****************** Pnm_ybr ******************
 * 
 * Struct that stores the y, pb, and pr values of an image.
 *
 * Attributes:
 *      float y:  the y value of a pixel
 *      float pb: the pb value of a pixel
 *      float pr: the pr value of a pixel
 *
 ********************************************/
typedef struct Pnm_ybr {
        float y;
        float pb;
        float pr;
} *Pnm_ybr;

#endif