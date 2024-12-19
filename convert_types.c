/**************************************************************
 *
 *                     convert_types.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the functions for the type conversions
 *              that occur within the 40image program. The functions in this
 *              file are used to convert a, b, c, and d values from floats to
 *              ints and vice versa, and floats within the range of 0 and 1.
 *
 **************************************************************/

#include "convert_types.h"

/* maximum integer value for an a value (6-bits unsigned) */
#define A_BIT_MAX 63

/* maximum integer range for an a value (0 - 1) */
#define A_BIT_RANGE 1.0

/* maximum integer value for an bcd value (6-bits signed) */
#define BCD_BIT_MAX 31

/* maximum integer range for the b, c, and d values (-.3 - .3) */
#define BCD_BIT_RANGE 0.3

/**************** set_range ****************
 * 
 * Sets a hard minimum and maximum range of values that a float can take
 *
 * Parameters:
 *      float num: float value that will be checked
 * Returns:
 *      a float value which is either the original value if it was between the
 *      range of 0 to 1 or an altered value of 0 or 1 depending on if the value
 *      was greater than 1 or lesser than 0.
 * Expects:
 *      does not expect anything
 *
 ********************************************/
float set_range(float num) 
{
        if (num < 0) {
                return 0.0;
        } else if (num > 1) {
                return 1.0;
        } else {
                return num;
        }       
}

/************** a_float_to_int **************
 * 
 * Converts a given float to a 9-bit unsigned integer
 *
 * Parameters:
 *      float a_val: float value which we assume to be an a-value.
 * Returns:
 *      a 9-bit unsigned integer as type uint64_t
 * Expects:
 *      does not expect anything
 *
 ********************************************/
uint64_t a_float_to_int(float a_val)
{
        a_val *= A_BIT_MAX;

        if (a_val < 0) {
                a_val = 0;
        } else if (a_val > A_BIT_MAX) {
                a_val = A_BIT_MAX;
        }
        return (uint64_t)a_val;
}

/************** bcd_float_to_int *************
 * 
 * Converts a given float to a 5-bit signed integer
 *
 * Parameters:
 *      float bcd_val: float value which we assume to be an b, c, or d value
 * Returns:
 *      a 5-bit signed integer as type int64_t
 * Expects:
 *      does not expect anything
 *
 ********************************************/
int64_t bcd_float_to_int(float bcd_val)
{
        /* range of -15 < bcd_val < 15 */
        bcd_val *= (BCD_BIT_MAX / BCD_BIT_RANGE);

        if (bcd_val < -BCD_BIT_MAX) {
                bcd_val = -BCD_BIT_MAX;
        } else if (bcd_val > BCD_BIT_MAX) {
                bcd_val = BCD_BIT_MAX;
        }

        return bcd_val;
}

/************** a_int_to_float **************
 * 
 * Converts a given 9-bit unsigned integer to a float
 *
 * Parameters:
 *      uint64_t a_unsigned: 9-bit unsigned integer which will be converted.
 * Returns:
 *      a float value
 * Expects:
 *      does not expect anything
 *
 ********************************************/
float a_int_to_float(uint64_t u_a)
{
        return (u_a * A_BIT_RANGE) / A_BIT_MAX;
}

/************** bcd_int_to_float *************
 * 
 * Converts a given 5-bit signed integer to a float
 *
 * Parameters:
 *      int64_t bcd_signed: 5-bit signed integer which will be converted.
 * Returns:
 *      a float value
 * Expects:
 *      does not expect anything
 *
 ********************************************/
float bcd_int_to_float(int64_t bcd_signed)
{
        return (bcd_signed * BCD_BIT_RANGE) / BCD_BIT_MAX;
}