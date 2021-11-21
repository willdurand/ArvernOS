/** @file */
#ifndef KMAIN_H
#define KMAIN_H

void kmain_print_banner();

void kmain_start(const char* cmdline);

// utils
void print_ko();
void print_ok();
void print_step(const char* msg);

#endif
