#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "s7.h"

#include "cstruct.h"

static s7_int cstruct_t = 0;
static s7_pointer g_cstruct_methods;

/* forward decls */
/* section: identity */
static s7_pointer g_is_cstruct(s7_scheme *s7, s7_pointer args);

/* section: equality */
static bool       g_cstructs_are_eql(void *val1, void *val2);
static s7_pointer g_cstructs_are_equal(s7_scheme *s7, s7_pointer args);
static s7_pointer g_cstructs_are_equivalent(s7_scheme *s7, s7_pointer args);

/* section: getters and setters */
static s7_pointer g_cstruct_get(s7_scheme *s7, s7_pointer args);
static s7_pointer g_cstruct_set(s7_scheme *s7, s7_pointer args);

/* section: display */
static char *g_cstruct_display(s7_scheme *s7, void *value);
static char *g_cstruct_display_readably(s7_scheme *s7, void *value);

/* section: serialization */
static s7_pointer g_cstruct_to_string(s7_scheme *s7, s7_pointer args);

/* section: c-object construction */
static s7_pointer g_cstruct_copy(s7_scheme *s7, s7_pointer args);
static s7_pointer g_cstruct_init_from_s7(s7_scheme *s7, struct cstruct_s *cs, s7_pointer args);
static s7_pointer g_new_cstruct(s7_scheme *s7, s7_pointer args);

/* section: c-object destruction */
static s7_pointer g_destroy_cstruct(s7_pointer obj);

/* section: methods */
static s7_pointer g_cstruct_method(s7_scheme *s7, s7_pointer args);

/* section: extension methods */
/* extension methods extend standard Scheme procedures like 'length'
   and 'equals' to support custom c-types. */
void register_extension_methods(s7_scheme *s7, s7_int cstruct_t);

/* section: c-type configuration */
static int _register_cstruct_fns(s7_scheme *s7);
static int _make_c_type(s7_scheme *s7);
s7_int     configure_s7_cstruct_type(s7_scheme *s7); /* public */

/* section: gc */
static s7_pointer g_cstruct_gc_mark(s7_scheme *s7, s7_pointer p);

/* section: debugging */
void debug_print_s7(s7_scheme *s7, char *label, s7_pointer obj);

/* **************************************************************** */

/* **************************************************************** */
/* section: identity */
#define g_is_cstruct_help "(cstruct? obj) returns #t if obj is a cstruct."
#define g_is_cstruct_sig s7_make_signature(s7, 2, s7_make_symbol(s7, "boolean?"), s7_t(s7))
static s7_pointer g_is_cstruct(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_is_cstruct");
#endif
    return(s7_make_boolean(s7, s7_c_object_type(s7_car(args)) == cstruct_t));
}
/* /section: identity */

/* **************************************************************** */
/* section: equality */
static bool g_cstructs_are_eql(void *val1, void *val2)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstructs_are_eql");
#endif
    s7_int i, len;
    struct cstruct_s *b1 = (struct cstruct_s *)val1;
    struct cstruct_s *b2 = (struct cstruct_s *)val2;
    if (val1 == val2) return(true);
    //FIXME
    return(true);
}

static s7_pointer g_cstructs_are_equal(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstructs_are_equal");
#endif
    return(s7_make_boolean(s7, g_cstructs_are_eql((void *)s7_c_object_value(s7_car(args)), (void *)s7_c_object_value(s7_cadr(args)))));
}

static s7_pointer g_cstructs_are_equivalent(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstructs_are_equivalent");
#endif

#define g_cstructs_are_equivalent_help "(equivalent? cstruct1 cstruct2)"
    s7_pointer v1, v2, arg1, arg2;
    struct cstruct_s *g1, *g2;
    bool result;
    uint32_t gc1, gc2;
    size_t len;
    arg1 = s7_car(args);
    arg2 = s7_cadr(args);
    if (!s7_is_c_object(arg2))
        return(s7_f(s7));
    if (arg1 == arg2)
        return(s7_make_boolean(s7, true));
    if (s7_is_let(arg1))             /* (cstruct-let (cstruct)) */
        return(s7_make_boolean(s7, false));    /* checked == above */
    g1 = (struct cstruct_s *)s7_c_object_value(arg1);
    if (s7_c_object_type(arg2) != cstruct_t)
        return(s7_make_boolean(s7, false));
    g2 = (struct cstruct_s *)s7_c_object_value(arg2);
    /* len = g1->size; */
    /* if (len != g2->size) */
    /*     return(s7_make_boolean(s7, false)); */
    /* v1 = s7_make_float_vector_wrapper(s7, len, g1->data, 1, NULL, false); */
    /* gc1 = s7_gc_protect(s7, v1); */
    /* v2 = s7_make_float_vector_wrapper(s7, len, g2->data, 1, NULL, false); */
    /* gc2 = s7_gc_protect(s7, v2); */
    result = s7_is_equivalent(s7, v1, v2);
    s7_gc_unprotect_at(s7, gc1);
    s7_gc_unprotect_at(s7, gc2);
    return(s7_make_boolean(s7, result));
}
/* /section: equality */

/* **************************************************************** */
/* section: getters and setters */
s7_pointer cstruct_lookup_kw(s7_scheme *s7,
                             struct cstruct_s *cstruct, s7_pointer kw)
{
#ifdef DEBUG_TRACE
    log_debug("cstruct_lookup_kw");
#endif
    if (kw == s7_make_keyword(s7, "c")) {
        return s7_make_character(s7, cstruct->c);
    }
    if (kw == s7_make_keyword(s7, "s"))
        return s7_make_string(s7, cstruct->s);
    if (kw == s7_make_keyword(s7, "i"))
        return s7_make_integer(s7, cstruct->i);
    if (kw == s7_make_keyword(s7, "pi"))
        return s7_make_integer(s7, *cstruct->pi);
    if (kw == s7_make_keyword(s7, "l"))
        return s7_make_integer(s7, cstruct->l);
    if (kw == s7_make_keyword(s7, "pl"))
        return s7_make_integer(s7, *cstruct->pl);

    if (kw == s7_make_keyword(s7, "f"))
        return s7_make_real(s7, cstruct->f);
    if (kw == s7_make_keyword(s7, "pf"))
        return s7_make_real(s7, *cstruct->pf);

    if (kw == s7_make_keyword(s7, "d"))
        return s7_make_real(s7, cstruct->d);
    if (kw == s7_make_keyword(s7, "pd"))
        return s7_make_real(s7, *cstruct->pd);
}

/** g_cstruct_get

    function called when objects of type cstruct are evaluated as
    functions, i.e. when they occur in function position (car of a
    list).

    takes two args, a cstruct object and a keyword to look up in the object.
 */
#define G_CSTRUCT_GET_HELP "(cstruct-ref b i) returns the cstruct value at index i."
#define G_CSTRUCT_GET_SIG s7_make_signature(s7, 3, s7_t(s7), s7_make_symbol(s7, "cstruct?"), s7_make_symbol(s7, "integer?"))
static s7_pointer g_cstruct_get(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_get");
    debug_print_s7(s7, "cstruct_ref args: ", args);
#endif

    struct cstruct_s *g;
    size_t index;
    s7_int typ;
    s7_pointer obj;
    if (s7_list_length(s7, args) != 2)
        return(s7_wrong_number_of_args_error(s7, "cstruct-ref takes 2 arguments: ~~S", args));

    obj = s7_car(args);
    typ = s7_c_object_type(obj);
    if (typ != cstruct_t)
        return(s7_wrong_type_arg_error(s7, "cstruct-ref", 1, obj, "a cstruct"));
    g  = (struct cstruct_s *)s7_c_object_value(obj);

    if (s7_is_null(s7, s7_cdr(args))) /* this is for an (obj) test */
        return(s7_wrong_type_arg_error(s7, "cstruct-ref", 1, obj, "missing keyword arg"));
        /* return(s7_make_integer(s7, 32)); */

    /* kw arg = name of field, find it field in the object */
    /* symbol arg = name of method, find it in object's method table */
    s7_pointer arg = s7_cadr(args);
    if (s7_is_keyword(arg))
        return cstruct_lookup_kw(s7, g, arg);
    else {
        if (s7_is_symbol(arg)) { /* ((cstruct 'empty) b) etc */
            /* in this case, arg is a method sym, which we lookup in
               the method table of the object */
            s7_pointer val;
            /* g_cstruct_methods as env in which to lookup arg */
            val = s7_symbol_local_value(s7, arg, g_cstruct_methods);
            if (val != arg)
                return(val);
            else
                ; /* found self-referring method? method not found? */
        } else
            return(s7_wrong_type_arg_error(s7, "cstruct-ref",
                                           2, arg, "a kw or sym"));
    }
}

static s7_pointer g_cstruct_set(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_set");
#endif
#define g_cstruct_set_help "(cstruct-set! b i x) sets the cstruct value at index i to x."
#define g_cstruct_set_sig s7_make_signature(s7, 4, s7_make_symbol(s7, "float?"), s7_make_symbol(s7, "cstruct?"), s7_make_symbol(s7, "integer?"), s7_make_symbol(s7, "float?"))
    struct cstruct_s *g;
    s7_int index, typ;
    s7_pointer obj;
    return(s7_caddr(args)); //FIXME

    /* /\* c_object_set functions need to check that they have been passed the correct number of arguments *\/ */
    /* if (s7_list_length(s7, args) != 3) */
    /*     return(s7_wrong_number_of_args_error(s7, "cstruct-set! takes 3 arguments: ~~S", args)); */
    /* obj = s7_car(args); */
    /* typ = s7_c_object_type(obj); */
    /* if (typ != cstruct_t) */
    /*     return(s7_wrong_type_arg_error(s7, "cstruct-set!", 1, obj, "a cstruct")); */
    /* if (s7_is_immutable(obj)) */
    /*     return(s7_wrong_type_arg_error(s7, "cstruct-set!", 1, obj, "a mutable cstruct")); */
    /* if (!s7_is_integer(s7_cadr(args))) */
    /*     return(s7_wrong_type_arg_error(s7, "cstruct-set!", 2, s7_cadr(args), "an integer")); */
    /* g = (struct cstruct_s *)s7_c_object_value(obj); */
    /* index = s7_integer(s7_cadr(args)); */
    /* if ((index >= 0) && (index < g->size)) */
    /*     { */
    /*         g->data[index] = s7_number_to_real(s7, s7_caddr(args)); */
    /*         return(s7_caddr(args)); */
    /*     } */
    /* return(s7_out_of_range_error(s7, "cstruct-set", 2, s7_cadr(args), "should be less than cstruct length")); */
}
/* /section: getters and setters */

/* **************************************************************** */
/* section: display */
static char *g_cstruct_display(s7_scheme *s7, void *value)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_display");
#endif

    char workbuf[1024] = { '\0', };

    char *msg = "FIXME g_cstruct_display!";
    char *buf, *flt;

#define WORKPTR (workptr = workbuf + strlen(workbuf))

    /* s7_int i, len, prec, old_len, loc, bytes; */
    struct cstruct_s *cs = (struct cstruct_s *)value;
    char *workptr = workbuf;
    sprintf(workptr, "#cstruct<\n"); WORKPTR;
    sprintf(workptr, "  c = '%c',\n", cs->c); WORKPTR;
    sprintf(workptr, "  s = \"%s\",\n", cs->s); WORKPTR;

    sprintf(workptr, "  i = %d,\n", cs->i); WORKPTR;
    if (cs->pi == NULL)
        {sprintf(workptr, " pi = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pi = %d,\n", *(cs->pi)); WORKPTR;}

    sprintf(workptr, "  l = %ld,\n", cs->l); WORKPTR;
    if (cs->pl == NULL)
        {sprintf(workptr, " pl = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pl = %d,\n", *(cs->pl)); WORKPTR;}

    sprintf(workptr, "  f = %f,\n", cs->f); WORKPTR;
    if (cs->pd == NULL)
        {sprintf(workptr, " pf = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pf = %f,\n", *(cs->pf)); WORKPTR;}
    sprintf(workptr, ">#\n", cs->c);

    sprintf(workptr, "  d = %f,\n", cs->d); WORKPTR;
    if (cs->pd == NULL)
        {sprintf(workptr, " pd = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pd = %f,\n", *(cs->pd)); WORKPTR;}
    sprintf(workptr, ">#");

    int len = strlen(workbuf);
    buf = (char *)calloc(1, len);
    strncpy(buf, workbuf, len);

    // printf("5 WWWW: %s\n", workbuf);
    // printf("buf: %s\n", buf);
    return buf;
    /* prec = s7_integer(s7_let_field_ref(s7, s7_make_symbol(s7, "float-format-precision"))); */
    /* if (prec >= 16) prec = 3; */
    /* len = b->size; */
    /* old_len = s7_integer(s7_let_field_ref(s7, s7_make_symbol(s7, "print-length"))); */
    /* if (len > old_len) len = old_len; */
    /* buf = (char *)calloc((len + 1) * 64, sizeof(char)); */
    /* if (!buf) s7_error(s7, s7_make_symbol(s7, "out-of-memory"), s7_list(s7, 1, s7_make_string(s7, "unable to allocate string to display cstruct"))); */
    /* loc = snprintf(buf, (len + 1) * 64, "(cstruct"); */
    /* for (i = 0; i < len; i++) */
    /*     { */
    /*         flt = (char *)(buf + loc); */
    /*         if (is_NaN(b->data[i])) */
    /*             bytes = snprintf(flt, 64, " +nan.0"); */
    /*         else */
    /*             { */
    /*                 if (is_inf(b->data[i])) */
    /*                     bytes = snprintf(flt, 64, " %cinf.0", (b->data[i] >= 0.0) ? '+' : '-'); */
    /*                 else bytes = snprintf(flt, 64, " %.*f", (int)prec, b->data[i]); */
    /*             } */
    /*         loc += (bytes > 64) ? 64 : bytes; */
    /*     } */
    /* if (b->size > old_len) {buf[loc++] = ' '; buf[loc++] = '.'; buf[loc++] = '.'; buf[loc++] = '.';} */
    /* buf[loc] = ')'; */
    /* buf[loc + 1] = 0; */
    /* return(buf); */
}

/** g_cstruct_display_readable

    produces a "roundtrippable" string, one that when read by the reader
    results in an object equal to the original.
 */
static char *g_cstruct_display_readably(s7_scheme *s7, void *value)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_display_readably");
#endif

    char *buf, *flt;
    s7_int i, len, loc, bytes;
    char *msg = "FIXME g_cstruct_display readably!";

    buf = (char *)calloc(1, strlen(msg));
    strncpy(buf, msg, strlen(msg));

    struct cstruct_s *b = (struct cstruct_s *)value;
    return buf;

    /* len = b->size; */
    /* buf = (char *)calloc((len + 1) * 64, sizeof(char)); */
    /* loc = snprintf(buf, (len + 1) * 64, "(cstruct"); */
    /* for (i = 0; i < len; i++) */
    /*     { */
    /*         flt = (char *)(buf + loc); */
    /*         if (is_NaN(b->data[i])) */
    /*             bytes = snprintf(flt, 64, " +nan.0"); */
    /*         else */
    /*             { */
    /*                 if (is_inf(b->data[i])) */
    /*                     bytes = snprintf(flt, 64, " %cinf.0", (b->data[i] >= 0.0) ? '+' : '-'); */
    /*                 else bytes = snprintf(flt, 64, " %.16g", b->data[i]); */
    /*             } */
    /*         loc += (bytes > 64) ? 64 : bytes; */
    /*     } */
    /* buf[loc] = ')'; */
    /* buf[loc + 1] = 0; */
    /* return(buf); */
}
/* /section: display */

/* **************************************************************** */
/* section: serialization */
static s7_pointer g_cstruct_to_string(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_to_string");
    /* debug_print_s7(s7, "to_string cdr: ", s7_cdr(args)); */
#endif

    s7_pointer obj, choice;
    char *descr;
    obj = s7_car(args);
    if (s7_is_pair(s7_cdr(args)))
        choice = s7_cadr(args);
    else choice = s7_t(s7);
    if (choice == s7_make_keyword(s7, "readable"))
        descr = g_cstruct_display_readably(s7, s7_c_object_value(obj));
    else descr = g_cstruct_display(s7, s7_c_object_value(obj));
    /* printf("g_cstruct_display => %s", descr); */
    obj = s7_make_string(s7, descr);
    free(descr);
    return(obj);
}
/* /section: serialization */

/* **************************************************************** */
/* section: c-object construction */

static s7_pointer g_cstruct_copy(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_copy");
#endif

    s7_pointer new_g, obj;
    struct cstruct_s *g, *g1;
    size_t len;
    s7_int start = 0;
    obj = s7_car(args);
    return obj;//FIXME

    /* return(new_g); */
}

/*
  g_cstruct_init_from_s7
  initialize a C struct from s7-scheme arg-list (compare
  'cstruct_init', initialize a cstruct from C args)
 */
static s7_pointer g_cstruct_init_from_s7(s7_scheme *s7, struct cstruct_s *cs, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_init_from_s7");
    debug_print_s7(s7, "INIT ARGS: ", args);
#endif

    //FIXME: is this the right way to cdr over a list?
    s7_pointer arg = s7_car(args);
    s7_pointer cdr  = s7_cdr(args);
    int i = 0;
    while ( !s7_is_unspecified(s7, arg) ) {
        /* s7_display(s7, s7_make_integer(s7, i), p); */
        /* s7_display(s7, s7_make_string(s7, "arg: "), p); */
        /* s7_display(s7, arg, p); */
        /* s7_newline(s7, p); */

        switch(i) {
        case 0:
            if (!s7_is_character(arg)) {
                return(s7_wrong_type_arg_error(s7, "make-cstruct",
                                               1, arg, "a char"));
            }
            cs->c = s7_character(arg);
            break;
        case 1:
            cs->s = s7_string(arg);
        case 2:                 /* int */
            cs->i = s7_integer(arg);
            break;
        case 3:                 /* int* */
            cs->pi = (int*)calloc(1, sizeof(int));
            *cs->pi = s7_integer(arg);
            break;
        case 4:                 /* long */
            /* fixme: what is long in Scheme? */
            cs->l = s7_integer(arg);
            break;
        case 5:                 /* long* */
            cs->pl = (int*)calloc(1, sizeof(double));
            *cs->pl = s7_integer(arg);
            break;
        case 6:                 /* float */
            cs->f = s7_real(arg);
            break;
        case 7:                 /* float* */
            cs->pf = (int*)calloc(1, sizeof(float));
            *cs->pf = s7_real(arg);
            break;
        case 8:                 /* double */
            cs->d = s7_real(arg);
            break;
        case 9:                 /* double* */
            cs->pd = (int*)calloc(1, sizeof(double));
            *cs->pd = s7_real(arg);
            break;
        }
        arg = s7_car(cdr);
        cdr  = s7_cdr(cdr);
        i++;
    }
    return NULL;
}

/** g_new_cstruct
 */
/* docstring passed to the s7_define_.. used to register the fn in Scheme */
#define g_new_cstruct_help "(make-cstruct) returns a new cstruct with randome data"
#define MAKE_CSTRUCT_FORMAL_PARAMS "(c \#\\a) (s \"hello\") (i 1) (pi 2) (l 3) (pl 4) (f 5.0) (pf 6.0) (d 7) (pd 8)"
static s7_pointer g_new_cstruct(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_new_cstruct");
#endif

    struct cstruct_s *new_cstruct = (struct cstruct_s *)
        calloc(1, sizeof(struct cstruct_s));
    if (g_cstruct_init_from_s7(s7, new_cstruct, args) != NULL) {
        log_debug("OOPS");
    }

    s7_pointer new_cstruct_s7 = s7_make_c_object(s7, cstruct_t,
                                                 (void *)new_cstruct);

    s7_c_object_set_let(s7, new_cstruct_s7, g_cstruct_methods);
    s7_openlet(s7, new_cstruct_s7);

    return(new_cstruct_s7);
}
/* /section: c-object construction */

/* **************************************************************** */
/* section: c-object destruction */
static s7_pointer g_destroy_cstruct(s7_pointer obj)
{
#ifdef DEBUG_TRACE
    log_debug("g_destroy_cstruct");
#endif
    struct cstruct_s *cs = (struct cstruct_s*)s7_c_object_value(obj);
    cstruct_free(cs);
}
/* /section: c-object destruction */

/* **************************************************************** */
/* section: methods */
#define G_CSTRUCT_METHOD_HELP "(cstruct-method m) returns the method m."
#define G_CSTRUCT_METHOD_SIG s7_make_signature(s7, 3, s7_t(s7), s7_make_symbol(s7, "cstruct?"), s7_make_symbol(s7, "integer?"))
static s7_pointer g_cstruct_method(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_method");
    debug_print_s7(s7, "cstruct_method args: ", args);
#endif

    struct cstruct_s *g;
    size_t index;
    s7_int typ;
    s7_pointer obj;
    if (s7_list_length(s7, args) != 1)
        return(s7_wrong_number_of_args_error(s7, "cstruct-method takes 1 argument: ~~S", args));

    obj = s7_car(args);
    typ = s7_c_object_type(obj);
    /* if (typ != cstruct_t) */
    /*     return(s7_wrong_type_arg_error(s7, "cstruct-method", 1, obj, "a cstruct")); */
    g  = (struct cstruct_s *)s7_c_object_value(obj);

    /* if (s7_is_null(s7, s7_cdr(args))) /\* this is for an (obj) test *\/ */
    /*     return(s7_wrong_type_arg_error(s7, "cstruct-ref", 1, obj, "missing symbol arg")); */
        /* return(s7_make_integer(s7, 32)); */

    /* symbol arg = name of method, find it in object's method table */
    /* s7_pointer arg = s7_cadr(args); */
    if (s7_is_symbol(obj)) { /* ((cstruct 'empty) b) etc */
        /* in this case, arg is a method sym, which we lookup in
           the method table of the object */
        s7_pointer val;
        /* g_cstruct_methods as env in which to lookup arg */
        val = s7_symbol_local_value(s7, obj, g_cstruct_methods);
        if (val != obj)
            return(val);
        else
            ; /* found self-referring method? method not found? */
    } else
        return(s7_wrong_type_arg_error(s7, "cstruct-method",
                                       2, obj, "a symbol"));
}
/* section: methods */

/* section: extension methods */
void register_extension_methods(s7_scheme *s7, s7_int cstruct_t)
{
    s7_c_type_set_gc_free(s7, cstruct_t, g_destroy_cstruct);
    /* s7_c_type_set_equal(s7, cstruct_t, g_cstructs_are_eql); */
    /* s7_c_type_set_is_equal(s7, cstruct_t, g_cstructs_are_equal); */
    /* s7_c_type_set_is_equivalent(s7, cstruct_t, g_cstructs_are_equivalent); */
    /* s7_c_type_set_gc_mark(s7, cstruct_t, g_cstruct_gc_mark); */
    s7_c_type_set_ref(s7, cstruct_t, g_cstruct_get);
    /* s7_c_type_set_set(s7, cstruct_t, g_cstruct_set); */
    /* s7_c_type_set_length(s7, cstruct_t, g_cstruct_length); */
    /* s7_c_type_set_copy(s7, cstruct_t, g_cstruct_copy); */
    /* s7_c_type_set_reverse(s7, cstruct_t, g_cstruct_reverse); */
    /* s7_c_type_set_fill(s7, cstruct_t, g_cstruct_fill); */
    s7_c_type_set_to_string(s7, cstruct_t, g_cstruct_to_string);
}

/* /section: extension methods */


/* **************************************************************** */
/* section: c-type configuration */

static int _register_cstruct_fns(s7_scheme *s7)
{
#ifdef DEBUG_TRACE
    log_debug("_register_cstruct_fns");
#endif
    /* s7_define_safe_function(s7, "cstruct", g_to_cstruct, 0, 0, true, g_cstruct_help); */
    s7_define_safe_function_star(s7, "make-cstruct", g_new_cstruct,
                                 MAKE_CSTRUCT_FORMAL_PARAMS,
                                 g_new_cstruct_help);

    s7_define_typed_function(s7, "cstruct-ref", g_cstruct_get, 2, 0, false, G_CSTRUCT_GET_HELP, G_CSTRUCT_GET_SIG);

    s7_define_typed_function(s7, "cstruct-method", g_cstruct_method, 1, 0, false, G_CSTRUCT_METHOD_HELP, G_CSTRUCT_METHOD_SIG);

    /* s7_c_type_set_getter(s7, g_cstruct_type, s7_name_to_value(s7, "cstruct-ref")); */

    s7_define_typed_function(s7, "cstruct-set!", g_cstruct_set, 3, 0, false, g_cstruct_set_help, g_cstruct_set_sig);
    /* s7_c_type_set_setter(s7, g_cstruct_type, s7_name_to_value(s7, "cstruct-set!")); */

    // cstruct-let => s7_c_object_let, a let for the instance not the type
    /* s7_define_safe_function(s7, "cstruct-let", g_cstruct_let, 1, 0, false, g_cstruct_let_help); */

    /* s7_define_safe_function(s7, "subcstruct", g_subcstruct, 1, 0, true, g_subcstruct_help); */
    /* s7_define_safe_function(s7, "cstruct-append", g_cstruct_append, 0, 0, true, g_cstruct_append_help); */
    /* s7_define_safe_function(s7, "cstruct-reverse!", g_cstruct_reverse_in_place, 1, 0, false, g_cstruct_reverse_in_place_help); */
    s7_define_typed_function(s7, "cstruct?", g_is_cstruct, 1, 0, false, g_is_cstruct_help, g_is_cstruct_sig);
    /* s7_define_safe_function_star(s7, "cstructs1", g_cstructs, "(frequency 4)", "test for function*"); */
    /* s7_define_safe_function_star(s7, "cstructs", g_cstructs, "(frequency 4) (scaler 1)", "test for function*"); */
    /* s7_define_safe_function_star(s7, "cstructs3", g_cstructs, "(frequency 4) (scaler 1) (asdf 32)", "test for function*"); */
    /* s7_define_safe_function_star(s7, "cstructs4", g_cstructs, "(frequency 4) (scaler 1) (asdf 32) etc", "test for function*"); */
    /* s7_define_function_star(s7, "unsafe-cstructs1", g_cstructs, "(frequency 4)", "test for function*"); */
    /* s7_define_function_star(s7, "unsafe-cstructs", g_cstructs, "(frequency 4) (scaler 1)", "test for function*"); */
    /* s7_define_function_star(s7, "unsafe-cstructs3", g_cstructs, "(frequency 4) (scaler 1) (asdf 32)", "test for function*"); */
    /* s7_define_function_star(s7, "unsafe-cstructs4", g_cstructs, "(frequency 4) (scaler 1) (asdf 32) etc", "test for function*"); */
    /* s7_define_safe_function_star(s7, "cstructs5", g_cstructs, "(frequency 4) :allow-other-keys", "test for function*"); */
    g_cstruct_methods = s7_eval_c_string(s7, "(openlet (immutable! (inlet 'float-vector? (lambda (p) (display \"foo \") #t) \
								       'signature (lambda (p) (list '#t 'cstruct? 'integer?)) \
								       'type cstruct? \
								       'foo (lambda (p) 999) \
								       'arity (lambda (p) (cons 1 1)) \
								       'aritable? (lambda (p args) (= args 1)) \
								       'vector-dimensions (lambda (p) (list (length p))) \
						                       'empty (lambda (p) (zero? (length p))) \
								       'vector-ref cstruct-ref \
								       'vector-set! cstruct-set!)))"
                                         );
						                       /* 'reverse! cstruct-reverse!)))" */
                                                                       /* 'subsequence subcstruct \ */
						                       /* 'append cstruct-append \ */

    s7_gc_protect(s7, g_cstruct_methods);
}

static int _make_c_type(s7_scheme *s7)
{
#ifdef DEBUG_TRACE
    log_debug("_make_c_type");
#endif
    cstruct_t = s7_make_c_type(s7, "<cstruct>");
    register_extension_methods(s7, cstruct_t);
    return 0;
}

s7_int configure_s7_cstruct_type(s7_scheme *s7)
{
#ifdef DEBUG_TRACE
    log_debug("configure_s7_cstruct_type");
#endif
    s7_int t = _make_c_type(s7);
    int rc   = _register_cstruct_fns(s7);
    return t;
}
/* section: c-type configuration */

/* **************************************************************** */
/* section: gc */
static s7_pointer g_cstruct_gc_mark(s7_scheme *s7, s7_pointer p)
{
    /* nothing to mark because we protect g_cstruct_methods below, and all cstructs get the same let */
    return(p);
}

/* static s7_pointer g_cstruct_gc_free(s7_scheme *s7, s7_pointer obj) */
/* { */
/* #ifdef DEBUG_TRACE */
/*     log_debug("g_cstruct_gc_free"); */
/* #endif */
/*     struct cstruct_s *cs = (struct cstruct_s*)s7_c_object_value(obj); */
/*     cstruct_free(cs); */
/*     return(NULL); */
/* } */

/* /section: gc */

/* **************************************************************** */
/* section: debugging */
void debug_print_s7(s7_scheme *s7, char *label, s7_pointer obj)
{
    log_debug("debug_print_s7: ");
    s7_pointer p = s7_current_output_port(s7);
    s7_display(s7, s7_make_string(s7, label), p);
    s7_display(s7, obj, p);
    s7_newline(s7, p);
}

/* /section: debugging */
