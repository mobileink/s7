#include <errno.h>
#include <fcntl.h>
#include <libgen.h>

#if INTERFACE
#ifdef LINUX                    /* FIXME */
#include <linux/limits.h>
#else // FIXME: macos test
#include <limits.h>
#endif
#endif
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>

#include "log.h"
#include "s7.h"

/* #include "bazel.h" */
#include "cstruct.h"
#include "cstruct_s7.h"

struct cstruct_s cs_static = {
                       .c = 'x',
                       .s = "hello",
                       .b = true,
                       .i = 1,
                       .l = 999,
                       .f = 3.0,
                       .d = 999.999,
};

int cstruct_init(struct cstruct_s *cs,
                 char c, char *s,
                 bool b, bool *bp,
                 int i, int *pi,
                 long l, long *pl,
                 float f, float *pf,
                 double d, double *pd)
{
    cs->c = c;
    int len = strlen(s);
    cs->s = calloc(1, len);
    strncpy(cs->s, s, len);
    cs->i = i;
    cs->pi = calloc(1, sizeof(int));
    cs->pi = *pi;
    cs->l = l;
    //cs->pl
    cs->f = f;
    //cs->pf
    cs->d = d;
    //cs->pd
}

void cstruct_free(struct cstruct_s *cs)
{
    log_debug("cstruct_free");
    free(cs->s);
    free(cs->pi);
    free(cs);
}

int run_cstruct(s7_scheme *s7)
{
    log_debug("run_cstruct");
    /* struct cstruct_s *cs_dyn = calloc(sizeof(struct cstruct_s), 1); */

    int rc = configure_s7_cstruct_type(s7);
    static char *load_script = "cstruct.scm";

    s7_pointer lf;
    lf =  s7_load(s7, load_script);

    log_debug("calling cstruct handler");
    s7_pointer args =  s7_list(s7,
                               3, //num_values,
                               s7_make_integer(s7, 1),
                               s7_make_integer(s7, 2),
                               s7_make_integer(s7, 3));
    s7_pointer result = s7_call(s7,
                                s7_name_to_value(s7, "handler"),
                                args);
    /* free(cs_dyn); */
    return 0;
}
