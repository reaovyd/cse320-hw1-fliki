#include <stdio.h>
#include <stdlib.h>

#include "fliki.h"
#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    if(global_options == HELP_OPTION)
        USAGE(*argv, EXIT_SUCCESS);
    // TO BE IMPLEMENTED
    //fgetc(NULL);
    FILE *diff_file_fptr = fopen(diff_filename, "r"); 
    if(patch(stdin, stdout, diff_file_fptr)) {
        if(diff_file_fptr != NULL) {
            fclose(diff_file_fptr);
        }
        return EXIT_FAILURE;
    }
    if(diff_file_fptr != NULL) {
        fclose(diff_file_fptr);
    }
    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
