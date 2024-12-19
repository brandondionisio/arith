/**************************************************************
 *
 *                     word_operations.h
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: Function declarations for the transformations involving the
 *              32-bit words. The functions in this file are used to add packed
 *              words to the sequence to print out, unpack words read-in from
 *              the input file into a 2d array, and free the words inside the
 *              2d array once we are finished with them.
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "assert.h"
#include "mem.h"
#include "seq.h"
#include "pnm.h"
#include "bitpack.h"
#include "arith40.h"
#include "convert_types.h"
#include "structs.c"


/*****************************************************************
 *              Word Operation Function Declarations             *
 *****************************************************************/
void unpack_word(uint64_t word, int col, int row, A2Methods_T methods,
                                                                 A2 new_image);
Pnm_ybr get_new_pixel(float pr, float pb, float y);
Seq_T get_seq(A2 image, A2Methods_T methods);
uint64_t get_word(int col, int row, A2 image, A2Methods_T methods);
void free_image(A2 image, A2Methods_T methods);
void free_words_small_apply(void *elem, void *cl);