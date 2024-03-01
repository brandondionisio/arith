#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "uarray2.h"

typedef A2Methods_UArray2 A2;

typedef struct data {
        A2 array;
        A2Methods_T methods;
        unsigned denominator;
} *data;
