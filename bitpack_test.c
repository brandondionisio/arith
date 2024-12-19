#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "bitpack.h"

int main()
{
        /* testing unsigned */
        uint64_t test_int = 15;
        assert(Bitpack_fitsu(test_int, 4));
        assert(!Bitpack_fitsu(test_int, 3));
        assert(Bitpack_fitsu(test_int, 6));
        test_int = 16;
        assert(!Bitpack_fitsu(test_int, 4));

        /* testing signed */
        int64_t test1 = 63;
        int64_t test2 = -8;
        // bit representation -> 0111111
        assert(Bitpack_fitss(test1, 7));
        assert(!Bitpack_fitss(test1, 6));
        assert(Bitpack_fitss(test2, 4));
        assert(!Bitpack_fitss(test2, 3));

        /* testing getu */
        assert(Bitpack_getu(0x3f4, 6, 2) == 61);
        assert(Bitpack_gets(0x3f4, 6, 2) == -3);     

        /* testing newu */
        assert(Bitpack_getu(Bitpack_newu(8, 2, 1, 1), 2, 1) == 1);
        assert(Bitpack_getu(Bitpack_newu(0x3f4, 6, 2, 34), 6, 2) == 34);
        assert(Bitpack_getu(Bitpack_newu(16, 3, 2, 7), 3, 2) == 7);
        assert(Bitpack_getu(Bitpack_newu(16, 3, 2, 7), 3, 2) == Bitpack_getu(28, 3, 2));
<<<<<<< HEAD

=======
        
>>>>>>> master
        /* testing news */
        assert(Bitpack_gets(Bitpack_news(8, 2, 1, 1), 2, 1) == 1);
        assert(Bitpack_gets(Bitpack_news(0x3f4, 6, 2, 18), 6, 2) == 18);
        assert(Bitpack_news(0x3f4, 6, 2, -18) == 952);
        assert(Bitpack_gets(Bitpack_news(16, 4, 2, 7), 3, 2) == -1);
}