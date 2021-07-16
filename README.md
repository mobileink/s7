# s7

s7 is a Scheme interpreter.

**IMPORTANT**: [This](https://github.com/mobileink/s7) is a fork of https://cm-gitlab.stanford.edu/bil/s7. Its only purpose is to serve as a place to keep manpages (derived from `s7.h`) and demo code.

This is branch 'main'; the original repo is on branch 'master'.

The original README and source code is on this branch in [s7/](s7).

## docs

### manpages

You can open them by file path:

`$ man ./man/man7/s7.7`

Or you can put them on the manpath:

* MacOS looks for man directories beside the directories that are on
  the path; run `manpath -d` to see this. So the easy way to add the
  manpages is to add `~/bin` to your path, and softlink the s7
  manpages to `~/man`. So if you've cloned s7 to `~/s7`:

```
~/ $ mkdir man
~/ $ cd man
~/ $ ln -s ../s7/man3 man/
~/ $ ln -s ../s7/man7 man/
```

[NB: You can also use absolute paths; but this will not work: `~/ $ ln -s s7/man/man3 man/`. The link becomes `man7 => s7/man7` which won't resolve.]

## demo code

Currently there is just one simple demo, demonstrating some of "load" capabilities:

`$ bazel run demos:driver -- -d 1`

NB: don't forget the double-dash `--`.

A more complete (but still unfinished) demo is:

`$ bazel run demos/cstruct:driver`

It exports a C struct and exercises a good portion of the API.
Eventually I'd like to turn it into a template for generating new s7
extension projects. For now, I use it for exploring the API, so it has
lots of log messages, and the Scheme file (demos/cstruct/cstruct.scm)
has a bunch of code for exploring the API.

## tests

`test/g_block` contains C code extracted from `s7/s7test.scm`. To run:

`$ bazel run test/g_block`

