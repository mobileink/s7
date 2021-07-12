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

/* #include "bazel.h" */

#include "cstruct.h"
/* #include "cstruct_s7.h" */

int main(int argc, char *argv[]) // , char **envp)
{
    int opt;
    int testcase;

    char *script_dir = "./demos/cstruct";

    s7_scheme *s7;
    s7 = s7_init();                 /* initialize the interpreter */

    s7_pointer newpath;
    newpath =  s7_add_to_load_path(s7, script_dir);

    /* debugging: */
    /* s7_pointer loadpath = s7_load_path(s7); */
    /* printf("load path: %s\n", s7_object_to_c_string(s7, loadpath)); */

    int rc;
    rc = run_cstruct(s7);
    printf("return code: %d\n", rc);
    s7_quit(s7);
    return rc;
}
