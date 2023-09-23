#include <stdlib.h>
#include <stdio.h>

#include "fliki.h"
#include "global.h"
#include "debug.h"

/* FILE POINTER FLAGS*/
static int FPTR_PRV   = -1;
static int FPTR_CUR_C = -1; 

/* FLAGS */
static int HUNK_INVALID_SYNTAX     = 0;
static int VALID_HUNK_NEXT         = 0;  /* used to define whether hunk_getc is allowed to return a char */
static int already_on_a_line       = 0;
static HUNK_TYPE CURRENT_OP_MODE   = HUNK_NO_TYPE;
static int COMPLETELY_READ         = 0;

typedef enum CPT_MODE_TYPE {
    CPT_NO_TYPE,
    CPT_DELETION,
    CPT_ADDITION
} CPT_MODE_TYPE;

static struct change_p_t {
    CPT_MODE_TYPE cur_mode; /* either deletion =1 or addition mode =2 */
} cpt;

static struct buffer_pos_t {
    int buffer_pos_del;
    int buffer_pos_add;
    int buffer_size_pos_add;
    int buffer_size_pos_del;
} bpt;


/* DECLARATIONS */
static void print_error_msg(HUNK *hp, char *err_msg);
static void print_char(int c, FILE *out);
static inline int is_valid_digit(int c);
static inline int is_comma(int c);
static inline int to_digit(int c);
static int get_number(FILE *in, int *num, int *new_line_found);
static int parse_hunk_val(HUNK *hp, FILE *in, int con, int *cm1, int *cm2, int *new_line_found, int *finished_parsing);
static int verify_hunk_parse(HUNK *hp, FILE *in);
static int is_empty_file(FILE *f);
static void reset_cpt();
static void reset_buffers();
static void reset_some();
static void hunk_format_print(HUNK *hp, FILE *out);
static void hunk_show_deletions(HUNK *hp, FILE *out);
static void hunk_show_additions(HUNK *hp, FILE *out);

/* DEFINITIONS */
static void print_error_msg(HUNK *hp, char *err_msg) {
    if(global_options & QUIET_OPTION) {
        return; 
    }
    fprintf(stderr, "%s\n", err_msg); 
    hunk_show(hp, stderr);
}

static void print_char(int c, FILE *out) {
    if(global_options & NO_PATCH_OPTION) {
        return;
    }
    fputc(c, out);
}

static inline int is_valid_digit(int c) {
    return c >= '0' && c <= '9';
}

static inline int is_comma(int c) {
    return c == ',';
}

static inline int to_digit(int c) {
    return c - '0';
}

// returns *num if valid; -1 otherwise
static int get_number(FILE *in, int *num, int *new_line_found) {
    while(1) {
        if(feof(in)) {
            break;
        }
        FPTR_PRV = FPTR_CUR_C;
        FPTR_CUR_C = fgetc(in);
        if(!is_valid_digit(FPTR_CUR_C)) {
            if(FPTR_CUR_C != '\n') {
                ungetc(FPTR_CUR_C, in);
                FPTR_CUR_C = FPTR_PRV;
            } else {
                *new_line_found = 1; 
            }
            return *num;
        } else {
            if(*num == -1) {
                *num = 0;
            }
            *num *= 10;
            *num += (to_digit(FPTR_CUR_C));
        }
    }
    return -1;
}

static int parse_hunk_val(HUNK *hp, FILE *in, int con, int *cm1, int *cm2, int *new_line_found, int *finished_parsing) {
    switch(con) {
        case 1:
            int h_num = -1;
            int res = get_number(in, &h_num, new_line_found);
            if(res == -1) {
                return 0;
            } else {
                hp->old_start = h_num;
                hp->old_end = h_num;
            }
            return 1;
        case 2:
            FPTR_PRV = FPTR_CUR_C;
            FPTR_CUR_C = fgetc(in); 
            if(is_comma(FPTR_CUR_C)) {
                *cm1 = 1;
                return 1;
            } else {
                *cm1 = 0;
                switch(FPTR_CUR_C) {
                    case 'a':
                        hp->type = HUNK_APPEND_TYPE; 
                        return 1; 
                    case 'c':
                        hp->type = HUNK_CHANGE_TYPE; 
                        return 1; 
                    case 'd':
                        hp->type = HUNK_DELETE_TYPE; 
                        return 1; 
                }
                if(FPTR_CUR_C == '\n') {
                    *new_line_found = 1;
                }
            }
            return 0;
        case 3:
            if(*cm1) {
                int h_num = -1;
                int res = get_number(in, &h_num, new_line_found);
                if(res == -1) {
                    return 0;
                } else {
                    hp->old_end = h_num;
                    return 1;
                }
            } else {
                int h_num = -1;
                int res = get_number(in, &h_num, new_line_found);
                if(res == -1) {
                    return 0;
                } else {
                    hp->new_start = h_num;
                    hp->new_end   = h_num;
                    if(*new_line_found == 1) {
                        *finished_parsing = 1;
                    }
                    return 1;
                }
            }
            return 0;
        case 4:
            FPTR_PRV = FPTR_CUR_C;
            FPTR_CUR_C = fgetc(in);
            if(is_comma(FPTR_CUR_C)) {
               *cm2 = 1;
               return 1;
            } else {
                switch(FPTR_CUR_C) {
                    case 'c':
                        hp->type = HUNK_CHANGE_TYPE;
                        return 1;
                    case 'd':
                        hp->type = HUNK_DELETE_TYPE;
                        return 1;
                }
                if(FPTR_CUR_C == '\n') {
                    *new_line_found = 1;
                }
            }
            return 0;
        case 5:
            h_num = -1;
            res = get_number(in, &h_num, new_line_found);
            if(res == -1) {
                return 0;
            }
            hp->new_end = h_num;
            if(*cm2) {
                if(*new_line_found == 1) {
                    *finished_parsing = 1;
                }
                if(hp->type == HUNK_DELETE_TYPE) {
                    return 0;
                }
                return 1;
            } else {
                hp->new_start = h_num;
                if(*new_line_found == 1) {
                    *finished_parsing = 1;
                }
                return 1;
            }
        case 6:
            FPTR_PRV = FPTR_CUR_C;
            FPTR_CUR_C = fgetc(in);
            if(is_comma(FPTR_CUR_C)) {
                if(hp->type == HUNK_DELETE_TYPE) {
                    return 0;
                }
                return 1;
            }
            if(FPTR_CUR_C == '\n') {
                *new_line_found = 1;
            }
            return 0;
        case 7:
            h_num = -1;
            res = get_number(in, &h_num, new_line_found);
            if(res == -1) {
                return 0;
            }
            hp->new_end = h_num;
            if(*new_line_found == 1) {
                *finished_parsing = 1;
            }
            return 1;
    }
    // in the case con >= 8
    FPTR_PRV   = FPTR_CUR_C; 
    FPTR_CUR_C = fgetc(in);
    *new_line_found = 1;
    *finished_parsing = 0;
    if(FPTR_CUR_C == '\n') {
        return *new_line_found;
    }
    *new_line_found = 0;
    return *new_line_found;
}

static int verify_hunk_parse(HUNK *hp, FILE *in) {
    int len = 1;
    int finished_parsing = 0;
    int new_line_found = 2;
    int res_cum = 1;
    int cm1 = 0, cm2 = 0;

    while(1) {
        if(feof(in)) {
            return EOF;
        }
        int res = parse_hunk_val(hp, in, len, &cm1, &cm2, &new_line_found, &finished_parsing); 
        res_cum &= res;
        if(res_cum == 0) {
            return 0;
        }
        if(new_line_found == 1 || new_line_found == 0) { 
            //new line hasn't been found for 0 but anything larger than case 7 is wrong
            break;
        }
        len++;
    }
    if(new_line_found == 2) {
        return 0;
    }
    return res_cum & new_line_found & finished_parsing & (hp->new_start <= hp->new_end && hp->old_start <= hp->old_end);
}

static int is_empty_file(FILE *f) {
    if(f == NULL) {
        return 1;
    }
    int c = fgetc(f);
    if(c == EOF) {
        return 1;
    }
    ungetc(c, f);
    return 0;
}


static void reset_cpt() {
    cpt.cur_mode    = 0;
}

static void reset_buffers() {
    for(int i = 0; i < HUNK_MAX; ++i) {
        *(hunk_deletions_buffer + i) = 0;
        *(hunk_additions_buffer + i) = 0;
    }

    bpt.buffer_pos_del        = 2; /* typically buffer_size_pos_add + 2 = buffer_pos_del */
    bpt.buffer_pos_add        = 2;
    bpt.buffer_size_pos_add   = 0;
    bpt.buffer_size_pos_del   = 0;

    *(hunk_additions_buffer + (HUNK_MAX - 2)) = -1;
    *(hunk_additions_buffer + (HUNK_MAX - 1)) = -1;

    *(hunk_deletions_buffer + (HUNK_MAX - 2)) = -1;
    *(hunk_deletions_buffer + (HUNK_MAX - 1)) = -1;
}

static void reset_some() {
    reset_cpt();
    reset_buffers();
    already_on_a_line       = 0;
    COMPLETELY_READ         = 0;
}

static void hunk_format_print(HUNK *hp, FILE *out) {
    /* assume valid hunk */
    char format = hp->type == HUNK_CHANGE_TYPE ? 'c' : hp->type == HUNK_DELETE_TYPE ? 'd' : hp->type == HUNK_APPEND_TYPE ? 'a' : '?';
    if(hp->old_end == hp->old_start) {
        if(hp->new_end == hp->new_start) {
            fprintf(out, "%d%c%d\n", hp->old_end, format,  hp->new_end);
        } else {
            fprintf(out, "%d%c%d,%d\n", hp->old_end, format,  hp->new_start, hp->new_end);
        }
    } else {
        if(hp->new_end == hp->new_start) {
            fprintf(out, "%d,%d%c%d\n", hp->old_start, hp->old_end, format,  hp->new_end);
        } else {
            fprintf(out, "%d,%d%c%d,%d\n", hp->old_start, hp->old_end, format, hp->new_start, hp->new_end);
        }
    }
}


static void hunk_show_deletions(HUNK *hp, FILE *out) {
    int del_pos_itr = 2;
    int size_del_pos_itr = del_pos_itr - 2;

    unsigned char del_msk1 = 0xFF, del_msk2 = 0xFF;
    unsigned short del_calc_msk = 0x0;
    del_msk1 &= *(hunk_deletions_buffer + size_del_pos_itr);
    del_msk2 &= *(hunk_deletions_buffer + (size_del_pos_itr + 1));
    del_calc_msk |= del_msk2;
    del_calc_msk <<= 8;
    del_calc_msk |= del_msk1;

    while(del_pos_itr < HUNK_MAX - 2) {
        fputc('<', out);
        fputc(' ', out);
        while(del_calc_msk > 0) {
            fputc(*(hunk_deletions_buffer + del_pos_itr), out);
            ++del_pos_itr;
            --del_calc_msk;
        }
        if(del_calc_msk == 0) {
            del_pos_itr += 2;
            size_del_pos_itr = del_pos_itr - 2;

            del_msk1 = 0xFF;
            del_msk2 = 0xFF;
            del_calc_msk = 0x0;
            del_msk1 &= *(hunk_deletions_buffer + size_del_pos_itr);
            del_msk2 &= *(hunk_deletions_buffer + (size_del_pos_itr + 1));
            del_calc_msk |= del_msk2;
            del_calc_msk <<= 8;
            del_calc_msk |= del_msk1;

            if(del_calc_msk == 0) {
                break;
            }
        }
    }
    if(*(hunk_deletions_buffer + (HUNK_MAX - 2)) == 0 && *(hunk_deletions_buffer + (HUNK_MAX - 1)) == 0) {
        fprintf(out, "...\n");
    }
}

static void hunk_show_additions(HUNK *hp, FILE *out) {
    int add_pos_itr = 2;
    int size_add_pos_itr = add_pos_itr - 2;

    unsigned char add_msk1 = 0xFF, add_msk2 = 0xFF;
    unsigned short add_calc_msk = 0x0;
    add_msk1 &= *(hunk_additions_buffer + size_add_pos_itr);
    add_msk2 &= *(hunk_additions_buffer + (size_add_pos_itr + 1));
    add_calc_msk |= add_msk2;
    add_calc_msk <<= 8;
    add_calc_msk |= add_msk1;

    while(add_pos_itr < HUNK_MAX - 2) {
        fputc('>', out);
        fputc(' ', out);
        while(add_calc_msk > 0) {
            fputc(*(hunk_additions_buffer + add_pos_itr), out);
            ++add_pos_itr;
            --add_calc_msk;
        }
        if(add_calc_msk == 0) {
            add_pos_itr += 2;
            size_add_pos_itr = add_pos_itr - 2;

            add_msk1 = 0xFF;
            add_msk2 = 0xFF;
            add_calc_msk = 0x0;
            add_msk1 &= *(hunk_additions_buffer + size_add_pos_itr);
            add_msk2 &= *(hunk_additions_buffer + (size_add_pos_itr + 1));
            add_calc_msk |= add_msk2;
            add_calc_msk <<= 8;
            add_calc_msk |= add_msk1;

            if(add_calc_msk == 0) {
                break;
            }
        }
    }
    if(*(hunk_additions_buffer + (HUNK_MAX - 2)) == 0 && *(hunk_additions_buffer + (HUNK_MAX - 1)) == 0) {
        fprintf(out, "...\n");
    }
}


/**
 * @brief Get the header of the next hunk in a diff file.
 * @details This function advances to the beginning of the next hunk
 * in a diff file, reads and parses the header line of the hunk,
 * and initializes a HUNK structure with the result.
 *
 * @param hp  Pointer to a HUNK structure provided by the caller.
 * Information about the next hunk will be stored in this structure.
 * @param in  Input stream from which hunks are being read.
 * @return 0  if the next hunk was successfully located and parsed,
 * EOF if end-of-file was encountered or there was an error reading
 * from the input stream, or ERR if the data in the input stream
 * could not be properly interpreted as a hunk.
 */

int hunk_next(HUNK *hp, FILE *in) {
    if(HUNK_INVALID_SYNTAX) {
        return ERR;
    }
    if(in == NULL || feof(in)) {
        return EOF;
    }

    FPTR_PRV = FPTR_CUR_C;
    FPTR_CUR_C = fgetc(in);

    if(FPTR_PRV == -1) { // jsut for the top of file
        if(is_valid_digit(FPTR_CUR_C)) {
            ungetc(FPTR_CUR_C, in);
            FPTR_CUR_C = FPTR_PRV;
            int res = verify_hunk_parse(hp, in);
            int ret = res == EOF ? EOF : res == 0 ? ERR : 0;
            if(ret == 0) {
                reset_some();
                VALID_HUNK_NEXT = 1;
                ++hp->serial;
                CURRENT_OP_MODE = hp->type;
                if(CURRENT_OP_MODE == HUNK_CHANGE_TYPE) {
                    cpt.cur_mode = CPT_DELETION; 
                }
            } else if(ret == ERR) {
                VALID_HUNK_NEXT = 0;
                HUNK_INVALID_SYNTAX = 1;
                return ret;
            }

            return ret;
        } else {
            HUNK_INVALID_SYNTAX = 1;
            return ERR;
        }
    }

    while(1) {
        if(feof(in)) {
            return EOF;
        }
        if(FPTR_PRV == '\n') {
            if(!is_valid_digit(FPTR_CUR_C) && !(FPTR_CUR_C == '<' || FPTR_CUR_C == '>' || FPTR_CUR_C == '-')) {
                HUNK_INVALID_SYNTAX = 1;
                return ERR;
            } else if(is_valid_digit(FPTR_CUR_C)) {
                if(CURRENT_OP_MODE == HUNK_CHANGE_TYPE) {
                    // must have been a section swap
                    // because of hunk_Getc moving the fptr
                    // or in is this while loop that moves it and if the current op is a change hunk
                    // we must have detected a section swap at least one because there must be an addition section
                    // if not, then it is syntactically incorrect
                    if(cpt.cur_mode != CPT_ADDITION) {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                }
                break;
            } else {
                if(CURRENT_OP_MODE == HUNK_CHANGE_TYPE) {
                    if(cpt.cur_mode == CPT_DELETION) {
                        if(FPTR_CUR_C == '-') {
                            for(int i = 0; i < 2; ++i) {
                                FPTR_PRV = FPTR_CUR_C;
                                FPTR_CUR_C = fgetc(in);
                                if(FPTR_CUR_C != '-') {
                                    HUNK_INVALID_SYNTAX = 1;
                                    return ERR;
                                }
                            }
                            FPTR_PRV = FPTR_CUR_C;
                            FPTR_CUR_C = fgetc(in);
                            if(FPTR_CUR_C != '\n') {
                                HUNK_INVALID_SYNTAX = 1;
                                return ERR;
                            }
                            cpt.cur_mode = CPT_ADDITION;
                        } else {
                            if(FPTR_CUR_C != '<') {
                                HUNK_INVALID_SYNTAX = 1;
                                return ERR;
                            }
                            FPTR_PRV = FPTR_CUR_C;
                            FPTR_CUR_C = fgetc(in);
                            if(FPTR_CUR_C != ' ') {
                                HUNK_INVALID_SYNTAX = 1;
                                return ERR;
                            }
                        }
                    } else if(cpt.cur_mode == CPT_ADDITION) {
                        if(FPTR_CUR_C != '>') {
                            HUNK_INVALID_SYNTAX = 1;
                            return ERR;
                        }
                        FPTR_PRV = FPTR_CUR_C;
                        FPTR_CUR_C = fgetc(in);
                        if(FPTR_CUR_C != ' ') {
                            HUNK_INVALID_SYNTAX = 1;
                            return ERR;
                        }
                    }
                } else if(CURRENT_OP_MODE == HUNK_APPEND_TYPE) {
                    if(FPTR_CUR_C != '>') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                    FPTR_PRV = FPTR_CUR_C;
                    FPTR_CUR_C = fgetc(in);
                    if(FPTR_CUR_C != ' ') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                } else if(CURRENT_OP_MODE == HUNK_DELETE_TYPE) {
                    if(FPTR_CUR_C != '<') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                    FPTR_PRV = FPTR_CUR_C;
                    FPTR_CUR_C = fgetc(in);
                    if(FPTR_CUR_C != ' ') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                }
            }
        }
        FPTR_PRV = FPTR_CUR_C;
        FPTR_CUR_C = fgetc(in);
    }

    ungetc(FPTR_CUR_C, in);
    FPTR_CUR_C = FPTR_PRV;

    int res = verify_hunk_parse(hp, in);
    int ret = res == EOF ? EOF : res == 0 ? ERR : 0;
    if(ret == 0) {
        reset_some();
        VALID_HUNK_NEXT = 1;
        ++hp->serial;
        CURRENT_OP_MODE = hp->type;
        if(CURRENT_OP_MODE == HUNK_CHANGE_TYPE) {
            cpt.cur_mode = CPT_DELETION; 
        }
    } else if(ret == ERR) {
        HUNK_INVALID_SYNTAX = 1;
        VALID_HUNK_NEXT = 0;
        return ret;
    }

    return ret;
}

/**
 * @brief  Get the next character from the data portion of the hunk.
 * @details  This function gets the next character from the data
 * portion of a hunk.  The data portion of a hunk consists of one
 * or both of a deletions section and an additions section,
 * depending on the hunk type (delete, append, or change).
 * Within each section is a series of lines that begin either with
 * the character sequence "< " (for deletions), or "> " (for additions).
 * For a change hunk, which has both a deletions section and an
 * additions section, the two sections are separated by a single
 * line containing the three-character sequence "---".
 * This function returns only characters that are actually part of
 * the lines to be deleted or added; characters from the special
 * sequences "< ", "> ", and "---\n" are not returned.
 * @param hdr  Data structure containing the header of the current
 * hunk.
 *
 * @param in  The stream from which hunks are being read.
 * @return  A character that is the next character in the current
 * line of the deletions section or additions section, unless the
 * end of the section has been reached, in which case the special
 * value EOS is returned.  If the hunk is ill-formed; for example,
 * if it contains a line that is not terminated by a newline character,
 * or if end-of-file is reached in the middle of the hunk, or a hunk
 * of change type is missing an additions section, then the special
 * value ERR (error) is returned.  The value ERR will also be returned
 * if this function is called after the current hunk has been completely
 * read, unless an intervening call to hunk_next() has been made to
 * advance to the next hunk in the input.  Once ERR has been returned,
 * then further calls to this function will continue to return ERR,
 * until a successful call to call to hunk_next() has successfully
 * advanced to the next hunk.
 */

int hunk_getc(HUNK *hp, FILE *in) {
    if(HUNK_INVALID_SYNTAX) {
        return ERR;
    }

    if(!VALID_HUNK_NEXT) {
        return ERR;
    }

    if(COMPLETELY_READ) {
        return ERR;
    }

    if(!already_on_a_line) {
        if(CURRENT_OP_MODE == HUNK_CHANGE_TYPE) {
            if(cpt.cur_mode == CPT_ADDITION) {
                FPTR_PRV = FPTR_CUR_C;
                FPTR_CUR_C = fgetc(in);
                if(FPTR_CUR_C == '>') {
                    FPTR_PRV = FPTR_CUR_C;
                    FPTR_CUR_C = fgetc(in);
                    if(FPTR_CUR_C != ' ') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                } else {
                    if(is_valid_digit(FPTR_CUR_C) || feof(in)) {
                        ungetc(FPTR_CUR_C, in);
                        FPTR_CUR_C = FPTR_PRV;
                        COMPLETELY_READ = 1;
                        return EOS;
                    } else {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                }
            } else if(cpt.cur_mode == CPT_DELETION) {
                FPTR_PRV = FPTR_CUR_C;
                FPTR_CUR_C = fgetc(in);
                if(FPTR_CUR_C == '<') {
                    FPTR_PRV = FPTR_CUR_C;
                    FPTR_CUR_C = fgetc(in);
                    if(FPTR_CUR_C != ' ') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                    // but if it is just continue
                } else if(FPTR_CUR_C == '-') {
                    // still probably count ---\n non existing as an error
                    for(int i = 0; i < 2; ++i) {
                        FPTR_PRV = FPTR_CUR_C;
                        FPTR_CUR_C = fgetc(in);
                        if(FPTR_CUR_C != '-') {
                            HUNK_INVALID_SYNTAX = 1;
                            return ERR;
                        }
                    }
                    FPTR_PRV = FPTR_CUR_C;
                    FPTR_CUR_C = fgetc(in);

                    if(FPTR_CUR_C != '\n') {
                        HUNK_INVALID_SYNTAX = 1;
                        return ERR;
                    }
                    cpt.cur_mode = CPT_ADDITION;
                    return EOS;
                } else {
                    // bc even if it was a valid digit hunk, there is no addition section so ye
                    HUNK_INVALID_SYNTAX = 1;
                    return ERR;
                }
            }
        } else if(CURRENT_OP_MODE == HUNK_DELETE_TYPE) {
            FPTR_PRV = FPTR_CUR_C;
            FPTR_CUR_C = fgetc(in);
            if(FPTR_CUR_C == '<') {
                FPTR_PRV = FPTR_CUR_C;
                FPTR_CUR_C = fgetc(in);
                if(FPTR_CUR_C != ' ') {
                    HUNK_INVALID_SYNTAX = 1;
                    return ERR;
                }
            } else {
                if(is_valid_digit(FPTR_CUR_C) || feof(in)) {
                    ungetc(FPTR_CUR_C, in);
                    FPTR_CUR_C = FPTR_PRV;
                    COMPLETELY_READ = 1;
                    return EOS;
                } else {
                    HUNK_INVALID_SYNTAX = 1;
                    return ERR;
                }
            }
        } else if(CURRENT_OP_MODE == HUNK_APPEND_TYPE) {
            FPTR_PRV = FPTR_CUR_C;
            FPTR_CUR_C = fgetc(in);
            if(FPTR_CUR_C == '>') {
                FPTR_PRV = FPTR_CUR_C;
                FPTR_CUR_C = fgetc(in);
                if(FPTR_CUR_C != ' ') {
                    HUNK_INVALID_SYNTAX = 1;
                    return ERR;
                }
            } else {
                // OK SO if feof randomly on a new line just treat this as EOS first of all ig 
                if(is_valid_digit(FPTR_CUR_C) || feof(in)) {
                    ungetc(FPTR_CUR_C, in);
                    FPTR_CUR_C = FPTR_PRV;
                    COMPLETELY_READ = 1;
                    return EOS;
                } else {
                    HUNK_INVALID_SYNTAX = 1;
                    return ERR;
                }
            }
        }
        already_on_a_line = 1;
    }

    FPTR_PRV = FPTR_CUR_C;
    FPTR_CUR_C = fgetc(in);

    if(feof(in)) {
        HUNK_INVALID_SYNTAX = 1;
        return ERR;
    }

    if(FPTR_CUR_C == '\n') {
        already_on_a_line = 0;
    }

    if(CURRENT_OP_MODE == HUNK_APPEND_TYPE || 
            (CURRENT_OP_MODE == HUNK_CHANGE_TYPE && cpt.cur_mode == CPT_ADDITION)) {
        unsigned char msk1 = 0xFF, msk2 = 0xFF;
        unsigned short calc_msk = 0x0;
        msk1 &= *(hunk_additions_buffer + bpt.buffer_size_pos_add);
        msk2 &= *(hunk_additions_buffer + (bpt.buffer_size_pos_add + 1));
        calc_msk |= msk2;
        calc_msk <<= 8;
        calc_msk |= msk1;
        if(bpt.buffer_pos_add < HUNK_MAX - 2) {
            ++calc_msk;
            msk1 = calc_msk & 0xFF;
            msk2 = (calc_msk >> 8) & 0xFF;
            *(hunk_additions_buffer + bpt.buffer_size_pos_add) = msk1;
            *(hunk_additions_buffer + (bpt.buffer_size_pos_add + 1)) = msk2;
            *(hunk_additions_buffer + bpt.buffer_pos_add) = FPTR_CUR_C;

            ++bpt.buffer_pos_add;
            if(FPTR_CUR_C == '\n') {
                bpt.buffer_pos_add += 2;
                bpt.buffer_size_pos_add = bpt.buffer_pos_add - 2;
            }
            if(bpt.buffer_pos_add >= HUNK_MAX - 2) { /* piazza says truncation occurred here*/
                *(hunk_additions_buffer + (HUNK_MAX - 2)) = 0;
                *(hunk_additions_buffer + (HUNK_MAX - 1)) = 0;
            }
        } 
    } else if(CURRENT_OP_MODE == HUNK_DELETE_TYPE || 
            (CURRENT_OP_MODE == HUNK_CHANGE_TYPE && cpt.cur_mode == CPT_DELETION)) {
        unsigned char msk1 = 0xFF, msk2 = 0xFF;
        unsigned short calc_msk = 0x0;
        msk1 &= *(hunk_deletions_buffer + bpt.buffer_size_pos_del);
        msk2 &= *(hunk_deletions_buffer + (bpt.buffer_size_pos_del + 1));
        calc_msk |= msk2;
        calc_msk <<= 8;
        calc_msk |= msk1;
        if(bpt.buffer_pos_del < HUNK_MAX - 2) {
            ++calc_msk;
            msk1 = calc_msk & 0xFF;
            msk2 = (calc_msk >> 8) & 0xFF;
            *(hunk_deletions_buffer + bpt.buffer_size_pos_del) = msk1;
            *(hunk_deletions_buffer + (bpt.buffer_size_pos_del + 1)) = msk2;
            *(hunk_deletions_buffer + bpt.buffer_pos_del) = FPTR_CUR_C;

            ++bpt.buffer_pos_del;
            if(FPTR_CUR_C == '\n') {
                bpt.buffer_pos_del += 2;
                bpt.buffer_size_pos_del = bpt.buffer_pos_del - 2;
            }
            if(bpt.buffer_pos_del >= HUNK_MAX - 2) { /* piazza says truncation occurred here*/
                *(hunk_deletions_buffer + (HUNK_MAX - 2)) = 0;
                *(hunk_deletions_buffer + (HUNK_MAX - 1)) = 0;
            }
        }
    }
    return FPTR_CUR_C;
}

/**
 * @brief  Print a hunk to an output stream.
 * @details  This function prints a representation of a hunk to a
 * specified output stream.  The printed representation will always
 * have an initial line that specifies the type of the hunk and
 * the line numbers in the "old" and "new" versions of the file,
 * in the same format as it would appear in a traditional diff file.
 * The printed representation may also include portions of the
 * lines to be deleted and/or inserted by this hunk, to the extent
 * that they are available.  This information is defined to be
 * available if the hunk is the current hunk, which has been completely
 * read, and a call to hunk_next() has not yet been made to advance
 * to the next hunk.  In this case, the lines to be printed will
 * be those that have been stored in the hunk_deletions_buffer
 * and hunk_additions_buffer array.  If there is no current hunk,
 * or the current hunk has not yet been completely read, then no
 * deletions or additions information will be printed.
 * If the lines stored in the hunk_deletions_buffer or
 * hunk_additions_buffer array were truncated due to there having
 * been more data than would fit in the buffer, then this function
 * will print an elipsis "..." followed by a single newline character
 * after any such truncated lines, as an indication that truncation
 * has occurred.
 *
 * @param hp  Data structure giving the header information about the
 * hunk to be printed.
 * @param out  Output stream to which the hunk should be printed.
 */

void hunk_show(HUNK *hp, FILE *out) {
    if(!VALID_HUNK_NEXT) {
        return;
    }
    hunk_format_print(hp, out);
    if(!COMPLETELY_READ) {
        return;
    }

    switch(hp->type) {
        case HUNK_CHANGE_TYPE:
            hunk_show_deletions(hp, out);
            fprintf(out, "---\n");
            hunk_show_additions(hp, out);
            break;
        case HUNK_APPEND_TYPE:
            hunk_show_additions(hp, out);
            break;
        case HUNK_DELETE_TYPE:
            hunk_show_deletions(hp, out);
            break;
        default: /* can't happen since valid_hunk_next but here so it can compile*/
            break;
    }
}

/**
 * @brief  Patch a file as specified by a diff.
 * @details  This function reads a diff file from an input stream
 * and uses the information in it to transform a source file, read on
 * another input stream into a target file, which is written to an
 * output stream.  The transformation is performed "on-the-fly"
 * as the input is read, without storing either it or the diff file
 * in memory, and errors are reported as soon as they are detected.
 * This mode of operation implies that in general when an error is
 * detected, some amount of output might already have been produced.
 * In case of a fatal error, processing may terminate prematurely,
 * having produced only a truncated version of the result.
 * In case the diff file is empty, then the output should be an
 * unchanged copy of the input.
 *
 * This function checks for the following kinds of errors: ill-formed
 * diff file, failure of lines being deleted from the input to match
 * the corresponding deletion lines in the diff file, failure of the
 * line numbers specified in each "hunk" of the diff to match the line
 * numbers in the old and new versions of the file, and input/output
 * errors while reading the input or writing the output.  When any
 * error is detected, a report of the error is printed to stderr.
 * The error message will consist of a single line of text that describes
 * what went wrong, possibly followed by a representation of the current
 * hunk from the diff file, if the error pertains to that hunk or its
 * application to the input file.  If the "quiet mode" program option
 * has been specified, then the printing of error messages will be
 * suppressed.  This function returns immediately after issuing an
 * error report.
 *
 * The meaning of the old and new line numbers in a diff file is slightly
 * confusing.  The starting line number in the "old" file is the number
 * of the first affected line in case of a deletion or change hunk,
 * but it is the number of the line *preceding* the addition in case of
 * an addition hunk.  The starting line number in the "new" file is
 * the number of the first affected line in case of an addition or change
 * hunk, but it is the number of the line *preceding* the deletion in
 * case of a deletion hunk.
 *
 * @param in  Input stream from which the file to be patched is read.
 * @param out Output stream to which the patched file is to be written.
 * @param diff  Input stream from which the diff file is to be read.
 * @return 0 in case processing completes without any errors, and -1
 * if there were errors.  If no error is reported, then it is guaranteed
 * that the output is complete and correct.  If an error is reported,
 * then the output may be incomplete or incorrect.
 */

int patch(FILE *in, FILE *out, FILE *diff) {
    if(in == NULL || out == NULL || diff == NULL) {
        print_error_msg(NULL, "file error: Error reading from an input stream or writing to an output stream\n");
        return -1;
    }
    if(is_empty_file(diff)) {
        int c;
        while((c = fgetc(in)) != EOF) {
            print_char(c, out);
        }
        return 0;
    }
    HUNK h_struct;
    int ol = 0, nl = 0;
    int c;
    int finished_reading_hunk = 1;
    int hn_res = EOF;
    while(1) {
        if(finished_reading_hunk == 1) {
            hn_res = hunk_next(&h_struct, diff);
            if(hn_res == ERR) {
                print_error_msg(&h_struct, "syntax error: failed to parse hunk object\n");
                return -1;
            }
            finished_reading_hunk = 0;
        }
        if(hn_res != EOF) {
            if((h_struct.type == HUNK_CHANGE_TYPE && (h_struct.old_start - 1 < ol && h_struct.new_start - 1 < nl))
                || (h_struct.type == HUNK_APPEND_TYPE && h_struct.old_start < ol)
                || (h_struct.type == HUNK_DELETE_TYPE && h_struct.new_start < nl)) {
                print_error_msg(&h_struct, "hunk error: line numbers failed to match as line numbers exceeded hunk header range\n");
                return -1;
            }
        }
        if(hn_res != EOF && h_struct.type == HUNK_CHANGE_TYPE && (h_struct.old_start - 1 == ol && h_struct.new_start - 1 == nl)) {
            int h_c;
            int del_ln_count = 0;
            int add_ln_count = 0;
            while(1) {
                h_c = hunk_getc(&h_struct, diff);
                if(h_c == ERR) {
                    print_error_msg(&h_struct, "syntax error: failed to read hunk data character\n");
                    return -1;
                }
                if(h_c == EOS) {
                    break;
                }
                c = fgetc(in);
                if(h_c != c) {
                    print_error_msg(&h_struct, "syntax error: deletion lines did not match!\n");
                    return -1;
                }
                if(h_c == '\n') {
                    ++ol;
                    ++del_ln_count;
                }
            }
            if(del_ln_count > (h_struct.old_end - h_struct.old_start + 1)) {
                print_error_msg(&h_struct, "line error: too many lines were found in this deletion section\n");
                return -1;
            }
            if(del_ln_count < (h_struct.old_end - h_struct.old_start + 1)) {
                print_error_msg(&h_struct, "line error: not enough lines were found in this deletion section\n");
                return -1;
            }
            while(1) {
                h_c = hunk_getc(&h_struct, diff);
                if(h_c == ERR) {
                    print_error_msg(&h_struct, "syntax error: failed to read hunk data character\n");
                    return -1;
                }
                if(h_c == EOS) {
                    break;
                }
                if(h_c == '\n') {
                    ++add_ln_count;
                    ++nl;
                }
                print_char(h_c, out);
            }
            if(add_ln_count > (h_struct.new_end - h_struct.new_start + 1)) {
                print_error_msg(&h_struct, "line error: too many lines were found in this addition section\n");
                return -1;
            }
            if(add_ln_count < (h_struct.new_end - h_struct.new_start + 1)) {
                print_error_msg(&h_struct, "line error: not enough lines were found in this addition section\n");
                return -1;
            }
            finished_reading_hunk = 1;
        } else if(hn_res != EOF && h_struct.type == HUNK_APPEND_TYPE && h_struct.old_start == ol) {
            int h_c;
            int add_ln_count = 0;
            while(1) {
               h_c = hunk_getc(&h_struct, diff); 
               if(h_c == ERR) {
                    print_error_msg(&h_struct, "syntax error: failed to read hunk data character\n");
                    return -1;
               }
               if(h_c == EOS) {
                   break;
               }
               if(h_c == '\n') {
                   ++add_ln_count;
                   ++nl;
               }
               print_char(h_c, out);
            }
            if(add_ln_count > (h_struct.new_end - h_struct.new_start + 1)) {
                print_error_msg(&h_struct, "line error: too many lines were found in this addition section\n");
                return -1;
            }
            if(add_ln_count < (h_struct.new_end - h_struct.new_start + 1)) {
                print_error_msg(&h_struct, "line error: not enough lines were found in this addition section\n");
                return -1;
            }
            finished_reading_hunk = 1;
        } else if(hn_res != EOF && h_struct.type == HUNK_DELETE_TYPE && h_struct.new_start == nl) {
            int h_c;
            int del_ln_count = 0;
            while(1) {
                h_c = hunk_getc(&h_struct, diff); 
                if(h_c == ERR) {
                    print_error_msg(&h_struct, "syntax error: failed to read hunk data character\n");
                    return -1;
                }
                if(h_c == EOS) {
                    break;
                }
                c = fgetc(in);
                if(h_c != c) {
                    print_error_msg(&h_struct, "syntax error: deletion lines did not match!\n");
                    return -1;
                }
                if(h_c == '\n') {
                    ++del_ln_count;
                    ++ol;
                }
            }
            if(del_ln_count > (h_struct.old_end - h_struct.old_start + 1)) {
                print_error_msg(&h_struct, "line error: too many lines were found in this deletion section\n");
                return -1;
            }
            if(del_ln_count < (h_struct.old_end - h_struct.old_start + 1)) {
                print_error_msg(&h_struct, "line error: not enough lines were found in this deletion section\n");
                return -1;
            }
            finished_reading_hunk = 1;
        } else {
            c = fgetc(in);
            if(feof(in)) {
                if(hn_res != EOF) {
                    print_error_msg(&h_struct, "hunk error: leftover hunk has not been processed!\n");
                    return -1;
                }
                break;
            }
            print_char(c, out);
            if(c == '\n') {
                ++nl;
                ++ol;
            }
        }
    }
    return 0;
}
