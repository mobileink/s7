## s7/repl emits libc_s7.c, then compiles it. It contains
## #include <s7/s7.h>
## so it must be run from this dir.

default:
	${MAKE} -C s7 repl;
	./s7/repl
