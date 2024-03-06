#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "except.h"
#include "bitpack.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

#define MAX_BIT_WIDTH 64

/************** Bitpack_fitsu **************
 * 
 * Checks if an unsigned integer 'n' can be represented in 'width' bits
 *
 * Parameters:
 *      uint64_t n:     unsigned integer that will be checked by function
 *      unsigned width: unsigned integer that represents the maximum
 *                      number of bits the integer can fit into  
 * Returns:
 *      a boolean value representing whether it fits or not
 * Expects:
 *      does not expect anything
 *
 ********************************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        /* Find maximum value an unsigned integer of passed-in width can be */
        uint64_t max_value = (1 << width) - 1;

        /* Check if n is lesser than or equal to maximum value */
        return (n <= max_value);
}

/************** Bitpack_fitss **************
 * 
 * Checks if a signed integer 'n' can be represented in 'width' bits
 *
 * Parameters:
 *      int64_t n:      signed integer that will be checked by function
 *      unsigned width: unsigned integer that represents the maximum
 *                      number of bits the integer can fit into  
 * Returns:
 *      a boolean value representing whether it fits or not
 * Expects:
 *      does not expect anything
 *
 ********************************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{       
        if (n >= 0) {
                /* if n is greater than or equal to 0, left shift by 1 and
                 * then treat n as an unsigned integer */
                return Bitpack_fitsu(n << 1, width);
        } else {
                /* if n is lesser than 0, flip n to positive and then treat n
                 * as an unsigned integer */
                return Bitpack_fitsu(((~(n - 1)) << 1) - 1, width);
        }
}

/************** Bitpack_getu **************
 * 
 * Extracts the value in the given unsigned word that starts at the given least
 * significant bit and has the given width
 *
 * Parameters:
 *      uint64_t word:  unsigned 64-bit integer from which we are extracting
 *                      a value
 *      unsigned width: unsigned integer of the width of the value we will
 *                      extract
 *      unsigned lsb:   unsigned integer of the index of the least significant
 *                      bit of the value in the word
 * Returns:
 *      the extracted unsigned bit value
 * Expects:
 *      passed-in width is less than the max bit width (throws a CRE if not)
 *      passed-in width + passed-in least significant bit is less than the
 *      max bit width bc we would be extracting a value out of range (throws
 *      a CRE if not)
 *
 ********************************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        /* assert valid width and lsb values */
        assert(width <= MAX_BIT_WIDTH);
        assert(width + lsb <= MAX_BIT_WIDTH);

        /* fields of width zero are defined to contain the value zero */
        if (width == 0) return 0;

        /* beginning of the mask */
        uint64_t mask = (1 << width) - 1;

        /* shifted mask to match the field */
        mask = mask << lsb;

        /* apply mask onto word using bitwise 'and', and right shift by lsb */
        return (word & mask) >> lsb;
}

/************** Bitpack_gets **************
 * 
 * Extracts the value in the given signed word that starts at the given least
 * significant bit and has the given width
 *
 * Parameters:
 *      int64_t word:   signed 64-bit integer from which we are extracting
 *                      a value
 *      unsigned width: unsigned integer of the width of the value we will
 *                      extract
 *      unsigned lsb:   unsigned integer of the index of the least significant
 *                      bit of the value in the word
 * Returns:
 *      the extracted signed bit value
 * Expects:
 *      passed-in width is less than the max bit width (throws a CRE if not)
 *      passed-in width + passed-in least significant bit is less than the
 *      max bit width bc we would be extracting a value out of range (throws
 *      a CRE if not)
 *
 ********************************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) 
{
        /* assert valid width and lsb values */
        assert(width <= MAX_BIT_WIDTH);
        assert(width + lsb <= MAX_BIT_WIDTH);

        /* fields of width zero are defined to contain the value zero */
        if (width == 0) return 0;

        /* use the Bitpack_getu function to get the unsigned field value */
        uint64_t unsigned_field = Bitpack_getu(word, width, lsb);

        /* greatest unsigned integer of the given width that could represent
         * a positive signed value in two's complement */
        uint64_t greatest_pos_val = (1 << (width - 1)) - 1;
        
        if (unsigned_field <= greatest_pos_val) {
                /* if field value is positive, return positive field value */
                return (int64_t)unsigned_field;
        } else {
                /* if field value is negative, transfer it to negative values */
                return (int64_t)(unsigned_field - (1 << width));
        }
}

/************** Bitpack_newu **************
 * 
 * Updates designated field in the unsigned integer 'word' with a new unsigned
 * integer 'value'
 *
 * Parameters:
 *      uint64_t word:  unsigned integer that will have its field updated
 *      unsigned width: unsigned integer that represents the width of the field
 *      unsigned lsb:   unsigned integer that denotes where the least
 *                      significant bit is
 *      uint64_t value: unsigned integer that will replace the original field
 * Returns:
 *      an unsigned integer which is the updated 'word'
 * Expects:
 *      passed-in width is less than the max bit width (throws a CRE if not)
 *      passed-in width + passed-in least significant bit is less than the
 *      max bit width bc we would be extracting a value out of range (throws
 *      a CRE if not)
 *      new field 'value' fits inside the width of the original field (raises
 *      a Bitpack_Overflow exception if not)
 *
 ********************************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value) 
{
        /* assert valid width and lsb values */
        assert(width <= MAX_BIT_WIDTH);
        assert(width + lsb <= MAX_BIT_WIDTH);

        /* check if the value fits within the width of the original field */
        if (Bitpack_fitsu(value, width) != true) {
                RAISE(Bitpack_Overflow);
                assert(0);
        }
        
        /* acquire the original field using Bitpack_getu */
        uint64_t unsigned_field = Bitpack_getu(word, width, lsb);

        /* obtain the bitwise exclusive or between the unsigned_field and the
         * unsigned value so that we can use it as a mask to return the
         * updated word */
        uint64_t field_exclusive_or = (unsigned_field ^ value);
        field_exclusive_or = field_exclusive_or << lsb;

        return (field_exclusive_or ^ word);
}

/************** Bitpack_news **************
 * 
 * Updates designated field in the unsigned integer 'word' with a new signed
 * integer 'value'
 *
 * Parameters:
 *      uint64_t word:  unsigned integer that will have its field updated
 *      unsigned width: unsigned integer that represents the width of the field
 *      unsigned lsb:   unsigned integer that denotes where the least
 *                      significant bit is
 *      int64_t value:  signed integer that will replace the original field
 * Returns:
 *      an unsigned integer which is the updated 'word'
 * Expects:
 *      passed-in width is less than the max bit width (throws a CRE if not)
 *      passed-in width + passed-in least significant bit is less than the
 *      max bit width bc we would be extracting a value out of range (throws
 *      a CRE if not)
 *      new field 'value' fits inside the width of the original field (raises
 *      a Bitpack_Overflow exception if not)
 *
 ********************************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value)
{
        /* assert valid width and lsb values */
        assert(width <= MAX_BIT_WIDTH);
        assert(width + lsb <= MAX_BIT_WIDTH);

        /* check if the value fits within the width of the original field */
        if (Bitpack_fitss(value, width) != true) {
                RAISE(Bitpack_Overflow);
                assert(0);
        }

        /* find the greatest positive value possible when interpreting a signed
         * integer as an unsigned integer */
        uint64_t greatest_pos_val = (1 << (width - 1)) - 1;

        if ((uint64_t)value <= greatest_pos_val) {
                /* if value is positive, treat as an unsigned integer and pass
                 * it through Bitpack_newu */
                return Bitpack_newu(word, width, lsb, value);
        } else {
                /* create a mask of 1's in order to account for the trailing 
                 * 1's present in the signed value */
                uint64_t value_mask = (1 << width) - 1;

                /* convert the value into an unsigned integer using bitwise
                 * 'and', and pass it through Bitpack_newu */
                return Bitpack_newu(word, width, lsb, (value & value_mask));
        }
}