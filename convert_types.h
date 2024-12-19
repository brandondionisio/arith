/**************************************************************
 *
 *                     convert_types.h
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: Function declarations for the type conversions that occur
 *              within the 40image program. The functions in this file are used
 *              to convert a, b, c, and d values from floats to ints and vice
 *              versa, and floats within the range of 0 and 1.
 *
 **************************************************************/

#include <stdint.h>

#include "math.h"

/*****************************************************************
 *                  Conversion Function Declarations             *
 *****************************************************************/

float set_range(float num);
uint64_t a_float_to_int(float a_val);
int64_t bcd_float_to_int(float bcd_val);
float a_int_to_float(uint64_t u_a);
float bcd_int_to_float(int64_t s_bcd);