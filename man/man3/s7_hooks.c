.Dd July 10, 2021
.Dt S7 3
.Sh NAME
.Nm S7
.Nd Hooks
.Sh LIBRARY
libs7, -ls7
.Sh SYNOPSIS
.In s7.h
void (*s7_begin_hook(s7_scheme *sc))(s7_scheme *sc, bool *val);
void s7_set_begin_hook(s7_scheme *sc, void (*hook)(s7_scheme *sc, bool *val));
.Sh DESCRIPTION
Call "hook" at the start of any block; use NULL to cancel.
.Fn s7_begin_hook
returns the current begin_hook function or NULL.
.Sh EXAMPLES
.Bd -literal -offset indent
verbatim code here...
.Ed
.Pp
.Sh SEE ALSO
.Xr s7_c_objects 3 ,
.Sh AUTHORS
.An Bill Schottstaedt Aq Mt bil@ccrma.stanford.edu
.An Mike Scholz
provided the FreeBSD support (complex trig funcs, etc)
.An Rick Taube, Andrew Burnson, Donny Ward, and Greg Santucci
provided the MS Visual C++ support
.An Kjetil Matheussen
provided the mingw support
.An chai xiaoxiang
provided the msys2 support
