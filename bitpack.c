#include "bitpack.h"

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        (void) n;
        (void) width;
        return true;
}

bool Bitpack_fitss( int64_t n, unsigned width)
{
        (void) n;
        (void) width;   
        return true;
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        (void) word;
        (void) width;
        (void) lsb;
        return 0;
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) 
{
        (void) word;
        (void) width;
        (void) lsb;
        return 0;
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value) 
{
        (void) word;
        (void) width;
        (void) lsb;
        (void) value;
        return 0;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value) 
{
        (void) word;
        (void) width;
        (void) lsb;
        (void) value;
        return 0;
}

extern Except_T Bitpack_Overflow;