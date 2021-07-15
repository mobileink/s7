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

/* #include "cstruct.h" */
#include "load.h"
#include "bazel.h"

void usage(void)
{
    printf("Usage: bazel run demos:driver -- -d <demo>\n\n\tdemos: 1 (more to follow)\n");
}

int main(int argc, char *argv[]) // , char **envp)
{
    /* for (char **env = envp; *env != 0; env++) { */
    /*     char *thisEnv = *env; */
    /*     printf("%s\n", thisEnv); */
    /* } */
    /* return 0; */

    int opt;
    int demo;

    char *script_dir = "./demos";

    /* load.scm, as a runtime dep, must be listed in 'data' attrib of
       bazel build rule */
    /* char *load_script = "load.scm"; */

    /* char *build_file = NULL; */

    if (argc < 2) {
        usage();
        exit(EXIT_FAILURE);
    }
    while ((opt = getopt(argc, argv, "d:hv")) != -1) {
        switch (opt) {
        case 'd':
            demo = atoi(optarg);
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'v':
            printf("verbose option (unimplemented) ");
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }
    printf("demo %d\n", demo);

    s7_scheme *s7;
    s7 = s7_init();                 /* initialize the interpreter */

    s7_pointer newpath;
    newpath =  s7_add_to_load_path(s7, script_dir);

    /* debugging: */
    s7_pointer loadpath = s7_load_path(s7);
    printf("load path: %s\n", s7_object_to_c_string(s7, loadpath));

    int rc = 0;
    switch(demo) {
    case 1:
        rc = run_load(s7);
        break;
    /* case 2: */
    /*     rc = run_cstruct(s7); */
    /*     break; */
    default:
        printf("unknown demo %d:\n(sorry, for now we have only one: 1", demo);
        usage();
        return -1;
    }
    printf("return code: %d\n", rc);
    s7_quit(s7);
    return rc;
}
