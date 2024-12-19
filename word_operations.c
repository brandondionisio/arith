/**************************************************************
 *
 *                     word_operations.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the functions for the transformations
 *              involving the 32-bit words. The functions in this file are
 *              used to add packed words to the sequence to print out, unpack
 *              words read-in from the input file into a 2d array, and free the
 *              words inside the 2d array once we are finished with them.
 *              
 **************************************************************/

#include "word_operations.h"

/* bit width of the a value */
#define A_WIDTH 6

/* bit width of the b, c, and d values */
#define BCD_WIDTH 6

/* bit width of the pr and pb values */
#define PRB_WIDTH 4

/* intial length for the sequence to hold the 32-bit words */
#define INITIAL_SEQ_LENGTH 1000

/**************** unpack_word ****************
 * 
 * given a 32-bit word and a col and row index, unpacks that word into
 * its corresponding y1, y2, y3, y4, pr, and pb values to create the four
 * pixel block to insert in the given A2 new image array.
 *
 * Parameters:
 *      uint64_t word:       a 32-bit word of type uint64_t
 *      int col:             column index for the 4 unpacked pixels to go into
 *      int row:             row index for the 4 unpacked pixels to go into
 *      A2Methods_T methods: methods object for inserting the new pixels
 *      A2 new_image:        pointer to an A2 image object that will store the
 *                           new pixels unpacked from the words
 * Returns:
 *      None
 * Expects:
 *      passed-in methods object is not NULL (throws a CRE if not)
 *      passed-in new_image object is not NULL (throws a CRE if not)
 *
 ********************************************/
void unpack_word(uint64_t word, int col, int row, A2Methods_T methods, A2 new_image) {
        assert(methods != NULL);
        assert(new_image != NULL);

        /* intialize unpacked variables */
        uint64_t pb_unsigned, pr_unsigned, a_unsigned;
        int64_t b_signed, c_signed, d_signed;
        float pr, pb, a, b, c, d;

        /* get all integer representations of the variables from the word */
        pr_unsigned = Bitpack_getu(word, PRB_WIDTH, 0);
        pb_unsigned = Bitpack_getu(word, PRB_WIDTH, PRB_WIDTH);
        d_signed = Bitpack_gets(word, BCD_WIDTH, 2 * PRB_WIDTH);
        c_signed = Bitpack_gets(word, BCD_WIDTH, 2 * PRB_WIDTH + BCD_WIDTH);
        b_signed = Bitpack_gets(word, BCD_WIDTH, 2 * PRB_WIDTH + 2 * BCD_WIDTH);
        a_unsigned = Bitpack_getu(word, A_WIDTH, 2 * PRB_WIDTH + 3 * BCD_WIDTH);

        /* obtain the floats from the integers */
        pr = Arith40_chroma_of_index(pr_unsigned);
        pb = Arith40_chroma_of_index(pb_unsigned);
        d = bcd_int_to_float(d_signed);
        c = bcd_int_to_float(c_signed);
        b = bcd_int_to_float(b_signed);
        a = a_int_to_float(a_unsigned);

        /* intialize the four pixels from the a, b, c, d, pb, and pr values */
        Pnm_ybr pixel1 = get_new_pixel(pr, pb, a - b - c + d);
        Pnm_ybr pixel2 = get_new_pixel(pr, pb, a - b + c - d);
        Pnm_ybr pixel3 = get_new_pixel(pr, pb, a + b - c + d);
        Pnm_ybr pixel4 = get_new_pixel(pr, pb, a + b + c + d);

        /* sets the pixels in the new image array */
        Pnm_ybr pix_ptr1 = (Pnm_ybr)methods->at(new_image, col, row);
        *pix_ptr1 = *pixel1;
        Pnm_ybr pix_ptr2 = (Pnm_ybr)methods->at(new_image, col + 1, row);
        *pix_ptr2 = *pixel2;
        Pnm_ybr pix_ptr3 = (Pnm_ybr)methods->at(new_image, col, row + 1);
        *pix_ptr3 = *pixel3;
        Pnm_ybr pix_ptr4 = (Pnm_ybr)methods->at(new_image, col + 1, row + 1);
        *pix_ptr4 = *pixel4;

        /* free allocated memory for the pixels */
        free(pixel1);
        free(pixel2);
        free(pixel3);
        free(pixel4);
}

/**************** get_new_pixel ****************
 * 
 * given pr, pb, and y float values, creates a new instance of a Pnm_ybr
 * pixel
 *
 * Parameters:
 *      float pr: a float representing a pr value
 *      float pb: a float representing a pb value
 *      float y:  a float representing a y value
 * Returns:
 *      the newly created Pnm_ybr struct representing the pixel
 * Expects:
 *      The newly allocated Pnm_ybr struct was successful (throws a CRE if not)
 *
 ********************************************/
Pnm_ybr get_new_pixel(float pr, float pb, float y)
{
        Pnm_ybr pixel;
        NEW(pixel);
        assert(pixel != NULL);

        pixel->pr = pr;
        pixel->pb = pb;
        pixel->y = y;
        return pixel;
}

/**************** get_seq ****************
 * 
 * Given an A2 image object and a methods object, parses through the 2x2 pixel
 * blocks in row-major order and stores the converted 32-bit words into a
 * Hanson sequence.
 *
 * Parameters:
 *      A2 image:            pointer to a 2D image array containing the pixel
 *                           information to be converted into 32-bit words.
 *      A2Methods_T methods: methods object with which we will get the height
 *                           and width of the image.
 * Returns:
 *      Newly created sequence of 32-bit words
 * Expects:
 *      passed-in A2 image object is not NULL (throws a CRE if not)
 *      passed-in methods object is not NULL (throws a CRE if not)
 *      newly created sequence is successful (throws a CRE if not)
 *
 ********************************************/
Seq_T get_seq(A2 image, A2Methods_T methods)
{
        assert(image != NULL);
        assert(methods != NULL);

        int width = methods->width(image);
        int height = methods->height(image);

        /* initialize the sequence containing 32-bit words */
        Seq_T words = Seq_new(INITIAL_SEQ_LENGTH);
        assert(words != NULL);

        for (int row = 0; row < height; row += 2) {
                for (int col = 0; col < width; col += 2) {
                        /* Dynamically allocate memory for the word and copy
                         * the value */
                        uint64_t *word_ptr = malloc(sizeof(uint64_t));
                        assert(word_ptr != NULL);
                        
                        /* word_ptr = word; */
                        *word_ptr = get_word(col, row, image, methods);

                        /* Put in the sequence */
                        Seq_addhi(words, word_ptr);
                }
        }

        /* return sequence */
        return words;
}

/**************** get_word ****************
 * 
 * Given the column and row indices of an A2 image, returns the codeword
 * associated with the four pixels at that index.
 *
 * Parameters:
 *      int col:             column index of the pixels of the word
 *      int row:             row index of the pixels of the word
 *      A2 image:            pointer to the A2 image array from which we are
 *                           obtaining the words
 *      A2Methods_T methods: methods object with which we will get the four
 *                           pixels in the image array
 * Returns:
 *      the extracted 32-bit word as type uint64_t
 * Expects:
 *      passed-in A2 image object is not NULL (throws a CRE if not)
 *      passed-in methods object is not NULL (throws a CRE if not)
 *
 ********************************************/
uint64_t get_word(int col, int row, A2 image, A2Methods_T methods) 
{
        assert(image != NULL);
        assert(methods != NULL);

        /* gets the 4 pixels from the col, row index */
        Pnm_ybr ybr1 = methods->at(image, col, row);
        Pnm_ybr ybr2 = methods->at(image, col + 1, row);
        Pnm_ybr ybr3 = methods->at(image, col, row + 1);
        Pnm_ybr ybr4 = methods->at(image, col + 1, row + 1);
        
        /* obtain the average pb and pr floats */
        float average_pb = (ybr1->pb + ybr2->pb + ybr3->pb + ybr4->pb) / 4;
        float average_pr = (ybr1->pr + ybr2->pr + ybr3->pr + ybr4->pr) / 4;

        /* convert the average pb and pr floats to integers */
        uint64_t unsign_pb = Arith40_index_of_chroma(average_pb);
        uint64_t unsign_pr = Arith40_index_of_chroma(average_pr);

        /* obtain the a, b, c, and d floats */
        float a = (ybr4->y + ybr3->y + ybr2->y + ybr1->y) / 4.0;
        float b = (ybr4->y + ybr3->y - ybr2->y - ybr1->y) / 4.0;
        float c = (ybr4->y - ybr3->y + ybr2->y - ybr1->y) / 4.0;
        float d = (ybr4->y - ybr3->y - ybr2->y + ybr1->y) / 4.0;

        /* convert the a, b, c, and d floats to integers */
        uint64_t unsign_a = a_float_to_int(a);
        int64_t sign_b = bcd_float_to_int(b);
        int64_t sign_c = bcd_float_to_int(c);
        int64_t sign_d = bcd_float_to_int(d);

        /* pack the word using the obtained a, b, c, d, pb, and pr integers */
        uint64_t word = 0;
        word = Bitpack_newu(word, PRB_WIDTH, 0, unsign_pr);
        word = Bitpack_newu(word, PRB_WIDTH, PRB_WIDTH, unsign_pb);
        word = Bitpack_news(word, BCD_WIDTH, 2 * PRB_WIDTH, sign_d);
        word = Bitpack_news(word, BCD_WIDTH, BCD_WIDTH + 2 * PRB_WIDTH, sign_c);
        word = Bitpack_news(word, BCD_WIDTH, 2 * BCD_WIDTH + 2 * PRB_WIDTH, sign_b);
        word = Bitpack_newu(word, A_WIDTH, 3 * BCD_WIDTH + 2 * PRB_WIDTH, unsign_a);

        return word;
}

/**************** free_image ****************
 * 
 * Frees the memory associated with the given image containing the information
 * for the codewords
 *
 * Parameters:
 *      A2 image:            pointer to the A2 image array which we are freeing
 *      A2Methods_T methods: methods object with which we are using to free the
 *                           image containing the word information
 * Returns:
 *      None
 * Expects:
 *      passed-in A2 image object is not NULL (throws a CRE if not)
 *      passed-in methods object is not NULL (throws a CRE if not)
 *
 ********************************************/
void free_image(A2 image, A2Methods_T methods)
{
        assert(image != NULL);
        assert(methods != NULL);

        methods->free(&image);
}