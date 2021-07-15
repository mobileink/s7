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

#include "cstruct.h"
#include "cstruct_s7.h"

struct cstruct_s cs_static = {
                       .c = 'x',
                       .str = "hello",
                       .b = true,
                       .i = 1,
                       .l = 999,
                       .f = 3.0,
                       .d = 999.999,
};

struct cstruct_s *cstruct_init(struct cstruct_s *cs,
                               char c, unsigned char bits,
                               char *s,
                               bool b, bool *pb,
                               short sh, short *psh,
                               int i, int *pi,
                               long l, long *pl,
                               long ll, long *pll,
                               float f, float *pf,
                               double d, double *pd)
{
    cs->c = c;
    cs->bits = bits;

    int len = strlen(s);
    cs->str = calloc(1, len);
    strncpy(cs->str, s, len);

    cs->sh_rt = sh;
    cs->psh_rt = calloc(1, sizeof(short));
    if (psh == NULL)
        *cs->psh_rt = 0;
    else
        *cs->psh_rt = *pi;

    cs->i = i;
    cs->pi = calloc(1, sizeof(int));
    if (pi == NULL)
        *cs->pi = 0;
    else
        *cs->pi = *pi;

    cs->l = l;
    cs->pl = calloc(1, sizeof(long));
    if (pl == NULL)
        *cs->pl = 0;
    else
        *cs->pl = *pl;

    cs->ll = ll;
    cs->pll = calloc(1, sizeof(long long));
    if (pll == NULL)
        *cs->pll = 0;
    else
        *cs->pl = *pl;

    cs->f = f;
    cs->pf = calloc(1, sizeof(float));
    if (pf == NULL)
        *cs->pf = 0.0;
    else
        *cs->pf = *pf;

    cs->d = d;
    cs->pd = calloc(1, sizeof(double));
    if (pd == NULL)
        *cs->pd = 0.0;
    else
        *cs->pd = *pd;
    return cs;
}

struct cstruct_s *cstruct_init_default(struct cstruct_s *cs)
{
    log_debug("cstruct_init_default");
    cs->c = 'a';
    cs->bits = 0xFF;

    cs->str = calloc(1, 6);
    strncpy(cs->str, "hello", 6);

    cs->b = false;
    cs->pb = calloc(1, sizeof(unsigned char));
    *cs->pb = false;

    cs->sh_rt = 7;
    cs->psh_rt = calloc(1, sizeof(short));
    *cs->psh_rt = 8;

    cs->i = 1;
    cs->pi = calloc(1, sizeof(int));
    *cs->pi = 2;

    cs->l = 3;
    cs->pl = calloc(1, sizeof(long));
    *cs->pl = 4;

    cs->ll = 3333;
    cs->pll = calloc(1, sizeof(long long));
    *cs->pll = 4444;

    cs->f = 5.0;
    cs->pf = calloc(1, sizeof(float));
    *cs->pf = 6.0;

    cs->d = 7;
    cs->pd = calloc(1, sizeof(double));
    *cs->pd = 8;

    return cs;
}

struct cstruct_s *cstruct_copy(struct cstruct_s *src,
                               struct cstruct_s *dest)
{
    dest->c = src->c;
    dest->bits = src->bits;

    free(dest->str);
    int len = strlen(src->str);
    dest->str = calloc(1, len);
    strncpy(dest->str, src->str, len);

    dest->sh_rt = src->sh_rt;
    *dest->psh_rt = *src->psh_rt;

    dest->i = src->i;
    *dest->pi = *src->pi;

    dest->l = src->l;
    *dest->pl = *src->pl;

    dest->ll = src->ll;
    *dest->pll = *src->pll;

    dest->f = src->f;
    *dest->pf = *src->pf;

    dest->d = src->d;
    *dest->pd = *src->pd;

    return dest;
}

void cstruct_free(struct cstruct_s *cs)
{
    log_debug("cstruct_free");
    free(cs->str);
    free(cs->pi);
    free(cs);
}

int run_cstruct(s7_scheme *s7)
{
    log_debug("run_cstruct");
    /* struct cstruct_s *cs_dyn = calloc(sizeof(struct cstruct_s), 1); */

    s7_int cstruct_t = configure_s7_cstruct_type(s7);
    static char *load_script = "cstruct.scm";

    s7_pointer lf;
    lf =  s7_load(s7, load_script);
    printf("load result: %s\n",
           s7_object_to_c_string(s7, lf));

    struct cstruct_s  *cs;
    cstruct_init(cs,
                 'X', // char c,
                 0xAB, // unsigned char bits,
                 "howdy", //  char *s,
                 true, NULL, // bool b, bool *bp,
                 21, NULL, // short sh, short *psh,
                 3, NULL, // int i, int *pi,
                 5, NULL, // long l, long *pl,
                 5555, NULL, // long long ll, long long *pll,
                 12.0, NULL, // float f, float *pf,
                 37.1, NULL // double d, double *pd
                 );

    s7_pointer new_cstruct_s7 = s7_make_c_object(s7, cstruct_t,
                                                 (void *)cs);

    s7_pointer args =  s7_list(s7,
                               1, //num_values,
                               new_cstruct_s7);
                               /* s7_make_integer(s7, 1)); */
                               /* s7_make_integer(s7, 2), */
                               /* s7_make_integer(s7, 3)); */

    log_debug("calling cstruct handler");
    s7_pointer result = s7_call(s7,
                                s7_name_to_value(s7, "handler"),
                                args);
    /* free(cs_dyn); */
    return 0;
}
