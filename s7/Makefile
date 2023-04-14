repl: s7
	gcc s7.c -o repl -DWITH_MAIN -I. -O2 -g -ldl -lm

# linux: -Wl,--export-dynamic

# gcc -o repl repl.c s7.o -lm -I.

s7:
	gcc -c s7.c -I.
