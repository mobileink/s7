/*
  test dsl:action->mibl, destructure-dsl-string, parse-pct-var
 */

#include <libgen.h>
#include <stdlib.h>             /* putenv */
#include <unistd.h>             /* getcwd */
#include <sys/errno.h>

#include "gopt.h"
#include "log.h"
#include "utarray.h"
#include "utstring.h"

#include "s7.h"

#include "unity.h"

s7_scheme *s7;
/* prototypes make the compiler happy */
void libc_s7_init(s7_scheme *sc);
void libdl_s7_init(s7_scheme *sc);
void libm_s7_init(s7_scheme *sc);

char *sexp_actual;
char *sexp_expected;

UT_string *setter;
UT_string *sexp;
s7_pointer actual;
s7_pointer expected;

bool verbose;
bool debug;

/* WARNING: setUp and tearDown are run once per test. */
void setUp(void) {
    /* log_info("setup"); */
}

void tearDown(void) {
    /* log_info("teardown"); */
}

void test_libc(void) {
    sexp_actual = "(fnmatch \"*.c\" \"s7.c\" FNM_PATHNAME)";
    sexp_expected = "0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));
}

void test_math(void) {
    sexp_actual = "(+ 2 3)";
    sexp_expected = "5";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));
}

void test_libm(void) {
    sexp_actual = "(pow 2 3)";
    sexp_expected = "8.0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));

    sexp_actual = "(ceil 2.3)";
    sexp_expected = "3.0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));

    sexp_actual = "(floor 2.3)";
    sexp_expected = "2.0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));

    sexp_actual = "(fmod 7 2)";
    sexp_expected = "1.0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));

    sexp_actual = "(fmod 7 3)";
    sexp_expected = "1.0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));
}

void test_regex(void) {
    sexp_actual = ""
        "(let* ((rg (regex.make)) "
        "       (_ (regcomp rg \"a.b\" 0))) "
        "   (let ((res (regexec rg \"acb\" 0 0))) "
        "      (regfree rg) "
        "      res))"
        ;
    sexp_expected = "0";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));

    /* capture - 1st match is whole thing, 2nd is subexpr */
#   define regex "\"a([0-9]+)b\""
    sexp_actual = ""
        "(let* ((s \"a123b\") "
        "       (rg (regex.make)) "
        "       (_ (regcomp rg " regex " REG_EXTENDED)) "
        "       (nmatch 2)) "
        "   (let* ((res (regexec rg s nmatch 0)) "
        "          (match-so (int-vector-ref res 2)) "
        "          (match-eo (int-vector-ref res 3)) "
        "          (match (substring s match-so match-eo))) "
        "      (regfree rg) "
        "      match))"
        ;
    sexp_expected = "\"123\"";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));

    /* from s7test.scm */
    /* not on macos: */
    /* sexp_actual = "" */
    /*     "(let* ((rg (regex.make)) " */
    /*     "       (_ (regcomp rg \"colou\\?r\" 0))) " */
    /*     "   (let ((res (regexec rg \"The color green\" 1 0))) " */
    /*     "      (regfree rg) " */
    /*     "      res))" */
    /*     ; */
    /* sexp_expected = "#i(4 9)"; */
    /* utstring_renew(sexp); */
    /* utstring_printf(sexp, "%s", sexp_actual); */
    /* actual = s7_eval_c_string(s7, utstring_body(sexp)); */
    /* expected = s7_eval_c_string(s7, sexp_expected); */
    /* TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected)); */

#   define regex2 "\"(ba(na)*s |nefer(ti)* )*\""
    sexp_actual = ""
        "(let* ((s \"bananas nefertiti\") "
        "       (rg (regex.make)) "
        "       (_ (regcomp rg " regex2 " REG_EXTENDED)) "
        "       (nmatch 3)) "
        "   (let ((res (regexec rg s nmatch 0))) "
        "      (regfree rg) "
        "      res))"
        ;
    sexp_expected = "#i(0 8 0 8 4 6)";
    utstring_renew(sexp);
    utstring_printf(sexp, "%s", sexp_actual);
    actual = s7_eval_c_string(s7, utstring_body(sexp));
    expected = s7_eval_c_string(s7, sexp_expected);
    TEST_ASSERT_TRUE(s7_is_equal(s7, actual, expected));
}

void _print_usage(void) {
    printf("Usage:\t$ bazel test <tgt> [flags, options]\n");
    printf("Flags\n");
    printf("\t-d, --debug\t\tEnable all debugging flags.\n");
    printf("\t--debug-config\t\tEnable all config debugging flags.\n");
    printf("\t--debug-scm\t\tEnable all scheme debugging flags.\n");
    printf("\t-t, --trace\t\tEnable trace flags.\n");
    printf("\t-v, --verbose\t\tEnable verbosity. Repeatable.\n");

    printf("Options:\n");
    /* printf("\t-D | -log <arg>\t\tLog <arg> (parsetree, mibl, or starlark}) to stdout.\n"); */

    printf("\t-r, --root <arg>"
           "\tStart traversal at <arg> (path relative to cwd).\n");
    printf("\t-p, --pkg <arg>"
           "\t\tProcess only <arg> (relative root path).\n");
}

enum OPTS {
    OPT_ROOT = 0,
    OPT_PKG,
    OPT_PACKAGE,

    FLAG_HELP,
    FLAG_DEBUG,
    FLAG_DEBUG_CONFIG,
    FLAG_DEBUG_MIBLRC,
    FLAG_DEBUG_MIBL_CRAWL,
    FLAG_DEBUG_SCM,
    FLAG_DEBUG_SCM_LOADS,

    FLAG_EMIT_PARSETREE,        /* config load_project to emit PARSETREE.mibl */

    FLAG_SHOW_CONFIG,
    FLAG_TRACE,
    FLAG_VERBOSE,

    LAST
};

static struct option options[] = {
    /* 0 */
    [OPT_ROOT] = {.long_name="root",.short_name='r',
                  .flags=GOPT_ARGUMENT_REQUIRED},
    [OPT_PKG] = {.long_name="pkg",.short_name='p',
                 .flags=GOPT_ARGUMENT_REQUIRED
    },
    [FLAG_DEBUG] = {.long_name="debug",.short_name='d',
                    .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_DEBUG_CONFIG] = {.long_name="debug-config",
                           .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_DEBUG_MIBLRC] = {.long_name="debug-miblrc",
                           .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_DEBUG_MIBL_CRAWL] = {.long_name="debug-mibl-crawl",
                               .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_DEBUG_SCM] = {.long_name="debug-scm", .short_name = 'D',
                        .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_DEBUG_SCM_LOADS] = {.long_name="debug-scm-loads",
                              .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_EMIT_PARSETREE] = {.long_name="emit-parsetree",
                              .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_SHOW_CONFIG] = {.long_name="show-config",
                          .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_TRACE] = {.long_name="trace",.short_name='t',
                    .flags=GOPT_ARGUMENT_FORBIDDEN},
    [FLAG_VERBOSE] = {.long_name="verbose",.short_name='v',
                      .flags=GOPT_ARGUMENT_FORBIDDEN | GOPT_REPEATABLE},
    [FLAG_HELP] = {.long_name="help",.short_name='h',
                   .flags=GOPT_ARGUMENT_FORBIDDEN},
    [LAST] = {.flags = GOPT_LAST}
};

void _set_options(struct option options[])
{
    if (options[FLAG_HELP].count) {
        _print_usage();
        exit(EXIT_SUCCESS);
    }

    if (options[FLAG_VERBOSE].count) {
        log_info("verbose ct: %d", options[FLAG_VERBOSE].count);
        verbose = true;
    }
}

int main(int argc, char **argv)
{
    /* log_debug("ARGV[0]: %s", argv[0]); */
    /* log_debug("CWD: %s", getcwd(NULL, 0)); */

    argc = gopt (argv, options);
    (void)argc;
    gopt_errors (argv[0], options);

    _set_options(options);

    s7 = s7_init();
    libc_s7_init(s7);
    libdl_s7_init(s7);
    libm_s7_init(s7);

    char *script_dir = "./test";
    s7_pointer newpath;
    newpath =  s7_add_to_load_path(s7, script_dir);
    (void)newpath;

    /* debugging: */
    /* s7_pointer loadpath = s7_load_path(s7); */
    /* char *s = s7_object_to_c_string(s7, loadpath); */
    /* printf("load path: %s\n", s); */
    /* free(s); */

    utstring_new(sexp);

    UNITY_BEGIN();

    RUN_TEST(test_libc);
    RUN_TEST(test_math);
    RUN_TEST(test_libm);
    RUN_TEST(test_regex);

    /* utstring_free(sexp); */
    return UNITY_END();
}
