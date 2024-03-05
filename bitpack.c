#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "except.h"
#include "bitpack.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        uint64_t max_value = (1U << width) - 1;

        return (n <= max_value);
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
        if (n >= 0) {
                return Bitpack_fitsu(n << 1, width);
        } else {
                return Bitpack_fitsu(((~(n - 1)) << 1) - 1, width);
        }
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) return 0;

        /* beginning of the mask */
        uint64_t mask = (1 << width) - 1;
        mask = mask << lsb;

        return (word & mask) >> lsb;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) return 0;

        /* Use the Bitpack_getu interface to get the unsigned field value. */
        uint64_t unsigned_field = Bitpack_getu(word, width, lsb);
        uint64_t greatest_pos_val = (1 << (width - 1)) - 1;

        /* Check whether the field value is negative. If it's negative, 
         * transfer it to negative values. */
        if (unsigned_field <= greatest_pos_val) {
                return (int64_t)unsigned_field;
        } else {
                return (int64_t)(unsigned_field - (1 << width));
        }
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value) 
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (Bitpack_fitsu(value, width) != true) {
                RAISE(Bitpack_Overflow);
                assert(0);
        }

        uint64_t unsigned_field = Bitpack_getu(word, width, lsb);

        uint64_t field_exclusive_or = (unsigned_field ^ value);

        field_exclusive_or = field_exclusive_or << lsb;

        return (field_exclusive_or ^ word);
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value) 
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (Bitpack_fitss(value, width) != true) {
                RAISE(Bitpack_Overflow);
                assert(0);
        }

        uint64_t unsigned_field = Bitpack_getu(word, width, lsb);

        uint64_t field_exclusive_or = (unsigned_field ^ value);

        field_exclusive_or = field_exclusive_or << lsb;

        return (field_exclusive_or ^ word);
}