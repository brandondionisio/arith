/**************************************************************
 *
 *                     a2plain.c
 *
 *     Assignment: HW 3: locality
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the A2Methods_T interface for the 
 *              UArray2_T data structure. This file is used to create a
 *              2D array of elements and contains apply functions to the 
 *              elements in the array. This file is used in conjunction with 
 *              ppmtrans.c to apply transformations to a PPM image.
 *  
 **************************************************************/

#include <string.h>
#include <a2plain.h>
#include <assert.h>

#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

typedef A2Methods_UArray2 A2;   /* private abbreviation */

/************* new ***************
 * 
 * Returns a new instance of an A2Methods_UArray2 with a given width, height
 * and size.
 *
 * Parameters:
 *      int width:  the width (number of cells along the x-axis) of the
 *                  A2Methods_UArray2 to be initialized
 *      int height: the height (number of cells along the y-axis) of the
 *                  A2Methods_UArray2 to be initialized
 *      int size:   the size of each cell for the new A2Methods_UArray2
 * Returns:
 *      an instance of the newly created A2Methods_UArray2 object
 * Expects:
 *      the passed-in width > 0, height > 0, and size > 0 (throws a CRE
 *      otherwise)
 *
 ********************************************/
static A2Methods_UArray2 new(int width, int height, int size)
{
        assert(width > 0 && height > 0 && size > 0);
        return UArray2_new(width, height, size);
}

/************* new_with_blocksize ***************
 * 
 * Returns a new instance of an A2Methods_UArray2 with a given width, height
 * and size and blocksize (we cannot use blocksize as UArray2s do not employ
 * blocking).
 *
 * Parameters:
 *      int width:     the width (number of cells along the x-axis) of the
 *                     A2Methods_UArray2 to be initialized
 *      int height:    the height (number of cells along the y-axis) of the
 *                     A2Methods_UArray2 to be initialized
 *      int size:      the size of each cell for the new A2Methods_UArray2
 *      int blocksize: unused blocksize variable for the sake of polymorphism
 * Returns:
 *      an instance of the newly created A2Methods_UArray2 object
 * Expects:
 *      the passed-in width > 0, height > 0, and size > 0 (throws a CRE
 *      otherwise)
 *
 ********************************************/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        assert(width > 0 && height > 0 && size > 0);
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/*************** a2free ***************
 * 
 * Frees all the memory associated with the array2p (A2Methods_UArray2)
 * passed in as a pointer
 *
 * Parameters:
 *      T *array2p: pointer to a UArray2b
 * Returns:
 *      None
 * Expects:
 *      The passed-in pointer does not point to nothing (throws a CRE
 *      otherwise)
 *      The dereferenced UArray2p is not NULL (throws a CRE otherwise)
 *
 ********************************************/
static void a2free(A2 *array2p)
{
        assert(array2p != NULL && *array2p != NULL);
        UArray2_free((UArray2_T *) array2p);
}

/****************** width ***************
 * 
 * Returns the width of the given A2Methods_UArray2
 *
 * Parameters:
 *      A2 array2: an A2Methods_UArray2 object
 * Returns:
 *      integer representing the width of the passed-in A2Methods_UArray2
 * Expects:
 *      The passed-in A2Methods_UArray2 is not NULL (throws a CRE otherwise)
 *
 ********************************************/
static int width(A2 array2)
{
        assert(array2 != NULL);
        return UArray2_width(array2);
}

/*************** height ***************
 * 
 * Returns the height of the given A2Methods_UArray2
 *
 * Parameters:
 *      A2 array2: an A2Methods_UArray2 object
 * Returns:
 *      integer representing the height of the passed-in A2Methods_UArray2
 * Expects:
 *      The passed-in A2Methods_UArray2 is not NULL (throws a CRE otherwise)
 *
 ********************************************/
static int height(A2 array2)
{
        assert(array2 != NULL);
        return UArray2_height(array2);
}

/*************** size ***************
 * 
 * Returns the size of a single element of the given A2Methods_UArray2
 *
 * Parameters:
 *      A2 array2: an A2Methods_UArray2 object
 * Returns:
 *      integer representing the height of the passed-in A2Methods_UArray2
 * Expects:
 *      The passed-in A2Methods_UArray2 is not NULL (throws a CRE if not)
 *
 ********************************************/
static int size(A2 array2)
{
        assert(array2 != NULL);
        return UArray2_size(array2);
}

/*************** blocksize ***************
 * 
 * Place holder function to attain polymorphism, will always return -1.
 *
 * Parameters:
 *      A2 array2: an A2Methods_UArray2 object that will be unused.
 * Returns:
 *      -1
 * Expects:
 *      None
 *
 ********************************************/
static int blocksize(A2 array2)
{
        (void) array2;
        return -1;
}

/*************** at ***************
 * 
 * Returns a pointer to the A2Methods_Object at the given col, row indices from
 * the given array2 object.
 *
 * Parameters:
 *      A2 array2: an A2Methods_UArray2 object
 * Returns:
 *      returns a pointer to the A2Methods_Object at the given col, row indices
 * Expects:
 *      The passed-in A2Methods_UArray2 is not NULL (throws a CRE otherwise)
 *
 ********************************************/
static A2Methods_Object *at(A2 array2, int col, int row)
{
        assert(array2 != NULL);
        return UArray2_at(array2, col, row);
}

/*************** map_row_major ***************
 * 
 * Maps through the given A2Methods_UArray2 in row-major order and executes
 * the given apply function on every element, storing data in the void pointer
 * closure.
 *
 * Parameters:
 *      A2Methods_UArray2 uarray2: an A2Methods_UArray2 object
 *      A2Methods_applyfun apply:  apply function used for row-major mapping
 *      void *cl:                  void pointer to a closure where data will be
 *                                 stored throughout mapping
 * Returns:
 *      None
 * Expects:
 *      uarray2 does not equal NULL (throws a CRE otherwise)
 *
 ***************************************/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        assert(uarray2 != NULL);
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/*************** map_col_major ***************
 * 
 * Maps through the given A2Methods_UArray2 in column-major order and executes
 * the given apply function on every element, storing data in the void pointer
 * closure.
 *
 * Parameters:
 *      A2Methods_UArray2 uarray2: an A2Methods_UArray2 object
 *      A2Methods_applyfun apply:  apply function used for column-major mapping
 *      void *cl:                  void pointer to a closure where data will be
 *                                 stored throughout mapping
 * Returns:
 *      None
 * Expects:
 *      uarray2 does not equal NULL (throws a CRE otherwise)
 *
 ***************************************/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        assert(uarray2 != NULL);
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/********** small_closure ********
 * 
 * Modified struct to conform to the closure of apply_small
 * This struct holds only the A2Methods_smallapplyfun and a void pointer to the
 * closure such that we can run the small mapping functions with only the elem
 * and the apply function.
 *
 *******************/
struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/*************** apply_small ***************
 * 
 * Apply function for the A2Methods_UArray2 small mapping functions that only
 * utilizes elem and a closure.
 *
 * Parameters:
 *      int i:             column index which is unused
 *      int j:             row index which is unused
 *      UArray2_T uarray2: instance of the UArray2_T which is unused
 *      void *elem:        void pointer to the element which IS used
 *      void *vcl:         void pointer to the closure which IS used 
 * Returns:
 *      None
 * Expects:
 *      None
 *
 ***************************************/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/*************** small_map_row_major ***************
 * 
 * Maps through the given A2Methods_UArray2 in row-major order and executes
 * the given apply function on every element, storing data in the void pointer
 * closure. This mapping function is smaller as it only utilizes elem and a
 * closure.
 *
 * Parameters:
 *      A2Methods_UArray2 uarray2: an A2Methods_UArray2 object
 *      A2Methods_applyfun apply:  apply function used for row-major mapping
 *      void *cl:                  void pointer to a closure where data will be
 *                                 stored throughout mapping
 * Returns:
 *      None
 * Expects:
 *      None
 *
 ***************************************/
static void small_map_row_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply, void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/*************** small_map_col_major ***************
 * 
 * Maps through the given A2Methods_UArray2 in column-major order and executes
 * the given apply function on every element, storing data in the void pointer
 * closure. This mapping function is smaller as it only utilizes elem and a
 * closure.
 *
 * Parameters:
 *      A2Methods_UArray2 uarray2: an A2Methods_UArray2 object
 *      A2Methods_applyfun apply:  apply function used for column-major mapping
 *      void *cl:                  void pointer to a closure where data will be
 *                                 stored throughout mapping
 * Returns:
 *      None
 * Expects:
 *      None
 *
 ***************************************/
static void small_map_col_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply, void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/********** uarray2_methods_plain_struct ********
 * 
 * Struct declaration that will hold all the methods of a plain UArray2.
 * This struct is of type A2Methods_T and retains all the functions used
 * for the UArray2 implementation. This implementation is polymorphic for
 * the A2Methods_T object.
 *
 ************************************************/
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL,                   /* map_block_major */
        map_row_major,          /* map_default, */
        small_map_row_major,
        small_map_col_major,
        NULL,                   /* small_map_block_major */
        small_map_row_major,    /* small_map_default */
};

/* finally the payoff: here is the exported pointer to the struct */

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;