#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "s7.h"

#include "cstruct.h"

static s7_int cstruct_t = 0;
static s7_pointer g_cstruct_methods_let;

/* forward decls */
/* section: identity */
static s7_pointer g_is_cstruct(s7_scheme *s7, s7_pointer args);

/* section: equality */
static bool _cstructs_are_value_equal(struct cstruct_s *val1,
                                      struct cstruct_s *val2);
static s7_pointer g_cstructs_are_equal(s7_scheme *s7, s7_pointer args);
static s7_pointer g_cstructs_are_equivalent(s7_scheme *s7, s7_pointer args);

/* section: getters and setters */
static s7_pointer g_cstruct_ref_specialized(s7_scheme *s7, s7_pointer args);
static s7_pointer g_cstruct_set_specialized(s7_scheme *s7, s7_pointer args);

static s7_pointer g_cstruct_object_applicator(s7_scheme *s7, s7_pointer args);

static void _register_get_and_set(s7_scheme *s7);

/* section: display */
static char *g_cstruct_display(s7_scheme *s7, void *value);
static char *g_cstruct_display_readably(s7_scheme *s7, void *value);

/* section: serialization */
static s7_pointer g_cstruct_to_string(s7_scheme *s7, s7_pointer args);

/* section: c-object construction */
static s7_pointer g_cstruct_copy(s7_scheme *s7, s7_pointer args);
static s7_pointer g_cstruct_init_from_s7(s7_scheme *s7, struct cstruct_s *cs, s7_pointer args);
static s7_pointer g_new_cstruct(s7_scheme *s7, s7_pointer args);
enum formals_e {
    FLD_C = 0, FLD_BYTE,
    FLD_STR,
    FLD_B, FLD_PB,
    FLD_SH_RT, FLD_PSH_RT,
    FLD_I, FLD_PI,
    FLD_L, FLD_PL, FLD_LL, FLD_PLL,
    FLD_F, FLD_PF, FLD_D, FLD_PD
};

/* section: c-object destruction */
static s7_pointer g_destroy_cstruct(s7_scheme *s7, s7_pointer obj);

/* section: extension methods */
static void _register_c_type_methods(s7_scheme *s7, s7_int cstruct_t);
static void _register_c_object_methods(s7_scheme *s7, s7_pointer cstruct);

/* section: c-type configuration */
static void   _register_cstruct_fns(s7_scheme *s7);

/* this is the public API that clients call: */
int configure_s7_cstruct_type(s7_scheme *s7); /* public */

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
    return(s7_make_boolean(s7,
                           s7_c_object_type(s7_car(args)) == cstruct_t));
}
/* /section: identity */

/* **************************************************************** */
/* section: equality

   eqv? "returns #t if obj1 and obj2 are normally regarded as the same
   object." (r7rs)

   eq? "On symbols, booleans, the empty list, pairs, and records, and
also on non-empty strings, vectors, and bytevectors, eq? and eqv? are
guaranteed to have the same behavior. On procedures, eq? must return
true if the arguments’ location tags are equal. On numbers and
characters, eq?’s behavior is implementation-dependent, but it will
always return either true or false. On empty strings, empty vectors,
and empty bytevectors, eq? may also behave differently from eqv?."
(r7rs)

   equal? "The equal? procedure, when applied to pairs, vectors,
strings and bytevectors, recursively compares them, returning #t when
the unfoldings of its arguments into (possibly infinite) trees are
equal (in the sense of equal?) as ordered trees, and #f otherwise. It
returns the same as eqv? when applied to booleans, symbols, numbers,
characters, ports, procedures, and the empty list. If two objects are
eqv?, they must be equal? as well. In all other cases, equal? may
return either #t or #f." (r7rs)

   equivalent? s7 only? same as Scheme 'equal?' ?

 */

/** _cstructs_are_value_equal

    true if (possibly distinct) cstructs are value-equal
 */
static bool _cstructs_are_value_equal(struct cstruct_s *val1,
                                      struct cstruct_s *val2)
{
#ifdef DEBUG_TRACE
    log_debug("_cstructs_are_value_equal");
#endif
    if (val1 == val2) return true;

    log_debug("v1>substruct %p, v2->substruct: %p",
              val1->substruct, val2->substruct);

    if (val1->substruct == NULL) {
        if (val2->substruct != NULL) {
            return false;
        /* } else { */
        }
    } else {
        if (val2->substruct == NULL)
            return false;
    }
    if ( ! _cstructs_are_value_equal(val1->substruct, val2->substruct) )
        return false;

    if (val1->c != val2->c) return false;

    if (val1->bits != val2->bits) return false;

    if (val1->str != val2->str) {
        if (strncmp(val1->str, val2->str, strlen(val1->str)) != 0)
            return false;
    }

    if (val1->bits != val2->bits) return false;

    /* ASSUMPTION: all ptrs initialized */
    if (val1->b != val2->b) return false;
    if (*val1->pb != *val2->pb) return false;

    if (val1->sh_rt != val2->sh_rt) return false;
    if (*val1->psh_rt != *val2->psh_rt) return false;

    if (val1->i != val2->i) return false;
    if (*val1->pi != *val2->pi) return false;

    if (val1->l != val2->l) return false;
    if (*val1->pl != *val2->pl) return false;

    if (val1->ll != val2->ll) return false;
    if (*val1->pll != *val2->pll) return false;

    if (val1->f != val2->f) return false;
    if (*val1->pf != *val2->pf) return false;

    if (val1->d != val2->d) return false;
    if (*val1->pd != *val2->pd) return false;

    return(true);
}

/** _cstructs_are_c_eql

    c implementation of Scheme 'eqv?' ?

    numbers: both exact and numerically equal (=)
    pairs, vectors, bytevectors, records, strings: same mem address
 */

/** g_cstructs_are_equal

    callback for Scheme 'eqv?' ?
    wrapper on _cstructs_are_c_eql
 */
static s7_pointer g_cstructs_are_equal(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstructs_are_equal");
#endif
    return(s7_make_boolean(s7,
          /* _cstructs_are_c_eql((void *)s7_c_object_value(s7_car(args)), */
          /*                      (void *)s7_c_object_value(s7_cadr(args))) */
          _cstructs_are_value_equal((void *)s7_c_object_value(s7_car(args)),
                                    (void *)s7_c_object_value(s7_cadr(args)))
                           )
           );
}

/** g_cstructs_are_equivalent

    implementation for Scheme 'equal?' 'eq?' ?
    true if same values
 */
#define G_CSTRUCTS_ARE_EQUIVALENT_HELP "(equivalent? cstruct1 cstruct2)"
static s7_pointer g_cstructs_are_equivalent(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstructs_are_equivalent");
#endif

    s7_pointer arg1, arg2;

    arg1 = s7_car(args);
    arg2 = s7_cadr(args);
    if (!s7_is_c_object(arg2))
        return(s7_f(s7));

    if (s7_is_let(arg1))             /* (cstruct-let (cstruct)) */
        return(s7_make_boolean(s7, false));    /* checked == above */

    /* same type? type of arg1 known to be cstruct */
    if (s7_c_object_type(arg2) != cstruct_t)
        return(s7_make_boolean(s7, false));

    if (arg1 == arg2) { /* same c-object? */
        return(s7_t(s7));
    }

    /* compare c objects for equivalence */
    struct cstruct_s *g1, *g2;
    g1 = (struct cstruct_s *)s7_c_object_value(arg1);
    g2 = (struct cstruct_s *)s7_c_object_value(arg2);

    if (g1 == g2) /* same cstruct */
        return s7_t(s7);

    bool eq = _cstructs_are_value_equal(g1, g2);
    if (eq)
        return(s7_t(s7));
        /* return(s7_make_boolean(s7, true)); */
    else
        return(s7_f(s7));
        /* return(s7_make_boolean(s7, false)); */
}
/* /section: equality */

/* **************************************************************** */
/* section: getters and setters */
/* get and set are special, since there are two ways to do each:
   * 1. generic get and set! (c-type methods)
   * 2. specialized cstruct-get and cstruct-set! (Scheme procedures)
 */

/* **************** */
/* helper fn */
static s7_pointer _cstruct_lookup_kw(s7_scheme *s7,
                             struct cstruct_s *cstruct, s7_pointer kw)
{
#ifdef DEBUG_TRACE
    log_debug("cstruct_lookup_kw");
#endif
    if (kw == s7_make_keyword(s7, "c")) {
        return s7_make_character(s7, cstruct->c);
    }
    if (kw == s7_make_keyword(s7, "str"))
        return s7_make_string(s7, cstruct->str);
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

    return(s7_wrong_type_arg_error(s7, "cstruct-ref",
                                       1, kw, "one of :c, :str, :i, etc."));
}

/* **************** */
/** g_cstruct_ref_specialized

    (cstruct-ref obj key)
    takes two args, a cstruct object and a keyword to look up in the object.
 */
#define G_CSTRUCT_REF_SPECIALIZED_HELP "(cstruct-ref b i) returns the cstruct value at index i."
#define G_CSTRUCT_REF_SPECIALIZED_SIG s7_make_signature(s7, 3, s7_t(s7), s7_make_symbol(s7, "cstruct?"), s7_make_symbol(s7, "integer?"))

static s7_pointer g_cstruct_ref_specialized(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_ref_specialized");
    /* debug_print_s7(s7, "g_cstruct_ref_specialized args: ", args); */
#endif

    struct cstruct_s *g;
    /* size_t index; */
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
        return _cstruct_lookup_kw(s7, g, arg);
    else {
        return(s7_wrong_type_arg_error(s7, "cstruct-ref",
                                       2, arg, "a keyword"));
    }
}

/** g_cstruct_object_applicator

    (more accurate: object_applicator, not essentially tied to ref)

    registered by s7_c_type_set_ref

    first arg is "self"

    function called when objects of type cstruct are evaluated as
    functions, i.e. when they occur in function position (car of a
    list).

    not to be confused with generic ref of SRFI 123, e.g. (ref vec i),
    which s7 does not support.(?)

    by convention, same as ref_specialized (i.e cstruct-ref) but this
    is not a requirement. could be used for anything, not just
    reference. example: (o :child-count) == (cstruct-child-count o)
    or (o :fullname) concats (o :fname) and (o :lname)

    iow, it's a generic generic function, whereas generic ref is just
    a generic ref function. "meta generic?"

    in practice, its a dispatcher. sorta. its job is to inspect the args and
    decide what to do with them.
 */
static s7_pointer g_cstruct_object_applicator(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_object_applicator");
    debug_print_s7(s7, "APPLICATOR ARGS: ", s7_cdr(args));
#endif

    /* no need to check arg1, it's the "self" cstruct obj */
    /* s7_pointer g  = (struct cstruct_s *)s7_c_object_value(obj); */

    s7_pointer rest = s7_cdr(args);
    if (s7_is_null(s7, rest))
        return(s7_wrong_type_arg_error(s7, "cstruct-fn",
                                       1, rest, "missing keyword arg"));

    s7_pointer op = s7_car(rest);

    /* Currently s7 does not make a distinction between keywords and symbols; (keyword? :a) and (symbol? :a) both report true, and s7_is_ */
    if (s7_is_keyword(op)) {
        if (op == s7_make_keyword(s7, "sumints")) {
            /* demo arbitrary kw op, :sumints sums int values in obj */
            log_debug("running :sumints");
            s7_pointer obj = s7_car(args);
            struct cstruct_s *cs  = (struct cstruct_s *)s7_c_object_value(obj);
            return s7_make_integer(s7, cs->i
                                   + *cs->pi
                                   + cs->l
                                   + *cs->pl
                                   + cs->ll
                                   + *cs->pll
                                   );
        } else {
            return g_cstruct_ref_specialized(s7, args);
        }
    } else {
        if (s7_is_symbol(op)) { /* method access ((cstruct 'foo) b) etc */
            s7_pointer val;
            val = s7_symbol_local_value(s7, op, g_cstruct_methods_let);
            if (val != op)
                return(val);
            else {
                /* found self-referring method? method not found? methods table corrupt */
                /* return(s7_wrong_type_arg_error(s7, "cstruct-ref", */
                /*                                2, arg, "a kw or sym")); */
                log_error("ERROR: corrupt object-methods-let?");
                return NULL;
            }
	} else {
            return(s7_wrong_type_arg_error(s7, "cstruct-ref",
                                           2, op, "a keyword or symbol"));
        }
    }
}

/* **************** */
/** g_cstruct_set_specialized

    registered twice: as a c-type generalize set! (s7_c_type_set_set()) and
    as procedure "cstruct-set!" (s7_define_typed_function())

    generalized set: (set! (c-obj :k) v)

    in this case set! will call the set method registered with the
    c-obj's c-type, passing the c-obj, key :k, and value v.

    note that outside of this set! context, (c-obj :k) will lookup the
    value bound to :k in c-obj (using g_struct_get).
 */
#define G_CSTRUCT_SET_SPECIALIZED_HELP "(cstruct-set! b i x) sets the cstruct value at index i to x."

#define G_CSTRUCT_SET_SPECIALIZED_SIG s7_make_signature(s7, 4, s7_make_symbol(s7, "float?"), s7_make_symbol(s7, "cstruct?"), s7_make_symbol(s7, "integer?"), s7_make_symbol(s7, "float?"))

static s7_pointer _update_cstruct(s7_scheme *s7,
                                  struct cstruct_s *cstruct,
                                  s7_pointer key, s7_pointer val)
{
#ifdef DEBUG_TRACE
    log_debug("_update_cstruct");
#endif

    if (key == s7_make_keyword(s7, "substruct")) {
        if (s7_c_object_type(val) != cstruct_t) {
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a cstruct"));
        } else {
            log_debug("2 xxxxxxxxxxxxxxxx");
            cstruct->substruct = s7_c_object_value(val);
            return val;
        }
    }

    if (key == s7_make_keyword(s7, "c")) {
        if (!s7_is_character(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a character"));
        cstruct->c = s7_character(val);
        return val;
    }

    if (key == s7_make_keyword(s7, "str")) {
        if (!s7_is_string(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a string"));
        free(cstruct->str);
        /* s7_string() doc says "do not free the string", so we need
           to copy it? */
        int n = s7_string_length(val);
        cstruct->str = calloc(1, n);
        strncpy(cstruct->str, s7_string(val), n);
        return val;
    }

    if (key == s7_make_keyword(s7, "i")) {
        if (!s7_is_integer(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "an integer"));
        cstruct->i = s7_integer(val);
        return val;
    }

    if (key == s7_make_keyword(s7, "pi")) {
        if (!s7_is_integer(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "an integer"));
        *cstruct->pi = s7_integer(val);
        return val;
    }

    /* NB: typedef int64_t s7_int; */
    if (key == s7_make_keyword(s7, "l")) {
        if (!s7_is_integer(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a long int"));
        *cstruct->pi = s7_integer(val);
        return val;
    }

    if (key == s7_make_keyword(s7, "pl")) {
        if (!s7_is_integer(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a long int"));
        *cstruct->pl = s7_integer(val);
        return val;
    }

    if (key == s7_make_keyword(s7, "f")) {
        if (!s7_is_real(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a real"));
        cstruct->f = s7_real(val);
        return val;
    }
    if (key == s7_make_keyword(s7, "pf")) {
        if (!s7_is_real(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a real"));
        *cstruct->pf = s7_real(val);
        return val;
    }

    /* NB: typedef double s7_double; */
    if (key == s7_make_keyword(s7, "d")) {
        if (!s7_is_real(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a real"));
        cstruct->d = s7_real(val);
        return val;
    }

    if (key == s7_make_keyword(s7, "pd")) {
        if (!s7_is_real(val))
            return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                           3, val, "a real"));
        *cstruct->pd = s7_real(val);
        return val;
    }

  return(s7_error(s7, s7_make_symbol(s7, "key not found"),
                  s7_list(s7, 2, s7_make_string(s7, "key not found: ~S"),
                          key)));
}

static s7_pointer g_cstruct_set_specialized(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_set_specialized");
#endif
    struct cstruct_s *g;
    s7_int typ;
    s7_pointer obj, kw;

    /* set! methods/procedures need to check that they have
       been passed the correct number of arguments: 3 */

    if (s7_list_length(s7, args) != 3)
        return(s7_wrong_number_of_args_error(s7, "cstruct-set! takes 3 arguments: ~~S", args));

    obj = s7_car(args);
    /* qua procedure, check type of first arg */
    typ = s7_c_object_type(obj);
    if (typ != cstruct_t)
        return(s7_wrong_type_arg_error(s7, "cstruct-set!", 1, obj, "a cstruct"));

    if (s7_is_immutable(obj))
        return(s7_wrong_type_arg_error(s7, "cstruct-set!", 1, obj, "a mutable cstruct"));

    /* validate lookup key type - in this case, a keyword */
    kw = s7_cadr(args);
    if (!s7_is_keyword(kw))
        return(s7_wrong_type_arg_error(s7, "cstruct-set!",
                                       2, kw, "a keyword"));

    /* mutate to object: */
    g = (struct cstruct_s *)s7_c_object_value(obj);
    _update_cstruct(s7, g, kw, s7_caddr(args));
    //FIXME: r7rs says result of set! is unspecified. does that mean
    //implementation-specified?
    return s7_unspecified(s7);
}

static s7_pointer g_cstruct_set_generic(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_set_generic");
    /* debug_print_s7(s7, "set_generic spec: ", args); */
#endif
    return g_cstruct_set_specialized(s7, args);
}

static void _register_get_and_set(s7_scheme *s7)
{
    /* generic (SRFI 17) */
    s7_c_type_set_getter(s7, cstruct_t, s7_name_to_value(s7, "cstruct-ref"));
    s7_c_type_set_setter(s7, cstruct_t, s7_name_to_value(s7, "cstruct-set!"));
    // only for var setters, not fn setters?
    /* s7_set_setter(s7, */
    /*               s7_name_to_value(s7, "cstruct-ref"), */
    /*               /\* s7_make_symbol(s7, "cstruct-ref"), *\/ */
    /*               s7_name_to_value(s7, "cstruct-set!")); */

    /* set_ref should be called set_applicator or some such */
    s7_c_type_set_ref(s7, cstruct_t, g_cstruct_object_applicator);
    s7_c_type_set_set(s7, cstruct_t, g_cstruct_set_generic);
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

    char *buf;

#define WORKPTR (workptr = workbuf + strlen(workbuf))

    /* s7_int i, len, prec, old_len, loc, bytes; */
    struct cstruct_s *cs = (struct cstruct_s *)value;
    char *workptr = workbuf;
    sprintf(workptr, "#cstruct<\n"); WORKPTR;

    sprintf(workptr, "  c = '%c',\n", cs->c); WORKPTR;
    sprintf(workptr, " bits = #x%X,\n", cs->bits); WORKPTR;

    sprintf(workptr, "  str = \"%s\",\n", cs->str); WORKPTR;

    sprintf(workptr, "  sh_rt = %d,\n", cs->sh_rt); WORKPTR;
    if (cs->psh_rt == NULL)
        {sprintf(workptr, " psh_rt = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " psh_rt = %d,\n", *(cs->psh_rt)); WORKPTR;}

    sprintf(workptr, "  i = %d,\n", cs->i); WORKPTR;
    if (cs->pi == NULL)
        {sprintf(workptr, " pi = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pi = %d,\n", *(cs->pi)); WORKPTR;}

    sprintf(workptr, "  l = %ld,\n", cs->l); WORKPTR;
    if (cs->pl == NULL)
        {sprintf(workptr, " pl = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pl = %ld,\n", *(cs->pl)); WORKPTR;}

    sprintf(workptr, "  ll = %lld,\n", cs->ll); WORKPTR;
    if (cs->pll == NULL)
        {sprintf(workptr, " pll = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pll = %lld,\n", *(cs->pll)); WORKPTR;}

    sprintf(workptr, "  f = %f,\n", cs->f); WORKPTR;
    if (cs->pd == NULL)
        {sprintf(workptr, " pf = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pf = %f,\n", *(cs->pf)); WORKPTR;}

    sprintf(workptr, "  d = %f,\n", cs->d); WORKPTR;
    if (cs->pd == NULL)
        {sprintf(workptr, " pd = NULL,\n"); WORKPTR;}
    else
        {sprintf(workptr, " pd = %f,\n", *(cs->pd)); WORKPTR;}

    if (cs->substruct == NULL) {
        sprintf(workptr, " substruct = NULL,\n"); WORKPTR;
    } else {
        char *tmp = g_cstruct_display(s7, (struct cstruct_s*)cs->substruct);
        sprintf(workptr, " substruct = %s\n", tmp);
        free(tmp);              /* since sprint copies */
        WORKPTR;
    }

    sprintf(workptr, ">#");

    int len = strlen(workbuf);
    buf = (char *)calloc(1, len);
    strncpy(buf, workbuf, len);

    return buf;
}

/** g_cstruct_display_readably

    produces a "roundtrippable" string, one that when read by the reader
    results in an object equal to the original.
 */
static char *g_cstruct_display_readably(s7_scheme *s7, void *value)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_display_readably");
#endif

    //FIXME: use vasprintf?

    char workbuf[1024] = { '\0', };

    char *buf;

#define WORKPTR (workptr = workbuf + strlen(workbuf))

    struct cstruct_s *cs = (struct cstruct_s *)value;

    char *workptr = workbuf;
    sprintf(workptr, "("); WORKPTR;

    sprintf(workptr, "(c #\\%c)\n", cs->c); WORKPTR;
    sprintf(workptr, " (bits #x%X)\n", cs->bits); WORKPTR;
    sprintf(workptr, " (str \"%s\")\n", cs->str); WORKPTR;
    sprintf(workptr, " (sh_rt %d)\n", cs->sh_rt); WORKPTR;
    if (cs->psh_rt == NULL)
        {sprintf(workptr, " (psh_rt '())\n"); WORKPTR;}
    else
        {sprintf(workptr, " (psh_rt %d)\n", *(cs->psh_rt)); WORKPTR;}

    sprintf(workptr, " (i %d)\n", cs->i); WORKPTR;
    if (cs->pi == NULL)
        {sprintf(workptr, " (pi '())\n"); WORKPTR;}
    else
        {sprintf(workptr, " (pi %d)\n", *(cs->pi)); WORKPTR;}

    sprintf(workptr, " (l %ld)\n", cs->l); WORKPTR;
    if (cs->pl == NULL)
        {sprintf(workptr, " (pl '())\n"); WORKPTR;}
    else
        {sprintf(workptr, " (pl %ld)\n", *(cs->pl)); WORKPTR;}

    sprintf(workptr, " (ll %lld)\n", cs->ll); WORKPTR;
    if (cs->pll == NULL)
        {sprintf(workptr, " (pll '())\n"); WORKPTR;}
    else
        {sprintf(workptr, " (pll %lld)\n", *(cs->pll)); WORKPTR;}

    sprintf(workptr, " (f %f)\n", cs->f); WORKPTR;
    if (cs->pd == NULL)
        {sprintf(workptr, " (pf '())\n"); WORKPTR;}
    else
        {sprintf(workptr, " (pf %f)\n", *(cs->pf)); WORKPTR;}

    sprintf(workptr, " (d %f)\n", cs->d); WORKPTR;
    if (cs->pd == NULL)
        {sprintf(workptr, " (pd '())\n"); WORKPTR;}
    else
        {sprintf(workptr, " (pd %f)\n", *(cs->pd)); WORKPTR;}

    if (cs->substruct == NULL) {
        sprintf(workptr, " (substruct '())\n"); WORKPTR;
    } else {
        char *tmp = g_cstruct_display_readably(s7, (struct cstruct_s*)
                                               cs->substruct);
        sprintf(workptr, " (substruct %s)", tmp);
        free(tmp);              /* since sprint copies */
        WORKPTR;
        /* sprintf(workptr, ")"); */
    }


    sprintf(workptr, ")");

    int len = strlen(workbuf);
    buf = (char *)calloc(1, len);
    strncpy(buf, workbuf, len);

    // printf("5 WWWW: %s\n", workbuf);
    // printf("buf: %s\n", buf);
    return buf;
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

    free(descr); //BUG? FIXME free substruct strings
    return(obj);
}
/* /section: serialization */

/* **************************************************************** */
/* section: c-object construction */

/** g_cstruct_copy

    registered as c-type method using s7_c_type_set_copy

    this extends the predefined (copy ...) proc. to handle our custom
    c-type.

    in s7, 'copy' is generic, but it seems to be limited to sequences;
    maybe copy-seq would be better. OTOH it does work with e.g. a
    single int, as in (let* ((a 1) (b (copy a))) ...). But the two-arg
    version (copy a b) only works with sequences?

    But it's also not strictly copy; it may copy only part of the
    source, for example. And it's destructive. A better name might be
    update! or similar. (idiomatic english: "copy a to b" means "make
    b distinct from but identical to a".)

    if arg1 has cstruct type
        if cdr(args) is pair
            arg2 = cadr(args)
            check for mutability
            if arg2.type is cstruct_t
                ok to copy
                if optional 3rd arg ('start-cp-from' index)
                    ...
                else
                    copy arg1 data to arg1
            else ;; arg2.type is NOT cstruct_t
                g_block_copy assumes arg2 is a vector and copies data to it?
        else ;; only one arg, copy it
            make a new cstruct
            copy arg1 data to new cstruct
            return new cstruct
    else ;;  arg1 not cstruct, so arg2 must be

 */
static s7_pointer g_cstruct_copy(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_cstruct_copy");
    debug_print_s7(s7, "COPY ARGS: ", args);
    /* debug_print_s7(s7, "COPY ARGS len: ", s7_list_length(s7, args)); */
#endif

    s7_pointer arg1, arg2;
    struct cstruct_s *cs1, *cs2;

    arg1 = s7_car(args);
    if (s7_c_object_type(arg1) == cstruct_t) {
        log_debug("copy cstruct");
        cs1 = (struct cstruct_s*)s7_c_object_value(arg1);
        if (s7_is_pair(s7_cdr(args))) {
            log_debug("copy a b");
            arg2 = s7_cadr(args);
            if (s7_is_immutable(arg2))
                return(s7_wrong_type_arg_error(s7, "cstruct-copy!",
                                               0, arg2, "a mutable cstruct"));
            if (s7_c_object_type(arg2) == cstruct_t) {
                log_debug("copy cstruct to cstruct");
                cs2 = (struct cstruct_s*)s7_c_object_value(arg2);
                cstruct_copy(cs1, cs2);
                return arg2;
            } else {
                return(s7_wrong_type_arg_error(s7, "cstruct-copy!",
                                               0, arg2, "a mutable cstruct"));
            }
        } else {
            log_debug("copy one");
            /* only one arg, copy it to new cstruct */
            struct cstruct_s *cs2 = (struct cstruct_s *)
                calloc(1, sizeof(struct cstruct_s));
            cs2 = cstruct_init_default(cs2);
            cstruct_copy(cs1, cs2);
            return s7_make_c_object(s7, cstruct_t,
                                    (void *)cs2);
        }
    } else {
        log_debug("copy non-cs");
        /* arg1 type != cstruct_t */
    }
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
    /* debug_print_s7(s7, "INIT ARGS: ", args); */
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

        int len;
        switch(i) {
        case FLD_C:
            if (!s7_is_character(arg)) {
                return(s7_wrong_type_arg_error(s7, "make-cstruct",
                                               1, arg, "a char"));
            }
            cs->c = s7_character(arg);
            break;
        case FLD_BYTE:
            if (!s7_is_integer(arg)) {
                return(s7_wrong_type_arg_error(s7, "make-cstruct",
                                               1, arg, "an integer < 256"));
            }
            cs->bits = (unsigned char)s7_integer(arg);
            break;
        case FLD_STR:
            len = s7_string_length(arg);
            cs->str = calloc(1, len);
            strncpy(cs->str, s7_string(arg), len);
            /* cs->str = s7_string(arg); */
        case FLD_B:                 /* bool */
            cs->b = s7_boolean(s7, arg);
            break;
        case FLD_PB:                 /* bool ptr */
            cs->pb = (bool*)calloc(1, sizeof(bool));
            *cs->pb = s7_boolean(s7, arg);
            break;

        case FLD_SH_RT:                 /* short */
            cs->i = s7_integer(arg);
            break;
        case FLD_PSH_RT:                 /* short ptr */
            cs->psh_rt = (short*)calloc(1, sizeof(short));
            *cs->psh_rt = s7_integer(arg);
            break;

            /* s7_integer returns s7_int = int64_t,
               big enough for long long int */
        case FLD_I:                 /* int */
            cs->i = s7_integer(arg);
            break;
        case FLD_PI:                 /* int* */
            cs->pi = (int*)calloc(1, sizeof(int));
            *cs->pi = s7_integer(arg);
            break;
        case FLD_L:                 /* long */
            cs->l = s7_integer(arg);
            break;
        case FLD_PL:                 /* long ptr */
            cs->pl = (long*)calloc(1, sizeof(long));
            *cs->pl = s7_integer(arg);
            break;
        case FLD_LL:                 /* long long */
            cs->ll = s7_integer(arg);
            break;
        case FLD_PLL:                 /* long long ptr */
            cs->pll = (long long*)calloc(1, sizeof(long long));
            *cs->pll = s7_integer(arg);
            break;
        case FLD_F:                 /* float */
            cs->f = s7_real(arg);
            break;
        case FLD_PF:                 /* float ptr */
            cs->pf = (float*)calloc(1, sizeof(float));
            *cs->pf = s7_real(arg);
            break;
        case FLD_D:                 /* double */
            cs->d = s7_real(arg);
            break;
        case FLD_PD:                 /* double ptr */
            cs->pd = (double*)calloc(1, sizeof(double));
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

#define MAKE_CSTRUCT_FORMAL_PARAMS "(c #\\a) (bits #xFF) (str \"hello\") (b #f) (pb #f) (sh_rt 7) (psh_rt 8) (i 1) (pi 2) (l 3) (pl 4) (ll 3333) (pll 4444) (f 5.0) (pf 6.0) (d 7) (pd 8)"
static s7_pointer g_new_cstruct(s7_scheme *s7, s7_pointer args)
{
#ifdef DEBUG_TRACE
    log_debug("g_new_cstruct");
#endif

    struct cstruct_s *new_cstruct = (struct cstruct_s *)
        calloc(1, sizeof(struct cstruct_s));
    new_cstruct = cstruct_init_default(new_cstruct);
    if (g_cstruct_init_from_s7(s7, new_cstruct, args) != NULL) {
        log_debug("OOPS");
    }

    s7_pointer new_cstruct_s7 = s7_make_c_object(s7, cstruct_t,
                                                 (void *)new_cstruct);

    _register_c_object_methods(s7, new_cstruct_s7);

    return(new_cstruct_s7);
}
/* /section: c-object construction */

/* **************************************************************** */
/* section: c-object destruction */
static s7_pointer g_destroy_cstruct(s7_scheme *s7, s7_pointer obj)
{
#ifdef DEBUG_TRACE
    log_debug("g_destroy_cstruct");
#endif
    struct cstruct_s *cs = (struct cstruct_s*)s7_c_object_value(obj);
    cstruct_free(cs);
    return NULL;
}
/* /section: c-object destruction */

/* section: extension methods */
/* extension methods extend standard Scheme procedures like 'length'
   and 'equals' to support custom c-types.

   unsupported methods return #f (?)
 */

#define METHODS_PREFIX   "(openlet (immutable! (inlet "
#define METHODS_POSTFIX  ")))"

#define OBJECT_METHODS \
    "'float-vector? (lambda (p) (display \"foo \") #t) " \
    "'signature (lambda (p) (list '#t 'cstruct? 'integer?)) " \
    "'type cstruct? " \
    "'foo (lambda (self) \"hello from foo method!\") " \
    "'memq (lambda (self arg) \"hello from perverse memq method!\") " \
    "'arity (lambda (p) (cons 1 1)) " \
    "'aritable? (lambda (p args) (= args 1)) " \
    "'vector-dimensions (lambda (p) (list (length p))) " \
    "'empty (lambda (p) (zero? (length p))) " \
    "'ref cstruct-ref " \
    "'vector-ref cstruct-ref " \
    "'vector-set! cstruct-set! "
    /* "'reverse! cstruct-reverse! " \ */
    /* "'subsequence subcstruct " \ */
    /* "'append cstruct-append " */

/* object methods: registered on each object, not type */
static void _register_c_object_methods(s7_scheme *s7, s7_pointer cstruct)
{
#ifdef DEBUG_TRACE
    log_debug("_register_c_object_methods");
#endif
    static bool initialized = false;
    if (!initialized) {
        g_cstruct_methods_let = s7_eval_c_string(s7, METHODS_PREFIX OBJECT_METHODS METHODS_POSTFIX);
        s7_gc_protect(s7, g_cstruct_methods_let);
        initialized = true;
    }
    s7_c_object_set_let(s7, cstruct, g_cstruct_methods_let);
    s7_openlet(s7, cstruct);
}

static void _register_c_type_methods(s7_scheme *s7, s7_int cstruct_t)
{
#ifdef DEBUG_TRACE
    log_debug("_register_c_type_methods");
#endif
    s7_c_type_set_gc_free(s7, cstruct_t, g_destroy_cstruct);
    s7_c_type_set_gc_mark(s7, cstruct_t, g_cstruct_gc_mark);

    /* s7_c_type_set_equal(s7, cstruct_t, _cstructs_are_c_eql); */
    s7_c_type_set_is_equal(s7, cstruct_t, g_cstructs_are_equal);
    s7_c_type_set_is_equivalent(s7, cstruct_t, g_cstructs_are_equivalent);

    s7_c_type_set_copy(s7, cstruct_t, g_cstruct_copy);
    /* s7_c_type_set_length(s7, cstruct_t, g_cstruct_length); */
    /* s7_c_type_set_reverse(s7, cstruct_t, g_cstruct_reverse); */
    /* s7_c_type_set_fill(s7, cstruct_t, g_cstruct_fill); */

    s7_c_type_set_to_string(s7, cstruct_t, g_cstruct_to_string);

}

/* /section: extension methods */


/* **************************************************************** */
/* section: c-type configuration */

static void _register_cstruct_fns(s7_scheme *s7)
{
#ifdef DEBUG_TRACE
    log_debug("_register_cstruct_fns");
#endif
    /* s7_define_safe_function(s7, "cstruct", g_to_cstruct, 0, 0, true, g_cstruct_help); */
    s7_define_safe_function_star(s7, "make-cstruct", g_new_cstruct,
                                 MAKE_CSTRUCT_FORMAL_PARAMS,
                                 g_new_cstruct_help);

    s7_define_typed_function(s7, "cstruct?", g_is_cstruct, 1, 0, false, g_is_cstruct_help, g_is_cstruct_sig);

    /* specialized get/set! */
    s7_define_typed_function(s7, "cstruct-ref", g_cstruct_ref_specialized, 2, 0, false, G_CSTRUCT_REF_SPECIALIZED_HELP, G_CSTRUCT_REF_SPECIALIZED_SIG);
    s7_define_typed_function(s7, "cstruct-set!", g_cstruct_set_specialized, 3, 0, false, G_CSTRUCT_SET_SPECIALIZED_HELP, G_CSTRUCT_SET_SPECIALIZED_SIG);

    // cstruct-let => s7_c_object_let, a let for the instance not the type
    /* s7_define_safe_function(s7, "cstruct-let", g_cstruct_let, 1, 0, false, g_cstruct_let_help); */

    /* s7_define_safe_function(s7, "subcstruct", g_subcstruct, 1, 0, true, g_subcstruct_help); */
    /* s7_define_safe_function(s7, "cstruct-append", g_cstruct_append, 0, 0, true, g_cstruct_append_help); */
    /* s7_define_safe_function(s7, "cstruct-reverse!", g_cstruct_reverse_in_place, 1, 0, false, g_cstruct_reverse_in_place_help); */
}

/* **************** */
/** configure_s7_cstruct_type(s7_scheme *s7)

    public
*/
//FIXME: return int rc?
int configure_s7_cstruct_type(s7_scheme *s7)
{
#ifdef DEBUG_TRACE
    log_debug("configure_s7_cstruct_type");
#endif
    /* s7_int t = _make_c_type(s7); */
    cstruct_t = s7_make_c_type(s7, "<cstruct>");
    _register_c_type_methods(s7, cstruct_t);
    _register_get_and_set(s7);
    _register_cstruct_fns(s7);
    s7_provide(s7, "cstruct");
    return cstruct_t;
}
/* section: c-type configuration */

/* **************************************************************** */
/* section: gc */
static s7_pointer g_cstruct_gc_mark(s7_scheme *s7, s7_pointer p)
{
    /* nothing to mark because we protect g_cstruct_methods_let, and all cstruct objects get the same let */
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
    /* s7_display(s7, s7_make_string(s7, label), p); */
    log_debug("label: %s", label);
    log_debug("%s", s7_object_to_c_string(s7, obj));
    /* s7_display(s7, obj, p); */
    /* s7_newline(s7, p); */
}

/* /section: debugging */
