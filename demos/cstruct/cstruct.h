#include <complex.h>
#include <stdatomic.h>

/* TODO: cover all C types */
typedef void (*myfun_t) (void);
struct cstruct_s {
    struct cstruct_s *substruct;

    /* TODO: signed and unsigned */
    char    c;
    signed char sc;
    unsigned char bits;         /* byte */

    bool     b;
    bool   *pb;

    /* arithmetic types */
    /* https://en.cppreference.com/w/c/language/arithmetic_types */
    short    sh_rt;
    short  *psh_rt;
    int      i;
    int    *pi;
    long     l;
    long   *pl;
    long long ll;
    long long *pll;
    float    f;
    float  *pf;
    double   d;
    double *pd;

    /* complex numbers */
    float complex fc;
    double complex dc;
    long double complex ldc;

    // imaginary not supported on macos c11
    /* float _Imaginary fi; */
    /* double _Imaginary di; */
    /* long double _Imaginary ldi; */

    /* void *vp; */

    /* strings - TODO: unicode, w_char, etc */
    char   *str; /* owned (and freed) by libcstruct */
    char  **strs[4];                /* list of 4 strings */

    /* array types */
    unsigned char ba[8];           /* byte array */
    int ia[4];
    int ia2[2][3];

    /* bitfields */
    struct {
        /* example from https://en.cppreference.com/w/c/language/bit_field */
        /* will usually occupy 8 bytes: */
        /* 5 bits: value of b1 */
        /* 27 bits: unused */
        /* 6 bits: value of b2 */
        /* 15 bits: value of b3 */
        /* 11 bits: unused */
        unsigned b1 : 5;
        unsigned :0; /* start a new unsigned int */
        unsigned b2 : 6;
        unsigned b3 : 15;
    }  bitfield;

    /* TODO: function types */
    myfun_t *fn;

    /* atomic types: https://en.cppreference.com/w/c/language/atomic */
    /* https://en.cppreference.com/w/c/atomic */
    atomic_bool atomb;          /* _Atomic _Bool */
    atomic_char atomch;         /* _Atomic char */
    atomic_int atomint;         /* _Atomic int */
    //etc.
};

int run_cstruct(s7_scheme *s7);

void cstruct_free(struct cstruct_s *cs);
