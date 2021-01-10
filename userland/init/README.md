# init

The `init` program is designed to be the first program to be executed in user
mode by the kernel. It has several options as displayed below. By default, the
program does nothing. The kernel passes the `-s` option by default in order to
run the user shell.

```
$ ./init -h
init: ../local-build/init started with pid=42
usage: init [-hs]

  s : run the user shell
  h : print this help message
```

Important: this program never stops, unless there is an error when calling
`execv()` (or a bug).
