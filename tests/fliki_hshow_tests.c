#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <string.h>

#include "fliki.h"
#include "global.h"

// of course hunk_next and hunk_getc are required to work before this one
Test(hunkshow_test_suite, hunk_show_basic_good_input) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/hunkshowinputs/basic_good_input.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }

    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    for(int i = 0; i < 14; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("err was found too early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    cr_assert_eq(hunk_deletions_buffer[0], 0xE, "Got %d but wanted %d", hunk_deletions_buffer[0], 0xE);
    cr_assert_eq(hunk_deletions_buffer[1], 0, "Got %d but wanted %d", hunk_deletions_buffer[1], 0);
    cr_assert_eq(hunk_deletions_buffer[16], 0, "Got %d but wanted %d", hunk_deletions_buffer[16], 0);
    cr_assert_eq(hunk_deletions_buffer[17], 0, "Got %d but wanted %d", hunk_deletions_buffer[17], 0);

    char str1[15];

    str1[14] = 0;

    for(int i = 2; i < 16; ++i) {
        str1[i - 2] = hunk_deletions_buffer[i];
    }

    int val = strcmp(str1, "i don't exist\n");
    cr_assert_eq(val, 0, "Got %d but wanted %d", val, 0);

    for(int i = 0; i < 45; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("err was found too early");
        }
    }

    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    cr_assert_eq(hunk_additions_buffer[0], 26, "Got %d but wanted %d", hunk_additions_buffer[0], 26);
    cr_assert_eq(hunk_additions_buffer[1], 0, "Got %d but wanted %d", hunk_additions_buffer[1], 0);

    cr_assert_eq(hunk_additions_buffer[28], 9, "Got %d but wanted %d", hunk_additions_buffer[28], 9);
    cr_assert_eq(hunk_additions_buffer[29], 0, "Got %d but wanted %d", hunk_additions_buffer[29], 0);

    cr_assert_eq(hunk_additions_buffer[39], 10, "Got %d but wanted %d", hunk_additions_buffer[39], 10);
    cr_assert_eq(hunk_additions_buffer[40], 0, "Got %d but wanted %d", hunk_additions_buffer[40], 0);

    cr_assert_eq(hunk_additions_buffer[51], 0, "Got %d but wanted %d", hunk_additions_buffer[51], 0);
    cr_assert_eq(hunk_additions_buffer[52], 0, "Got %d but wanted %d", hunk_additions_buffer[52], 0);
}

Test(hunkshow_test_suite, hunk_show_size_correct) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/hunkshowinputs/input1.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }

    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    for(int i = 0; i < 412; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("err was found too early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    cr_assert_eq(0xFF & hunk_additions_buffer[0], 0x9C, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[0], 0x9C);
    cr_assert_eq(0xFF & hunk_additions_buffer[1], 0x1, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[1], 0x1);
}

Test(hunkshow_test_suite, hunk_show_last_two_bytes) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/hunkshowinputs/input2.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }

    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    for(int i = 0; i < 508; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("err was found too early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    cr_assert_eq(0xFF & hunk_additions_buffer[0], 0xFC, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[0], 0xFC);
    cr_assert_eq(0xFF & hunk_additions_buffer[1], 0x1, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[1], 0x1);

    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 2], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 2], 0x0);
    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 1], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 1], 0x0);

    FILE *new_write_file = fopen("./testfiles/hunkshowinputs/input2_written.diff", "w"); 
    hunk_show(&mock_hunk, new_write_file);
    fclose(new_write_file);

    char *cmp = "cmp testfiles/hunkshowinputs/input2_actual1.diff testfiles/hunkshowinputs/input2_written.diff";

    int return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(hunkshow_test_suite, hunk_show_last_four_bytes) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/hunkshowinputs/input3.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }

    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    for(int i = 0; i < 506; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("err was found too early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    cr_assert_eq(0xFF & hunk_additions_buffer[0], 0xFA, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[0], 0xFA);
    cr_assert_eq(0xFF & hunk_additions_buffer[1], 0x1, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[1], 0x1);

    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 4], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 4], 0x0);
    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 3], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 3], 0x0);
    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 2], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 2], 0x0);
    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 1], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 1], 0x0);

    FILE *new_write_file = fopen("./testfiles/hunkshowinputs/input3_written.diff", "w"); 
    hunk_show(&mock_hunk, new_write_file);
    fclose(new_write_file);

    char *cmp = "cmp testfiles/hunkshowinputs/input3_actual.diff testfiles/hunkshowinputs/input3_written.diff";

    int return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(hunkshow_test_suite, hunk_show_complete_exceed) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/hunkshowinputs/input4.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }

    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    for(int i = 0; i < 1024; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("err was found too early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    cr_assert_eq(0xFF & hunk_additions_buffer[0], 0xFC, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[0], 0xFC);
    cr_assert_eq(0xFF & hunk_additions_buffer[1], 0x1, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[1], 0x1);

    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 2], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 2], 0x0);
    cr_assert_eq(hunk_additions_buffer[HUNK_MAX - 1], 0, "Got 0x%x but wanted 0x%x", 0xFF & hunk_additions_buffer[HUNK_MAX - 1], 0x0);

    FILE *mock_diff_file_new_write = fopen("./testfiles/hunkshowinputs/exceed_write_output1.diff", "w");
    hunk_show(&mock_hunk, mock_diff_file_new_write);
    fclose(mock_diff_file_new_write);

    char *cmp = "cmp testfiles/hunkshowinputs/exceed_write_output1.diff testfiles/hunkshowinputs/exceed_write_actual1.diff";

    int return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}
