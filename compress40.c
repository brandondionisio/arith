#include "compress40.h"
#include "bitpack.h"
#include "a2methods.h"
#include "a2plain.h"
#include "uarray2.h"
#include "assert.h"
#include "pnm.h"

typedef A2Methods_UArray2 A2;

// void apply_compression(int col, int row, A2Methods_T array, void *elem, void *cl);

void decompress_test(A2 pixmap);

/* typedef struct data {
        A2 array;
        A2Methods_T methods;
} *data;
*/
extern void compress40(FILE *input) 
{       
        assert(input != NULL);

        A2Methods_T methods = uarray2_methods_plain; 

        assert(methods != NULL);

        Pnm_ppm image = Pnm_ppmread(input, methods);
        
        assert(image != NULL);
        
        int width = (int)image->width;
        int height = (int)image->height;

        printf("Original image height: %d, width: %d\n", height, width);

        assert(width > 1 && height > 1);

        if ((width % 2) == 1) {
                width -= 1;
        }    

        if ((height % 2) == 1) {
                height -= 1;
        }

        printf("New image height: %d, width: %d\n", height, width);

        A2 trimmed_image = methods->new(width, height, 10);
        assert(trimmed_image != NULL);

        for (int col = 0; col < width; col++) {
                for (int row = 0; row < height; row++) {
                        printf("col is: %d & row is %d\n", col, row);
                        /* 
                        struct Pnm_rgb *new_elem = methods->at(new_arr, org_height - row - 1, col);
                        *new_elem = *(struct Pnm_rgb *)elem; */


                        //struct Pnm_rgb *pixel = methods->at(trimmed_image, col, row);
                        //*pixel = *(struct Pnm_rgb *)image->methods->at(image->pixels, col, row);
                }
        }

        // struct data {
        //         pixels,
        //         methods
        // };

        /* TODO: change the size to 32 bit word struct */
        //A2 compressed_image = image->methods->new(width / 2, height / 2, 10);
        //(void) compressed_image;

        /* image->methods->map(pixels, apply, cl); */

        // decompress_test(trimmed_image);
}

/* void convert_RGB(int col, int row, A2 array, void *elem, void *cl)
{       
        assert(elem != NULL);
        assert(cl != NULL);

        data cvc_array = (data)cl;
        A2 RGB_pixels = cvc_array->array;
        A2Methods_T methods = cvc_array->methods;

        int width = methods->width(array);
        int height = methods->height(array);

        
} */

void decompress_test(A2 pixmap)
{
        Pnm_ppmwrite(stdout, pixmap);
}


extern void decompress40(FILE *input) 
{
        (void)input;
}

