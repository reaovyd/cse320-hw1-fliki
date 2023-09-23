#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "fliki.h"
#include "global.h"

Test(patch_tests_extended_suite, fliki_cpairs_test) {
    char *cmd = "bin/fliki testfiles/cpairs/cpair.c.diff < testfiles/cpairs/cpair1.c > testfiles/cpairs/cpair_output.c";
    char *cmp = "cmp testfiles/cpairs/cpair_output.c testfiles/cpairs/cpair2.c";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_fruitchangepair_test) {
    char *cmd = "bin/fliki testfiles/fruit_changepair/fruit_change.diff < testfiles/fruit_changepair/fruit_change1 > testfiles/fruit_changepair/fruit_change_output";
    char *cmp = "cmp testfiles/fruit_changepair/fruit_change_output testfiles/fruit_changepair/fruit_change2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_golanglargerpair_test) {
    char *cmd = "bin/fliki testfiles/golang_largerpair/golang_larger.go.diff < testfiles/golang_largerpair/golang_larger1.go > testfiles/golang_largerpair/golang_larger_output.go";
    char *cmp = "cmp testfiles/golang_largerpair/golang_larger_output.go testfiles/golang_largerpair/golang_larger2.go";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_golangpair_test) {
    char *cmd = "bin/fliki testfiles/golangpair/main.go.diff < testfiles/golangpair/main1.go > testfiles/golangpair/golang_output.go";
    char *cmp = "cmp testfiles/golangpair/golang_output.go testfiles/golangpair/main2.go";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_janejohnpair_test) {
    char *cmd = "bin/fliki testfiles/janejohnpair/janejohn.diff < testfiles/janejohnpair/janejohn1 > testfiles/janejohnpair/janejohn_output";
    char *cmp = "cmp testfiles/janejohnpair/janejohn2 testfiles/janejohnpair/janejohn_output";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_jjjpair_test) {
    char *cmd = "bin/fliki testfiles/jjj_appendpair/jjj_append.diff < testfiles/jjj_appendpair/jjj_append1 > testfiles/jjj_appendpair/jjj_append_output";
    char *cmp = "cmp testfiles/jjj_appendpair/jjj_append_output testfiles/jjj_appendpair/jjj_append2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_lorenchange_test) {
    char *cmd = "bin/fliki testfiles/loren_changepair/loren_change.diff < testfiles/loren_changepair/loren_change1 > testfiles/loren_changepair/loren_change_output";
    char *cmp = "cmp testfiles/loren_changepair/loren_change2 testfiles/loren_changepair/loren_change_output";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_lorenmany_test) {
    char *cmd = "bin/fliki testfiles/loren_manypair/loren_many.diff < testfiles/loren_manypair/loren_many1 > testfiles/loren_manypair/loren_many_output";
    char *cmp = "cmp testfiles/loren_manypair/loren_many_output testfiles/loren_manypair/loren_many2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_otherfruits_test) {
    char *cmd = "bin/fliki testfiles/other_fruitspair/other_fruits.diff < testfiles/other_fruitspair/other_fruits1 > testfiles/other_fruitspair/other_fruits_output";
    char *cmp = "cmp testfiles/other_fruitspair/other_fruits_output testfiles/other_fruitspair/other_fruits2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_speechother_test) {
    char *cmd = "bin/fliki testfiles/speechotherpair/speech_other.diff < testfiles/speechotherpair/speech_other1 > testfiles/speechotherpair/speech_other_output";
    char *cmp = "cmp testfiles/speechotherpair/speech_other_output testfiles/speechotherpair/speech_other2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_speech_test) {
    char *cmd = "bin/fliki testfiles/speechpair/speech.diff < testfiles/speechpair/speech1 > testfiles/speechpair/speech_output";
    char *cmp = "cmp testfiles/speechpair/speech_output testfiles/speechpair/speech2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_tqbf_test) {
    char *cmd = "bin/fliki testfiles/tqbf_changepair/tqbf_change.diff < testfiles/tqbf_changepair/tqbf_change1 > testfiles/tqbf_changepair/tqbf_change_output";
    char *cmp = "cmp testfiles/tqbf_changepair/tqbf_change_output testfiles/tqbf_changepair/tqbf_change2";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_bad_article_test) {
    char *cmd = "bin/fliki testfiles/badarticlepair/article.diff < testfiles/badarticlepair/article1 > testfiles/badarticlepair/article_output 2>/dev/null";
    char *cmp = "cmp testfiles/badarticlepair/article2 testfiles/badarticlepair/article_output 2 >/dev/null";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_bad_article_other_test) {
    char *cmd = "bin/fliki testfiles/badarticleotherpair/article.diff < testfiles/badarticleotherpair/article1 > testfiles/badarticleotherpair/article_output 2>/dev/null";
    char *cmp = "cmp testfiles/badarticleotherpair/article2 testfiles/badarticleotherpair/article_output 2 >/dev/null";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program output did not match reference output.");
}
// no new line at end
Test(patch_tests_extended_suite, fliki_bad_article_nonewline_test) {
    char *cmd = "bin/fliki testfiles/nonewlinearticlepair/article.diff < testfiles/nonewlinearticlepair/article1 > testfiles/nonewlinearticlepair/article_output 2>/dev/null";
    char *cmp = "cmp testfiles/nonewlinearticlepair/article2 testfiles/nonewlinearticlepair/article_output 2 >/dev/null";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program output did not match reference output.");
}

Test(patch_tests_extended_suite, fliki_nopatch_test) {
    char *cmd = "bin/fliki -n testfiles/loren_manypair/loren_many.diff < testfiles/loren_manypair/loren_many1 > testfiles/loren_manypair/loren_many_output_empty";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with 0x%x instead of EXIT_SUCCESS",
		 return_code);
    FILE *fptr = fopen("./testfiles/loren_manypair/loren_many_output_empty", "r");
    fgetc(fptr);
    if(!feof(fptr)) {
        cr_assert_fail("file is not empty?");
    }
}

Test(patch_tests_extended_suite, fliki_quiet_test) {
    char *cmd = "bin/fliki -q testfiles/nonewlinearticlepair/article.diff < testfiles/nonewlinearticlepair/article1 > testfiles/nonewlinearticlepair/article_output_quiet 2>testfiles/nonewlinearticlepair/article_errors";

    char *cmp = "cmp testfiles/nonewlinearticlepair/article2 testfiles/nonewlinearticlepair/article_output_quiet 2 >/dev/null";
    char *cmp2 = "cmp testfiles/nonewlinearticlepair/article_errors rsrc/empty 2 >/dev/null";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program output did not match reference output.");
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program output did not match reference output.");
}

// too many lines test in oen of the hunks
Test(patch_tests_extended_suite, fliki_bad_speech_toomuch_test) {
    char *cmd = "bin/fliki testfiles/badpatchinputs/speech_too_much.diff < testfiles/badpatchinputs/speech1 > /dev/null 2>&1";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
}
Test(patch_tests_extended_suite, fliki_bad_speech_toolittle_test) {
    char *cmd = "bin/fliki testfiles/badpatchinputs/speech_too_little.diff < testfiles/badpatchinputs/speech1 > /dev/null 2>&1";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
}

Test(patch_tests_extended_suite, fliki_append_toolittle_test) {
    char *cmd = "bin/fliki testfiles/badpatchinputs/append_too_little.diff < testfiles/badpatchinputs/append_too_little_2 > /dev/null 2>&1";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
}

Test(patch_tests_extended_suite, fliki_delete_toolittle_test) {
    char *cmd = "bin/fliki testfiles/badpatchinputs/delete_too_little.diff < testfiles/badpatchinputs/append_too_little_1 > /dev/null 2>&1";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
}

Test(patch_tests_extended_suite, fliki_append_toomuch_test) {
    char *cmd = "bin/fliki testfiles/badpatchinputs/appendtoomuch/append_too_much.diff < testfiles/badpatchinputs/appendtoomuch/append_too_much_1 > /dev/null 2>&1";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
}

Test(patch_tests_extended_suite, fliki_delete_toomuch_test) {
    char *cmd = "bin/fliki testfiles/badpatchinputs/deletetoomuch/delete_too_much.diff < testfiles/badpatchinputs/deletetoomuch/delete_too_much_2 > /dev/null 2>&1";

    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with 0x%x instead of EXIT_FAILURE",
		 return_code);
}
