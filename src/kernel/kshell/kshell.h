#ifndef PRIVATE_KSHELL_H
#define PRIVATE_KSHELL_H

#include <kshell/kshell.h>

void cat(int argc, char* argv[]);
void date();
void exec(int argc, char* argv[]);
void host(int argc, char* argv[]);
void ls(int argc, char* argv[]);
void net();
void ntp(int argc, char* argv[]);
void overflow();
void ping(int argc, char* argv[]);
void power_off(int cmd);
int selftest();
void ps();

#endif
