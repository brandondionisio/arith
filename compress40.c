/*************************************************************
 *
 *                     compress40.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the functions for the compress40 and
 *              decompress40 functions, which are the main drivers of the
 *              compression and decompression part of this program.
 *              
 **************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "compress40.h"
#include "convert_rgb_cv.c"
#include "compression_funcs.c"
#include "word_operations.h"

/**************** compress40 ****************
 * 
 * Main driver for the compression function. Given a pointer to an input file,
 * reads in the image contents and compresses the image into 32-bit words.
 *
 * Parameters:
 *      FILE *input: pointer to an input file containing the contents of the
 *                   image to be compressed
 * Returns:
 *      none
 * Expects:
 *      passed-in pointer to the file is not NULL (throws a CRE if not)
 *      initialized methods object is not NULL (throws a CRE if not)
 *      map function from methods is not NULL (throws a CRE if not)
 *      read-in Pnm_ppm image object is not NULL (throws a CRE if not)
 *      width and height of the read-in Pnm_ppm image object is not NULL
 *      (throws a CRE if not)
 *
 ********************************************/
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

        trim_image(image, methods, &width, &height);

        A2 cv_pixels = rgb_to_cv(image, map, methods);
        
        Seq_T words = get_seq(cv_pixels, methods);
        
        print_image(words, width, height);   

        methods->free(&cv_pixels);
        Seq_free(&words);
}

/**************** decompress40 ****************
 * 
 * Main driver for the decompression function. Given a pointer to an input
 * file, reads in the 32-bit words for a compressed image, and decompresses
 * the image so that it can be outputted.
 *
 * Parameters:
 *      FILE *input: pointer to an input file containing the 32-bit words for
 *                   a compressed image.
 * Returns:
 *      none
 * Expects:
 *      passed-in pointer to the file is not NULL (throws a CRE if not)
 *      we read in two arguments (width & height) from the file header (throws
 *      a CRE if not)
 *      the next character after the file header is a newline (throws a CRE if
 *      not)
 *
 ********************************************/
extern void decompress40(FILE *input)
{
        assert(input != NULL);

        unsigned width, height;
        
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                                              &width, &height);

        assert(read == 2);
        
        int c = getc(input);
        
        assert(c == '\n');

        A2Methods_T methods = uarray2_methods_blocked;

        A2Methods_mapfun *map = methods->map_default;

        A2 new_image = methods->new_with_blocksize(width, height,
                                                    sizeof(struct Pnm_ybr), 2);

        /* read in words from file */
        uint64_t word = 0;
        uint64_t one_byte;

        for (int row = 0; row < (int)height; row += 2) {
                for (int col = 0; col < (int)width; col += 2) {
                        /* Bytes are written to the disk in big-endian order */
                        for (int w = 24; w >= 0; w = w - 8){
                                one_byte = getc(input);
                                one_byte = one_byte << w;
                                word = word | one_byte;
                        }

                        unpack_word(word, col, row, methods, new_image);
                        
                        word = 0;
                }
        }

        Pnm_ppm decompressed_image = cv_to_rgb(new_image, map, methods);

        Pnm_ppmwrite(stdout, decompressed_image);

        free_image(new_image, methods);

        Pnm_ppmfree(&decompressed_image);
}