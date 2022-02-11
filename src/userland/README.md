# Component: userland

This directory contains programs that are compiled using the ArvernOS _libc_ and
bundled into the initial RAM disk (`initrd`) when building the ISO.

- `local-build/` contains the binaries compiled for a "local environment" (see
  below)
- the other directories are programs

## Local builds

Ideally, all programs should be able to run on a local environment, i.e. your
current system or the Docker-based environment bundled with this project at
least.

It is possible to compile each program locally by running:

```
$ make clean
$ make local
```

<!-- doxygen:
## Related pages

* \subpage refInit
-->
