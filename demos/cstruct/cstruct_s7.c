#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "s7.h"

#include "cstruct.h"

static s7_int cstruct_t = 0;
static s7_pointer g_cstruct_methods;

/*
  sections: (search for 'section:')

  * identity
  * equality
  * getters and setters
  * display
  * serialization
  * c-object construction
  * c-object destruction
  * gc
  * c-type configuration
  * misc
 */

/* **************************************************************** */
/* section: identity */
#define g_is_cstruct_help "(cstruct? obj) returns #t if obj is a cstruct."
#define g_is_cstruct_sig s7_make_signature(sc, 2, s7_make_symbol(sc, "boolean?"), s7_t(sc))
static s7_pointer g_is_cstruct(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_is_cstruct\n");
#endif
    return(s7_make_boolean(sc, s7_c_object_type(s7_car(args)) == cstruct_t));
}
/* /section: identity */

/* **************************************************************** */
/* section: equality */
static bool g_cstructs_are_eql(void *val1, void *val2)
{
#ifdef DEBUG_TRACE
    printf("g_cstructs_are_eql\n");
#endif
    s7_int i, len;
    struct cstruct_s *b1 = (struct cstruct_s *)val1;
    struct cstruct_s *b2 = (struct cstruct_s *)val2;
    if (val1 == val2) return(true);
    //FIXME
    return(true);
}

static s7_pointer g_cstructs_are_equal(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_cstructs_are_equal\n");
#endif
    return(s7_make_boolean(sc, g_cstructs_are_eql((void *)s7_c_object_value(s7_car(args)), (void *)s7_c_object_value(s7_cadr(args)))));
}

static s7_pointer g_cstructs_are_equivalent(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_cstructs_are_equivalent\n");
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
        return(s7_f(sc));
    if (arg1 == arg2)
        return(s7_make_boolean(sc, true));
    if (s7_is_let(arg1))             /* (cstruct-let (cstruct)) */
        return(s7_make_boolean(sc, false));    /* checked == above */
    g1 = (struct cstruct_s *)s7_c_object_value(arg1);
    if (s7_c_object_type(arg2) != cstruct_t)
        return(s7_make_boolean(sc, false));
    g2 = (struct cstruct_s *)s7_c_object_value(arg2);
    /* len = g1->size; */
    /* if (len != g2->size) */
    /*     return(s7_make_boolean(sc, false)); */
    /* v1 = s7_make_float_vector_wrapper(sc, len, g1->data, 1, NULL, false); */
    /* gc1 = s7_gc_protect(sc, v1); */
    /* v2 = s7_make_float_vector_wrapper(sc, len, g2->data, 1, NULL, false); */
    /* gc2 = s7_gc_protect(sc, v2); */
    result = s7_is_equivalent(sc, v1, v2);
    s7_gc_unprotect_at(sc, gc1);
    s7_gc_unprotect_at(sc, gc2);
    return(s7_make_boolean(sc, result));
}
/* /section: equality */

/* **************************************************************** */
/* section: getters and setters */
static s7_pointer g_cstruct_ref(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_cstruct_ref\n");
#endif
#define g_cstruct_ref_help "(cstruct-ref b i) returns the cstruct value at index i."
#define g_cstruct_ref_sig s7_make_signature(sc, 3, s7_t(sc), s7_make_symbol(sc, "cstruct?"), s7_make_symbol(sc, "integer?"))
    struct cstruct_s *g;
    size_t index;
    s7_int typ;
    s7_pointer ind, obj;
    if (s7_list_length(sc, args) != 2)
        return(s7_wrong_number_of_args_error(sc, "cstruct-ref takes 2 arguments: ~~S", args));
    obj = s7_car(args);
    typ = s7_c_object_type(obj);
    if (typ != cstruct_t)
        return(s7_wrong_type_arg_error(sc, "cstruct-ref", 1, obj, "a cstruct"));
    g  = (struct cstruct_s *)s7_c_object_value(obj);
    if (s7_is_null(sc, s7_cdr(args))) /* this is for an (obj) test */
        return(s7_make_integer(sc, 32));
    /* ind = s7_cadr(args); */
    /* if (s7_is_integer(ind)) */
    /*     index = (size_t)s7_integer(ind); */
    /* else  */
    /*     { */
    /*         if (s7_is_symbol(ind)) /\* ((cstruct 'empty) b) etc *\/ */
    /*             { */
    /*                 s7_pointer val; */
    /*                 val = s7_symbol_local_value(sc, ind, g_cstruct_methods); */
    /*                 if (val != ind) */
    /*                     return(val); */
    /*             } */
    /*         return(s7_wrong_type_arg_error(sc, "cstruct-ref", 2, ind, "an integer")); */
    /*     } */
    /* if (index < g->size) */
    /*     return(s7_make_real(sc, g->data[index])); */
    /* return(s7_out_of_range_error(sc, "(implicit) cstruct-ref", 2, ind, "should be less than cstruct length")); */
}

static s7_pointer g_cstruct_set(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_cstruct_set\n");
#endif
#define g_cstruct_set_help "(cstruct-set! b i x) sets the cstruct value at index i to x."
#define g_cstruct_set_sig s7_make_signature(sc, 4, s7_make_symbol(sc, "float?"), s7_make_symbol(sc, "cstruct?"), s7_make_symbol(sc, "integer?"), s7_make_symbol(sc, "float?"))
    struct cstruct_s *g;
    s7_int index, typ;
    s7_pointer obj;
    return(s7_caddr(args)); //FIXME

    /* /\* c_object_set functions need to check that they have been passed the correct number of arguments *\/ */
    /* if (s7_list_length(sc, args) != 3) */
    /*     return(s7_wrong_number_of_args_error(sc, "cstruct-set! takes 3 arguments: ~~S", args)); */
    /* obj = s7_car(args); */
    /* typ = s7_c_object_type(obj); */
    /* if (typ != cstruct_t) */
    /*     return(s7_wrong_type_arg_error(sc, "cstruct-set!", 1, obj, "a cstruct")); */
    /* if (s7_is_immutable(obj)) */
    /*     return(s7_wrong_type_arg_error(sc, "cstruct-set!", 1, obj, "a mutable cstruct")); */
    /* if (!s7_is_integer(s7_cadr(args))) */
    /*     return(s7_wrong_type_arg_error(sc, "cstruct-set!", 2, s7_cadr(args), "an integer")); */
    /* g = (struct cstruct_s *)s7_c_object_value(obj); */
    /* index = s7_integer(s7_cadr(args)); */
    /* if ((index >= 0) && (index < g->size)) */
    /*     { */
    /*         g->data[index] = s7_number_to_real(sc, s7_caddr(args)); */
    /*         return(s7_caddr(args)); */
    /*     } */
    /* return(s7_out_of_range_error(sc, "cstruct-set", 2, s7_cadr(args), "should be less than cstruct length")); */
}
/* /section: getters and setters */

/* **************************************************************** */
/* section: display */
static char *g_cstruct_display(s7_scheme *sc, void *value)
{
#ifdef DEBUG_TRACE
    printf("g_cstruct_display\n");
#endif

    struct cstruct_s *b = (struct cstruct_s *)value;
    s7_int i, len, prec, old_len, loc, bytes;
    char *buf, *flt;
    return "FIXME g_cstruct_display!";
    /* prec = s7_integer(s7_let_field_ref(sc, s7_make_symbol(sc, "float-format-precision"))); */
    /* if (prec >= 16) prec = 3; */
    /* len = b->size; */
    /* old_len = s7_integer(s7_let_field_ref(sc, s7_make_symbol(sc, "print-length"))); */
    /* if (len > old_len) len = old_len; */
    /* buf = (char *)calloc((len + 1) * 64, sizeof(char)); */
    /* if (!buf) s7_error(sc, s7_make_symbol(sc, "out-of-memory"), s7_list(sc, 1, s7_make_string(sc, "unable to allocate string to display cstruct"))); */
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

static char *g_cstruct_display_readably(s7_scheme *sc, void *value)
{
#ifdef DEBUG_TRACE
    printf("g_cstruct_display_readably\n");
#endif

    char *buf, *flt;
    s7_int i, len, loc, bytes;
    struct cstruct_s *b = (struct cstruct_s *)value;
    return "FIXME: g_cstruct_display_readably";
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
static s7_pointer g_cstruct_to_string(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_cstruct_to_string\n");
#endif

    s7_pointer obj, choice;
    char *descr;
    obj = s7_car(args);
    if (s7_is_pair(s7_cdr(args)))
        choice = s7_cadr(args);
    else choice = s7_t(sc);
    if (choice == s7_make_keyword(sc, "readable"))
        descr = g_cstruct_display_readably(sc, s7_c_object_value(obj));
    else descr = g_cstruct_display(sc, s7_c_object_value(obj));
    obj = s7_make_string(sc, descr);
    free(descr);
    return(obj);
}
/* /section: serialization */

/* **************************************************************** */
/* section: c-object construction */

static s7_pointer g_cstruct_copy(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_cstruct_copy\n");
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
    printf("g_cstruct_init_from_s7\n");
#endif
    s7_pointer arg1;
    arg1 = s7_car(args);
    if (!s7_is_character(arg1)) {
        return(s7_wrong_type_arg_error(s7, "make-cstruct",
                                       1, arg1, "a char"));
    }
    cs->c = s7_character(arg1);
    cs->s = strdup("howdy");
    return NULL;
}

/* docstring passed to the s7_define_.. used to register the fn in Scheme */
#define g_new_cstruct_help "(make-cstruct) returns a new cstruct with randome data"
static s7_pointer g_new_cstruct(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    printf("g_new_cstruct\n");
#endif

    struct cstruct_s *new_cstruct = (struct cstruct_s *)
        calloc(1, sizeof(struct cstruct_s));
    if (g_cstruct_init_from_s7(s7, new_cstruct, args) != NULL) {
        printf("OOPS\n");
    }
    printf("new cs->s: %s\n", new_cstruct->s);

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
    printf("g_destroy_cstruct\n");
#endif
    struct cstruct_s *cs = (struct cstruct_s*)s7_c_object_value(obj);
    cstruct_free(cs);
}
/* /section: c-object destruction */

/* **************************************************************** */
/* section: gc */
/* static s7_pointer g_cstruct_gc_free(s7_scheme *sc, s7_pointer obj) */
/* { */
/* #ifdef DEBUG_TRACE */
/*     printf("g_cstruct_gc_free\n"); */
/* #endif */
/*     struct cstruct_s *cs = (struct cstruct_s*)s7_c_object_value(obj); */
/*     cstruct_free(cs); */
/*     return(NULL); */
/* } */

static s7_pointer g_cstruct_gc_mark(s7_scheme *sc, s7_pointer p)
{
    /* nothing to mark because we protect g_cstruct_methods below, and all cstructs get the same let */
    return(p);
}
/* /section: gc */

/* **************************************************************** */
/* section: c-type configuration */

int _register_cstruct_fns(s7_scheme *sc)
{
#ifdef DEBUG_TRACE
    printf("_register_cstruct_fns\n");
#endif
    s7_define_safe_function(sc, "make-cstruct", g_new_cstruct, 1, 0, false, g_new_cstruct_help);
    /* s7_define_safe_function(sc, "cstruct", g_to_cstruct, 0, 0, true, g_cstruct_help); */
    s7_define_typed_function(sc, "cstruct-ref", g_cstruct_ref, 2, 0, false, g_cstruct_ref_help, g_cstruct_ref_sig);
    /* s7_c_type_set_getter(sc, g_cstruct_type, s7_name_to_value(sc, "cstruct-ref")); */
    s7_define_typed_function(sc, "cstruct-set!", g_cstruct_set, 3, 0, false, g_cstruct_set_help, g_cstruct_set_sig);
    /* s7_c_type_set_setter(sc, g_cstruct_type, s7_name_to_value(sc, "cstruct-set!")); */

    // cstruct-let => s7_c_object_let, a let for the instance not the type
    /* s7_define_safe_function(sc, "cstruct-let", g_cstruct_let, 1, 0, false, g_cstruct_let_help); */

    /* s7_define_safe_function(sc, "subcstruct", g_subcstruct, 1, 0, true, g_subcstruct_help); */
    /* s7_define_safe_function(sc, "cstruct-append", g_cstruct_append, 0, 0, true, g_cstruct_append_help); */
    /* s7_define_safe_function(sc, "cstruct-reverse!", g_cstruct_reverse_in_place, 1, 0, false, g_cstruct_reverse_in_place_help); */
    s7_define_typed_function(sc, "cstruct?", g_is_cstruct, 1, 0, false, g_is_cstruct_help, g_is_cstruct_sig);
    /* s7_define_safe_function_star(sc, "cstructs1", g_cstructs, "(frequency 4)", "test for function*"); */
    /* s7_define_safe_function_star(sc, "cstructs", g_cstructs, "(frequency 4) (scaler 1)", "test for function*"); */
    /* s7_define_safe_function_star(sc, "cstructs3", g_cstructs, "(frequency 4) (scaler 1) (asdf 32)", "test for function*"); */
    /* s7_define_safe_function_star(sc, "cstructs4", g_cstructs, "(frequency 4) (scaler 1) (asdf 32) etc", "test for function*"); */
    /* s7_define_function_star(sc, "unsafe-cstructs1", g_cstructs, "(frequency 4)", "test for function*"); */
    /* s7_define_function_star(sc, "unsafe-cstructs", g_cstructs, "(frequency 4) (scaler 1)", "test for function*"); */
    /* s7_define_function_star(sc, "unsafe-cstructs3", g_cstructs, "(frequency 4) (scaler 1) (asdf 32)", "test for function*"); */
    /* s7_define_function_star(sc, "unsafe-cstructs4", g_cstructs, "(frequency 4) (scaler 1) (asdf 32) etc", "test for function*"); */
    /* s7_define_safe_function_star(sc, "cstructs5", g_cstructs, "(frequency 4) :allow-other-keys", "test for function*"); */
    g_cstruct_methods = s7_eval_c_string(sc, "(openlet (immutable! (inlet 'float-vector? (lambda (p) #t) \
								       'signature (lambda (p) (list '#t 'cstruct? 'integer?)) \
								       'type cstruct? \
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

    s7_gc_protect(sc, g_cstruct_methods);
}

int _make_c_type(s7_scheme *sc)
{
#ifdef DEBUG_TRACE
    printf("_make_c_type\n");
#endif
    cstruct_t = s7_make_c_type(sc, "<cstruct>");
    s7_c_type_set_gc_free(sc, cstruct_t, g_destroy_cstruct);
    /* s7_c_type_set_equal(sc, cstruct_t, g_cstructs_are_eql); */
    /* s7_c_type_set_is_equal(sc, cstruct_t, g_cstructs_are_equal); */
    /* s7_c_type_set_is_equivalent(sc, cstruct_t, g_cstructs_are_equivalent); */
    /* s7_c_type_set_gc_mark(sc, cstruct_t, g_cstruct_gc_mark); */
    /* s7_c_type_set_ref(sc, cstruct_t, g_cstruct_ref); */
    /* s7_c_type_set_set(sc, cstruct_t, g_cstruct_set); */
    /* s7_c_type_set_length(sc, cstruct_t, g_cstruct_length); */
    /* s7_c_type_set_copy(sc, cstruct_t, g_cstruct_copy); */
    /* s7_c_type_set_reverse(sc, cstruct_t, g_cstruct_reverse); */
    /* s7_c_type_set_fill(sc, cstruct_t, g_cstruct_fill); */
    /* s7_c_type_set_to_string(sc, cstruct_t, g_cstruct_to_string); */
    return 0;
}

s7_int configure_s7_cstruct_type(s7_scheme *s7)
{
#ifdef DEBUG_TRACE
    printf("configure_s7_cstruct_type\n");
#endif
    s7_int t = _make_c_type(s7);
    int rc   = _register_cstruct_fns(s7);
    return t;
}
/* section: c-type configuration */

/* **************************************************************** */
/* section: misc */
/* our cstruct does not behave line a list, we don't need these: */

/* static s7_pointer g_cstruct_length(s7_scheme *sc, s7_pointer args) */
/* { */
/*     struct cstruct_s *g = (struct cstruct_s *)s7_c_object_value(s7_car(args)); */
/*     return(s7_make_integer(sc, 1)); */
/* } */

/* static s7_pointer g_cstruct_reverse(s7_scheme *sc, s7_pointer args) */
/* { */
/*     size_t i, j; */
/*     struct cstruct_s *g, *g1; */
/*     s7_pointer new_g; */
/*     if (!s7_is_null(sc, s7_cdr(args))) */
/*         return(s7_wrong_number_of_args_error(sc, "(cstruct-)reverse", args)); */
/*     g = (struct cstruct_s *)s7_c_object_value(s7_car(args)); */
/*     /\* new_g = g_new_cstruct(sc, s7_cons(sc, s7_make_integer(sc, g->size), s7_nil(sc))); *\/ */
/*     /\* g1 = (struct cstruct_s *)s7_c_object_value(new_g); *\/ */
/*     /\* for (i = 0, j = g->size - 1; i < g->size; i++, j--) *\/ */
/*     /\*     g1->data[i] = g->data[j]; *\/ */
/*     /\* return(new_g); *\/ */
/*     return s7_car(args); */
/* } */

/* /section: misc */

