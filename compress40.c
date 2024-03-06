#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "uarray2.h"
#include "compress40.h"
#include "bitpack.h"
#include "assert.h"
#include "pnm.h"
#include "mem.h"
#include "arith40.h"
#include "math.h"
#include "data.c"
#include "seq.h"

#define SMALL_THRESHOLD = 0.3
#define A_WIDTH 9
#define BCD_WIDTH 5
#define PRB_WIDTH 4

static float set_range(float num);
void trim_image(Pnm_ppm *image, A2Methods_T methods, int *width, int *height);
A2 rgb_to_cvc(Pnm_ppm rgb_image, A2Methods_mapfun *map, A2Methods_T methods);
void apply_rgb_cvc(int col, int row, A2 array, void *elem, void *cl);
Pnm_ppm cvc_to_rgb(A2 cv_image, A2Methods_mapfun *map, A2Methods_T methods);
void apply_cvc_rgb(int col, int row, A2 array, void *elem, void *cl);
void decompress_test(A2 cvc_pixels, A2Methods_mapfun *map, A2Methods_T methods);
Seq_T get_seq(A2 image, A2Methods_T methods);
uint64_t encode_9_bit_float(float a_val);
int64_t encode_5_bit_float(float bcd_val);
void print_image(Seq_T words, int width, int height);

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

        trim_image(&image, methods, &width, &height);

        A2 pbr_pixels = rgb_to_cvc(image, map, methods);
        
        Seq_T words = get_seq(pbr_pixels, methods);        

        //decompress_test(pbr_pixels, map, methods);

        print_image(words, width, height);

        methods->free(&pbr_pixels);
        Seq_free(&words);
}

void trim_image(Pnm_ppm *image, A2Methods_T methods, int *width, int *height)
{
        if ((*width % 2) == 1) {
                (*width) -= 1;
                (*image)->width = *(int *)width;
        }    

        if ((*height % 2) == 1) {
                (*height) -= 1;
                (*image)->height = *(int *)height;
        }

        A2 trimmed_image = methods->new_with_blocksize(*(int *)width, *(int *)height, sizeof(struct Pnm_rgb), 2);

        assert(trimmed_image != NULL);

        for (int row = 0; row < *(int *)height; row++) {
                for (int col = 0; col < *(int *)width; col++) {
                        Pnm_rgb new_pixel = methods->at(trimmed_image, col, row);
                        Pnm_rgb old_pixel = methods->at((*image)->pixels, col, row);

                        new_pixel->red = old_pixel->red;
                        new_pixel->green = old_pixel->green;
                        new_pixel->blue = old_pixel->blue;
                }
        }

        methods->free(&(*image)->pixels);

        (*image)->pixels = trimmed_image;
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

        FREE(closure);

        Pnm_ppmfree(&rgb_image);

        return pbr_pixels;
}

void apply_rgb_cvc(int col, int row, A2 array, void *elem, void *cl)
{       
        struct data *closure = cl;

        if ((col > closure->methods->width(closure->array)) || 
                (row > closure->methods->height(closure->array))){
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

Pnm_ppm cvc_to_rgb(A2 cv_image, A2Methods_mapfun *map, A2Methods_T methods)
{
        int width = methods->width(cv_image);
        int height = methods->height(cv_image);
        unsigned denominator = 255;

        Pnm_ppm rgb_image;
        NEW(rgb_image);
        
        rgb_image->width = width;
        rgb_image->height = height;
        rgb_image->denominator = denominator;
        rgb_image->methods = methods;
        rgb_image->pixels = methods->new(width, height, sizeof(struct Pnm_rgb));

        data closure;
        NEW(closure);

        closure->array = rgb_image->pixels;
        closure->methods = methods;
        closure->denominator = denominator;

        map(cv_image, apply_cvc_rgb, closure);

        FREE(closure);

        return rgb_image;
}

void apply_cvc_rgb(int col, int row, A2 array, void *elem, void *cl)
{       
        data closure = cl;

        Pnm_rgb new_pixel = closure->methods->at(closure->array, col, row);
        Pnm_ybr cvc = elem;
        unsigned d = closure->denominator;

        new_pixel->red = d * set_range(1.0 * cvc->y + 0.0 * cvc->pb + 1.402 * cvc->pr);
        new_pixel->green = d * set_range(1.0 * cvc->y - 0.344136 * cvc->pb - 0.714136 * cvc->pr);
        new_pixel->blue = d * set_range(1.0 * cvc->y + 1.772 * cvc->pb + 0.0 * cvc->pr);

        (void)array;
}

static float set_range(float num) 
{
        if (num < 0) {
                return 0.0;
        } else if (num > 1) {
                return 1.0;
        } else {
                return num;
        }       
}

void decompress_test(A2 cvc_pixels, A2Methods_mapfun *map, A2Methods_T methods)
{
        Pnm_ppm new_image = cvc_to_rgb(cvc_pixels, map, methods); 
        Pnm_ppmwrite(stdout, new_image);
        Pnm_ppmfree(&new_image);
}

extern void decompress40(FILE *input) 
{
        (void)input;
}

Seq_T get_seq(A2 image, A2Methods_T methods)
{
        float average_pb, average_pr, a, b, c, d;
        int width = methods->width(image);
        int height = methods->height(image);

        /* initialize the sequence containing 32-bit words */
        Seq_T words = Seq_new(1000);

        for (int row = 0; row < height; row += 2) {
                for (int col = 0; col < width; col += 2) {
                        Pnm_ybr ybr1 = methods->at(image, col, row);
                        Pnm_ybr ybr2 = methods->at(image, col + 1, row);
                        Pnm_ybr ybr3 = methods->at(image, col, row + 1);
                        Pnm_ybr ybr4 = methods->at(image, col + 1, row + 1);
                        
                        average_pb = (ybr1->pb + ybr2->pb + ybr3->pb + ybr4->pb) / 4;
                        average_pr = (ybr1->pr + ybr2->pr + ybr3->pr + ybr4->pr) / 4;

                        uint64_t unsign_pb = Arith40_index_of_chroma(average_pb);
                        uint64_t unsign_pr = Arith40_index_of_chroma(average_pr);

                        a = (ybr4->y + ybr3->y + ybr2->y + ybr1->y) / 4.0;
                        b = (ybr4->y + ybr3->y - ybr2->y - ybr1->y) / 4.0;
                        c = (ybr4->y - ybr3->y + ybr2->y - ybr1->y) / 4.0;
                        d = (ybr4->y - ybr3->y - ybr2->y + ybr1->y) / 4.0;

                        uint64_t u_a = encode_9_bit_float(a);
                        int64_t u_b = encode_5_bit_float(b);
                        int64_t u_c = encode_5_bit_float(c);
                        int64_t u_d = encode_5_bit_float(d);

                        uint64_t word = 0;
                        word = Bitpack_newu(word, PRB_WIDTH, 0, unsign_pr);
                        word = Bitpack_newu(word, PRB_WIDTH, PRB_WIDTH, unsign_pb);
                        word = Bitpack_news(word, BCD_WIDTH, 2 * PRB_WIDTH, u_d);
                        word = Bitpack_news(word, BCD_WIDTH, BCD_WIDTH + 2 * PRB_WIDTH, u_c);
                        word = Bitpack_news(word, BCD_WIDTH, 2 * BCD_WIDTH + 2 * PRB_WIDTH, u_b);
                        word = Bitpack_newu(word, A_WIDTH, 3 * BCD_WIDTH + 2 * PRB_WIDTH, u_a);

                        /* put in the sequence */
                        Seq_addhi(words, &word);
                }
        }
        /* return sequence */
        return words;
}

uint64_t encode_9_bit_float(float a_val)
{
        a_val *= 511;
        int rounded_int = (int)(a_val + .5);

        if (rounded_int < 0) {
                rounded_int = 0;
        } else if (rounded_int > 511) {
                rounded_int = 511;
        }
        return (uint64_t)rounded_int;
}

int64_t encode_5_bit_float(float bcd_val)
{
        /* range of -16 < bcd_val < 16 */
        bcd_val *= (16 / 0.3);

        /* +.5 rounds to nearest integer */
        int64_t rounded_int = (int64_t)(bcd_val + 0.5);

        if (rounded_int < -16) {
                rounded_int = -16;
        } else if (rounded_int > 16) {
                rounded_int = 16;
        }

        return rounded_int;
}

void print_image(Seq_T words, int width, int height) 
{
        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        for (int i = 0; i < Seq_length(words); i++) {
                uint64_t *word = Seq_get(words, i);
                uint32_t codeword = (uint32_t)(*word);
                
                for (int j = 24; j >= 0; j = j - 8) {
                        unsigned char byte = (codeword >> j);
                        putchar(byte);
                }
        }
}

// ((Pnm_ybr)methods->at(image, col, row))->pb = average_pb;
                        // ((Pnm_ybr)methods->at(image, col + 1, row))->pb = average_pb;
                        // ((Pnm_ybr)methods->at(image, col, row + 1))->pb = average_pb;
                        // ((Pnm_ybr)methods->at(image, col + 1, row + 1))->pb = average_pb;
                        // ((Pnm_ybr)methods->at(image, col, row))->pr = average_pr;
                        // ((Pnm_ybr)methods->at(image, col + 1, row))->pr = average_pr;
                        // ((Pnm_ybr)methods->at(image, col, row + 1))->pr = average_pr;
                        // ((Pnm_ybr)methods->at(image, col + 1, row + 1))->pr = average_pr;