<<<<<<< HEAD
/*************************************************************
 *
 *                     40image.c
 *
 *     Assignment: HW 4: arith
 *        Authors: Brandon Dionisio & Jordan Pauzie (bdioni01 & jpauzi01)
 *           Date: 03/07/24
 *
 *     Summary: This file implements the main function for the 40image program
 *              which handles commands and calls the functions necessary for
 *              both image compression and decompression.
 *              
 **************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

=======
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
>>>>>>> master
#include "assert.h"
#include "compress40.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

<<<<<<< HEAD
/**************** main ****************
 * 
 * main function for the 40image program which handles commands and calls the
 * functions necessary for both image compression and decompression.
 *
 * Parameters:
 *      int argc:     number of arguments passed into the program
 *      char *argv[]: pointer to the array containing the arguments
 * Returns:
 *      integer determining the success of the program
 * Expects:
 *      there is at most one file provided on the command line (throws a CRE
 *      if not)
 *      provided file is able to be opened (throws a CRE if not)
 *
 ********************************************/
=======
>>>>>>> master
int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}
