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

/* #include "edit.h" */

/* UT_string *build_file; */

/* UT_string *buffer; */

int main(int argc, char *argv[]) // , char **envp)
{
    /* for (char **env = envp; *env != 0; env++) { */
    /*     char *thisEnv = *env; */
    /*     printf("%s\n", thisEnv); */
    /* } */
    /* return 0; */

    int opt;

    char *script_dir = "./demos";

    /* load.scm, as a runtime dep, must be listed in 'data' attrib of
       bazel build rule */
    char *load_script = "load.scm";

    /* char *build_file = NULL; */

    while ((opt = getopt(argc, argv, "f:s:u:hv")) != -1) {
        switch (opt) {
        /* case 'f': */
        /*     build_file = optarg; */
        /*     break; */
        case 's':
            printf("scheme script file: %s", optarg);
            load_script = optarg;
            break;
        /* case 'u': */
        /*     user_script_dir = optarg; */
        /*     break; */
        case 'h':
            printf("Help: ");
            exit(EXIT_SUCCESS);
        case 'v':
            printf("verbose option (unimplemented) ");
        default:
            printf("Usage: bazel run moonlark:edit -- [-f buildfile] [-l s7file]");
            exit(EXIT_FAILURE);
        }
    }
    /* if (utstring_len(build_file) == 0) { */
    /* if (build_file == NULL) { */
    /*     printf("-f <buildfile> must be provided."); */
    /*     exit(EXIT_FAILURE); */
    /* } */

    s7_scheme *s7;
    s7 = s7_init();                 /* initialize the interpreter */

    char *cwd = getcwd(NULL, 0);
    printf("cwd: %s\n", cwd);

    char *wd = getenv("BUILD_WORKING_DIRECTORY");
    if (wd) {
        printf("launched by `bazel run`: %s\n", wd);
        /* launched by bazel run cmd */

        /* char *bazel_script_dir = get_bazel_script_dir(callback_script_file); */
        /* s7lark_augment_load_path(s7, bazel_script_dir); */
        s7_pointer newlp;
        /* if (bazel_script_dir != NULL) { */
        newlp =  s7_add_to_load_path(s7, script_dir);
        /* } else */
        /*     printf("Could not find script dir: %s", callback_script_file); */

        /* user script dir is relative to launch dir; set it after chdir */
        /* chdir(wd); */

        /* if( access( user_script_dir, F_OK ) != 0 ) { */
        /*     log_warn("WARNING: user_luadir does not exist: %s", user_script_dir); */
        /* } */
        /* s7lark_augment_load_path(s7, user_script_dir); */

        /* debugging: */
        s7_pointer lp = s7_load_path(s7);
        printf("load path: %s\n", s7_object_to_c_string(s7, lp));

        /* s7lark_config_moonlark_table(L); */

        /* s7lark_load_script_file(L, load_script); */
        s7_pointer lf;
        /* if (load_script) { */
        /*     printf("loading user script: %s\n", load_script); */
        lf =  s7_load(s7, load_script);
        /* } else { */
        /*     printf("loading default script: %s\n", callback_script_file); */
        /*     if (!s7_load(s7, callback_script_file)) */
        /*         printf("load %s failed\n", callback_script_file); */
        /* } */
        printf("load result: %s\n", s7_object_to_c_string(s7, lf));
        /* fflush(stdout); */

        printf("calling handler\n");
        s7_pointer args =  s7_list(s7,
                                   3, //num_values,
                                   s7_make_integer(s7, 1),
                                   s7_make_integer(s7, 2),
                                   s7_make_integer(s7, 3));
        s7_pointer result = s7_call(s7,
                                    s7_name_to_value(s7, "handler"),
                                    args);

    } else {
        printf("BUILD_WORKING_DIRECTORY not found. This program is designed to be run from the root directory of a Bazel repo.\n");
    }

    /* now parse the file using libstarlark */
    /* struct parse_state_s *parse_state = starlark_parse_file(build_file); */
    /* printf("parsed file %s", parse_state->lexer->fname); */

    /* convert build file to Lua AST table */
    /* s7lark_ast2scm(s7, parse_state); */

    /* /\* call callback on sexp AST *\/ */
    /* s7lark_call_user_handler(s7, callback); */

    s7_quit(s7);
}
