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

#include "s7.h"

#include "bazel.h"
#include "load.h"

int run_load(s7_scheme *s7)
{
    /* printf("run_load\n"); */
    /* s7_pointer newpath = s7_load_path(s7); */
    /* printf("load_path result: %s\n", s7_object_to_c_string(s7, newpath)); */

    /* cstruct.scm, as a runtime dep, must be listed in 'data' attrib of
       bazel build rule */
    static char *load_script = "load.scm";

    s7_pointer lf;
    lf =  s7_load(s7, load_script);

    printf("calling load handler\n");
    s7_pointer args =  s7_list(s7,
                               3, //num_values,
                               s7_make_integer(s7, 1),
                               s7_make_integer(s7, 2),
                               s7_make_integer(s7, 3));
    s7_pointer result = s7_call(s7,
                                s7_name_to_value(s7, "handler"),
                                args);

    return 0;
}
