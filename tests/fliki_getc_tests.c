#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "fliki.h"
#include "global.h"

Test(hunk_getc_suite, hunk_getc_basic_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/keebspair/keeb.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, '"', "Got %c but wanted %c", c, '"');
}

Test(hunk_getc_suite, hunk_getc_fail1_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/keebspair/keeb.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

// hunk isn't available
Test(hunk_getc_suite, hunk_getc_failnohunkavailable_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/keebspair/keeb.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

// continuously return ERR
Test(hunk_getc_suite, hunk_getc_fail_returnERR_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/keebspair/badkeeb1.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

// should return 2 EOS
Test(hunk_getc_suite, hunk_getc_EOS_change_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/keebspair/keeb.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 57; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == EOS) {
            cr_assert_fail("did not expect EOS this early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    for(i = 0; i < 75; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == EOS) {
            cr_assert_fail("did not expect EOS this early");
        }
    }
    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);
}

Test(hunk_getc_suite, hunk_getc_EOSERR_append_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff01.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 7; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == EOS) {
            cr_assert_fail("did not expect EOS this early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_EOSERR_delete_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff01.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 7; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == EOS) {
            cr_assert_fail("did not expect EOS this early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badappend1_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff03.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 7; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange1_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff04.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange2_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff05.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 3; ++i) {
        // TODO mgiht have to change depending on stark's EOS answer? 
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange3_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff06.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 5; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange4_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff07.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange5_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff08.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange6_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff09.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange7_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff10.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int i;
    for(i = 0; i < 9; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange8_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff11.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int i;
    for(i = 0; i < 5; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange9_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff12.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int i;
    int val;

    for(i = 0; i < 11; ++i) {
        val = hunk_getc(&mock_hunk, mock_diff_file);
        if(val == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    cr_assert_eq(val, EOS, "Got %d but wanted %d", hn_code, EOS);

    for(; i < 20; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange10_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff13.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int i;

    for(i = 0; i < 5; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange11_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff14.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int i;

    for(i = 0; i < 8; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badappend2_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff15.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_baddelete1_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff16.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange12_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff17.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badchange13_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff18.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int i;

    for(i = 0; i < 4; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int cur_eos = hunk_getc(&mock_hunk, mock_diff_file); 
    cr_assert_eq(cur_eos, EOS, "Got %d but wanted %d", cur_eos, EOS);

    for(i = 0; i < 6; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    ///* now this tests if the next hunk is actually taken which it must be taken*/
    hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    cr_assert_eq(mock_hunk.old_start, 5, "Got %d but wanted %d", mock_hunk.old_start, 5);
    cr_assert_eq(mock_hunk.old_end, 5, "Got %d but wanted %d", mock_hunk.old_end, 5);
    cr_assert_eq(mock_hunk.type, 1, "Got %d but wanted %d", mock_hunk.type, 1);
    cr_assert_eq(mock_hunk.new_start, 4, "Got %d but wanted %d", mock_hunk.new_start, 4);
    cr_assert_eq(mock_hunk.new_end, 4, "Got %d but wanted %d", mock_hunk.new_end, 4);

    /* you should also be able to get the next character! */
    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, 'v', "Got %d but wanted %d", c, 'v');
}

Test(hunk_getc_suite, hunk_getc_badappend3_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff19.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_baddelete2_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff20.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badappend4_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff21.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);
}

Test(hunk_getc_suite, hunk_getc_baddelete3_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff22.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);

    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);
}

Test(hunk_getc_suite, hunk_getc_baddelete4_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff23.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 7; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}

Test(hunk_getc_suite, hunk_getc_badappend5_test) {
    HUNK mock_hunk;
    FILE *mock_diff_file = fopen("./testfiles/badgetcinputs/baddiff24.diff", "r");
    if(mock_diff_file == NULL) {
        cr_assert_fail("ensure you have this diff file");
    }
    int hn_code = hunk_next(&mock_hunk, mock_diff_file);
    cr_assert_eq(hn_code, 0, "Got %d but wanted %d", hn_code, 0);
    int i;
    for(i = 0; i < 7; ++i) {
        if(hunk_getc(&mock_hunk, mock_diff_file) == ERR) {
            cr_assert_fail("did not expect ERR this early");
        }
    }
    int c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, EOS, "Got %d but wanted %d", c, EOS);

    c = hunk_getc(&mock_hunk, mock_diff_file);
    cr_assert_eq(c, ERR, "Got %d but wanted %d", c, ERR);
}
