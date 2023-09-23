#include <stdlib.h>

#include "fliki.h"
#include "global.h"
#include "debug.h"

static int get_str_len(char *str) {
    int len = 0;
    while(*(str + len) != '\0') {
        ++len;
    }
    return len;
}

static int found_h(char *str) {
    return 
        get_str_len(str) == 2 && *(str + 0) == '-' && *(str + 1) == 'h' && *(str + 2) == '\0';
}

// 0 for finding a -h in a wrong spot (not the first arugment) 
// 1 for finding a -h in the correct spot and a 2 for finding no h
static int valid_h_arg(int argc, char **argv) {
    if(found_h(*(argv + 1))) {
        return 1;
    }
    for(int i = 2; i < argc; ++i) {
        if(found_h(*(argv + i))) {
            return 0;
        }
    }
    return 2; // means no h
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 * @modifies global variable "diff_filename" to point to the name of the file
 * containing the diffs to be used.
 */

int validargs(int argc, char **argv) {
    // TO BE IMPLEMENTED.
    if(argc == 1) {
        return -1; 
    }
    global_options = 0x0; 
    int h_arg_num = valid_h_arg(argc, argv);
    switch(h_arg_num) {
        case 0:
            return -1;
        case 1:
            global_options |= HELP_OPTION;
            return 0;
        case 2:
            break;
    }
    // guarantee that -h is not given
    // diff_filename pointer initially points to nil as it is undefined global static
    if(*(*(argv + (argc - 1)) + 0) != '-')
        diff_filename = *(argv + (argc - 1));

    if(diff_filename == NULL) // no filename 
        return -1;

    // any flag (meaning string preceding with -) that is invalid will return -1 
    // argc - 1 bc diff_filename is already set
    for(int i = 1; i < argc - 1; ++i) {
        char first_c = *(*(argv + i) + 0);
        if(first_c == '-') {
            if(get_str_len(*(argv + i)) != 2) {
                return -1;
            } else {
                char second_c = *(*(argv + i) + 1);
                switch(second_c) {
                    case 'q':
                        global_options |= QUIET_OPTION;
                        break;
                    case 'n':
                        global_options |= NO_PATCH_OPTION;
                        break;
                    default:
                        return -1;
                }
            }
        } else {
            // bc multiple files
            return -1;
        }
    }
    // success?
    //abort();
    return 0;
}
