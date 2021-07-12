struct cstruct_s {
    char    c;
    char   *s; /* owned (and freed) by libcstruct */

    bool     b;
    bool   *pb;
    int      i;
    int    *pi;
    long     l;
    long   *pl;
    float    f;
    float  *pf;
    double   d;
    double *pd;
    void *vp;
};

int run_cstruct(s7_scheme *s7);

void cstruct_free(struct cstruct_s *cs);
