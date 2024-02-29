#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compress40.h"
#include "bitpack.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "uarray2.h"
#include "assert.h"
#include "pnm.h"
#include "mem.h"
 
typedef A2Methods_UArray2 A2;

A2 rgb_to_cvc(Pnm_ppm rgb_image, A2Methods_mapfun *map, A2Methods_T methods);
void apply_rgb_cvc(int col, int row, A2 array, void *elem, void *cl);
void decompress_test(Pnm_ppm pixmap);

typedef struct data {
        A2 array;
        A2Methods_T methods;
        unsigned denominator;
} *data;

typedef struct Pnm_ybr {
        float y;
        float pb;
        float pr;
} *Pnm_ybr;

extern void compress40(FILE *input) 
{       
        assert(input != NULL);

        A2Methods_T methods = uarray2_methods_blocked; 
        
        A2Methods_mapfun *map = methods->map_default;
        
        assert(map);

        assert(methods != NULL);

        Pnm_ppm image = Pnm_ppmread(input, methods);
        
        assert(image != NULL);
        
        int width = (int)image->width;
        int height = (int)image->height;

        assert(width > 1 && height > 1);

        if ((width % 2) == 1) {
                width -= 1;
                image->width = width;
        }    

        if ((height % 2) == 1) {
                height -= 1;
                image->height = height;
        }

        A2 trimmed_image = methods->new_with_blocksize(width, height, sizeof(struct Pnm_rgb), 2);

        assert(trimmed_image != NULL);

        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        Pnm_rgb new_pixel = methods->at(trimmed_image, col, row);
                        Pnm_rgb old_pixel = methods->at(image->pixels, col, row);

                        new_pixel->red = old_pixel->red;
                        new_pixel->green = old_pixel->green;
                        new_pixel->blue = old_pixel->blue;
                }
        }

        methods->free(&(image->pixels));

        image->pixels = trimmed_image;

        A2 pbr_pixels = rgb_to_cvc(image, map, methods);

        /* TODO: change the size to 32 bit word struct */
        //A2 compressed_image = image->methods->new(width / 2, height / 2, 10);
        //(void) compressed_image;

        /* image->methods->map(pixels, apply, cl); */

        (void)pbr_pixels;

        decompress_test(image);
}

A2 rgb_to_cvc(Pnm_ppm rgb_image, A2Methods_mapfun *map, A2Methods_T methods)
{
        int width = rgb_image->width;
        int height = rgb_image->height;

        A2 pbr_pixels = methods->new_with_blocksize(width, height, sizeof(struct Pnm_ybr), 2);

        data closure;
        NEW(closure);

        closure->array = pbr_pixels;
        closure->methods = methods;
        closure->denominator = rgb_image->denominator;

        map(rgb_image->pixels, apply_rgb_cvc, closure);

        return pbr_pixels;
}


void apply_rgb_cvc(int col, int row, A2 array, void *elem, void *cl)
{       
        Pnm_ybr new_pixel;
        NEW(new_pixel);
        struct Pnm_rgb rgb = *(struct Pnm_rgb *)elem;
        data closure = cl;

        float y = 0.299 * rgb.red + 0.587 * rgb.green + 0.114 * rgb.blue;
        float pb = -0.168736 * rgb.red - 0.331264 * rgb.green + 0.5 * rgb.blue;
        float pr = 0.5 * rgb.red - 0.418688 * rgb.green - 0.081312 * rgb.blue;

        new_pixel->y = y;
        new_pixel->pb = pb;
        new_pixel->pr = pr;

        *(Pnm_ybr *)closure->methods->at(closure->array, col, row) = new_pixel;

        (void)array;
}

void decompress_test(Pnm_ppm pixmap)
{
        Pnm_ppmwrite(stdout, pixmap);
}


extern void decompress40(FILE *input) 
{
        (void)input;
}

