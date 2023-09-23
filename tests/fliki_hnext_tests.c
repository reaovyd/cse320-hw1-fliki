#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "fliki.h"
#include "global.h"

Test(hunk_test_suite, hunk_next_regular) {
    HUNK mock_hunk;
    mock_hunk.serial = 0;
    FILE *mock_diff_file = fopen("./testfiles/cpairs/cpair.c.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    cr_assert_eq(mock_hunk.old_start, 4, "Got %d but wanted %d", mock_hunk.old_start, 4);
    cr_assert_eq(mock_hunk.old_end,   4, "Got %d but wanted %d", mock_hunk.old_end  , 4);
    cr_assert_eq(mock_hunk.type,      3, "Got %d but wanted %d", mock_hunk.type     , 3);
    cr_assert_eq(mock_hunk.new_start, 4, "Got %d but wanted %d", mock_hunk.new_start, 4);
    cr_assert_eq(mock_hunk.new_end,   4, "Got %d but wanted %d", mock_hunk.old_end  , 4);
}

// ensures you have EOF in ur hunk_next
Test(hunk_test_suite, hunk_next_eof) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/cpairs/cpair.c.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code;
    for(int i = 0; i < 10; ++i) {
        hn_code = hunk_next(&mock_hunk, mock_diff_file);
        //printf("%d %d %d %d %d\n", mock_hunk.old_start, mock_hunk.old_end, mock_hunk.type, mock_hunk.new_start, mock_hunk.new_end);
    }
    cr_assert_eq(hn_code, EOF, "Got %d but wanted %d", hn_code, EOF);
}

// when you hunk_next and then getc, but then hunk_next again and then getc, that final getc should be for the next hunk
Test(hunk_test_suite, hunk_next_getc_combo) {
    HUNK mock_hunk;
    mock_hunk.serial = 0;
    FILE *mock_diff_file = fopen("./testfiles/cpairs/cpair.c.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    cr_assert_eq(mock_hunk.old_start, 4, "Got %d but wanted %d", mock_hunk.old_start, 4);
    cr_assert_eq(mock_hunk.old_end,   4, "Got %d but wanted %d", mock_hunk.old_end  , 4);
    cr_assert_eq(mock_hunk.serial,    1, "Got %d but wanted %d", mock_hunk.serial, 1);
    cr_assert_eq(mock_hunk.type,      3, "Got %d but wanted %d", mock_hunk.type     , 3);
    cr_assert_eq(mock_hunk.new_start, 4, "Got %d but wanted %d", mock_hunk.new_start, 4);
    cr_assert_eq(mock_hunk.new_end,   4, "Got %d but wanted %d", mock_hunk.old_end  , 4);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, '#', "Got %d but wanted %d", c, '#');

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    cr_assert_eq(mock_hunk.old_start, 6, "Got %d but wanted %d", mock_hunk.old_start ,6);
    cr_assert_eq(mock_hunk.old_end,   7, "Got %d but wanted %d", mock_hunk.old_end   ,7);
    cr_assert_eq(mock_hunk.type,      3, "Got %d but wanted %d", mock_hunk.type      ,3);
    cr_assert_eq(mock_hunk.new_start, 6, "Got %d but wanted %d", mock_hunk.new_start ,6);
    cr_assert_eq(mock_hunk.new_end,   11, "Got %d but wanted %d", mock_hunk.old_end  ,11);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    cr_assert_eq(mock_hunk.old_start, 11, "Got %d but wanted %d", mock_hunk.old_start ,11);
    cr_assert_eq(mock_hunk.old_end,   13, "Got %d but wanted %d", mock_hunk.old_end   ,13);
    cr_assert_eq(mock_hunk.type,      3, "Got %d but wanted %d", mock_hunk.type      ,3);
    cr_assert_eq(mock_hunk.new_start, 15, "Got %d but wanted %d", mock_hunk.new_start ,15);
    cr_assert_eq(mock_hunk.new_end,   21, "Got %d but wanted %d", mock_hunk.old_end  ,21);

    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ' ', "Got %d but wanted %d", c, ' ');
}

// should fail since the first hunk is invalid
Test(hunk_test_suite, hunk_next_bad_parse1) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/cpair_bad_parse1.c.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

// tests an ERR in the middle
Test(hunk_test_suite, hunk_next_bad_parse2) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/cpair_bad_parse2.c.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

// these next tests are for the only 8 valid different combinations for hunks
Test(hunk_test_combination_suite, hunk_next_bad_combination1) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/bad_combination1.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_bad_combination2) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/bad_combination2.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_bad_combination3) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/bad_combination3.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_bad_combination4) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/bad_combination4.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

// these verify sizes
Test(hunk_test_combination_suite, hunk_next_bad_combination5) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/bad_combination5.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_bad_combination6) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/bad_combination6.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

// these next 4 are valid for change

Test(hunk_test_combination_suite, hunk_next_good_combo1) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/good_combination1.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
}

Test(hunk_test_combination_suite, hunk_next_good_combo2) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/good_combination2.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
}

Test(hunk_test_combination_suite, hunk_next_good_combo3) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/good_combination3.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
}

Test(hunk_test_combination_suite, hunk_next_good_combo4) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/good_combination4.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
}

Test(hunk_test_combination_suite, hunk_next_too_much_space_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/too_much_space.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_detect_bad_data) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/hnext_detect_bad_data.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_detect_bad_data2) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/hnext_detect_bad_data2.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

// test for top of the line ERR 

Test(hunk_test_combination_suite, hunk_next_horrible_input_but_works1) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/horrible_input_but_works1.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
}

Test(hunk_test_combination_suite, hunk_next_horrible_input_but_works2) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/horrible_input_but_works2.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_horrible_input_but_works3) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/horrible_input_but_works3.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, ERR, "Got %d but wanted %d", hn_code, ERR);
}

Test(hunk_test_combination_suite, hunk_next_horrible_input_but_works4) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/horrible_input_but_works4.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, EOF, "Got %d but wanted %d", hn_code, EOF);
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, EOF, "Got %d but wanted %d", hn_code, EOF);
}

Test(hunk_test_combination_suite, hunk_next_horrible_input_but_works5) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badinputs/horrible_input_but_works5.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, EOF, "Got %d but wanted %d", hn_code, EOF);
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, EOF, "Got %d but wanted %d", hn_code, EOF);
}
