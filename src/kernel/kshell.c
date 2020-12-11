#include "kshell.h"
#include <core/debug.h>
#include <core/elf.h>
#include <drivers/cmos.h>
#include <drivers/timer.h>
#include <fs/debug.h>
#include <fs/vfs.h>
#include <net/dns.h>
#include <net/ipv4.h>
#include <net/net.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

static char readline[READLINE_SIZE] = { 0 };
static char last_readline[READLINE_SIZE] = { 0 };
static unsigned int readline_index = 0;

static bool caps_lock_mode = false;
static bool ctrl_mode = false;
static bool shift_mode = false;

#define NB_DOCUMENTED_COMMANDS 7

static const char* commands[][NB_DOCUMENTED_COMMANDS] = {
  { "help", "display information about system shell commands" },
  { "host", "perform a DNS lookup" },
  { "ls", "list files" },
  { "net", "show configured network interfaces" },
  { "overflow", "test the stack buffer overflow protection" },
  { "ping", "ping an IPv4 address" },
  { "selftest", "run the system test suite" },
};

static unsigned char keymap[][128] = {
  { 0 },         { 27 },       { '1', '!' }, { '2', '@' },  { '3', '#' },
  { '4', '$' },  { '5', '%' }, { '6', '^' }, { '7', '&' },  { '8', '*' },
  { '9', '(' },  { '0', ')' }, { '-', '_' }, { '=', '+' },  { '\b' },
  { '\t' },      { 'q', 'Q' }, { 'w', 'W' }, { 'e', 'E' },  { 'r', 'R' },
  { 't', 'T' },  { 'y', 'Y' }, { 'u', 'U' }, { 'i', 'I' },  { 'o', 'O' },
  { 'p', 'P' },  { '[', '{' }, { ']', '}' }, { '\n' },      { 0 },
  { 'a', 'A' },  { 's', 'S' }, { 'd', 'D' }, { 'f', 'F' },  { 'g', 'G' },
  { 'h', 'H' },  { 'j', 'J' }, { 'k', 'K' }, { 'l', 'L' },  { ';', ':' },
  { '\'', '"' }, { '`', '~' }, { 0 },        { '\\', '|' }, { 'z', 'Z' },
  { 'x', 'X' },  { 'c', 'C' }, { 'v', 'V' }, { 'b', 'B' },  { 'n', 'N' },
  { 'm', 'M' },  { ',', '<' }, { '.', '>' }, { '/', '?' },  { 0 },
  { '*' },       { 0 },        { ' ' },      { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { '-' },
  { 0 },         { 0 },        { 0 },        { '+' },       { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },        { 0 },         { 0 },
  { 0 },         { 0 },        { 0 },
};

unsigned char get_char(uint8_t scancode, bool shift, bool caps_lock)
{
  if ((caps_lock || shift) && keymap[scancode][1]) {
    return keymap[scancode][1];
  }

  return keymap[scancode][0];
}

void help(int argc, char* argv[])
{
  if (argc < 2) {
    for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
      printf("%-10s %s\n", commands[i][0], commands[i][1]);
    }

    return;
  }

  for (uint8_t i = 0; i < NB_DOCUMENTED_COMMANDS; i++) {
    if (strncmp(argv[1], commands[i][0], strlen(commands[i][0])) == 0) {
      printf("%s - %s\n", argv[1], commands[i][1]);
      return;
    }
  }

  printf("no help for this command\n");
}

void print_selftest_header(const char* name)
{
  printf("\n\033[1;33m[%s]\033[0m\n", name);
}

void selftest()
{
  print_selftest_header("interrupts");
  printf("  invoking breakpoint exception\n");
  __asm__("int3");

  print_selftest_header("syscalls");
  printf("  syscalling\n");
  test("kshell");

  print_selftest_header("memory");
  char* str = (void*)0x42;
  printf("  pointer before malloc(): p=%p\n", str);
  int str_len = 9;
  str = (char*)malloc(str_len * sizeof(char));

  if (str == 0) {
    printf("  failed\n");
  } else {
    printf("  success! p=%p", str);
    strncpy(str, "it works", str_len);
    printf(" and value is: %s\n", str);
    free(str);
  }

  print_selftest_header("filesystem");
  inode_t debug = vfs_namei(FS_DEBUG_MOUNTPOINT);
  const char* message = "  this message should be written to the console\n";
  vfs_write(debug, (void*)message, strlen(message), 0);
  vfs_free(debug);

  printf("\ndone.\n");
}

void net()
{
  uint8_t in_id = 0;
  net_interface_t* in = net_get_interface(in_id);

  printf("eth%d:\n", in_id);
  printf("  driver : %s\n", in->driver->get_name());
  printf(
    "  ip     : %d.%d.%d.%d\n", in->ip[0], in->ip[1], in->ip[2], in->ip[3]);
  printf("  mac    : %02x:%02x:%02x:%02x:%02x:%02x\n",
         in->mac[0],
         in->mac[1],
         in->mac[2],
         in->mac[3],
         in->mac[4],
         in->mac[5]);
  printf("  gateway: %d.%d.%d.%d\n",
         in->gateway_ip[0],
         in->gateway_ip[1],
         in->gateway_ip[2],
         in->gateway_ip[3]);
  printf("  dns    : %d.%d.%d.%d\n",
         in->dns_ip[0],
         in->dns_ip[1],
         in->dns_ip[2],
         in->dns_ip[3]);
}

void busywait(uint32_t delay_in_seconds)
{
  uint64_t t = timer_uptime();
  while (timer_uptime() < (t + delay_in_seconds)) {
    ;
  }
}

void host(int argc, char* argv[])
{
  if (argc != 2) {
    printf("usage: %s domain.tld\n", argv[0]);
    return;
  }

  net_interface_t* in = net_get_interface(0);

  printf("DNS lookup for: %s\n", argv[1]);
  dns_request(in, argv[1]);

  busywait(2);
}

void ping(int argc, char* argv[])
{
  if (argc != 5) {
    printf("usage: %s w x y z\n", argv[0]);
    return;
  }

  uint8_t ip[4] = {
    (uint8_t)atoi(argv[1]),
    (uint8_t)atoi(argv[2]),
    (uint8_t)atoi(argv[3]),
    (uint8_t)atoi(argv[4]),
  };

  net_interface_t* in = net_get_interface(0);

  printf("PING %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  ipv4_ping(in, ip);

  busywait(2);
}

void ls(int argc, char* argv[])
{
  inode_t inode = argc == 1 ? vfs_namei("/") : vfs_namei(argv[1]);

  if (!inode) {
    printf("no such file or directory\n");
    return;
  }

  if (vfs_inode_type(inode) != FS_DIRECTORY) {
    printf("'%s' is not a directory\n", inode->name);
    vfs_free(inode);
    return;
  }

  uint64_t num = 0;

  while (1) {
    dirent_t* de = vfs_readdir(inode, num++);

    if (!de) {
      break;
    }

    stat_t stat;
    vfs_stat(de->inode, &stat);
    printf("%6llu %s\n", stat.size, de->name);

    vfs_free(de->inode);
    free(de);
  }

  vfs_free(inode);
}

int try_exec(int argc, char* argv[])
{
  inode_t inode = vfs_namei(argv[0]);

  if (!inode) {
    char* buf = malloc(strlen(argv[0]) + 5);
    sprintf(buf, "/bin/%s", argv[0]);
    inode = vfs_namei(buf);
    free(buf);
  }

  if (!inode) {
    return -1;
  }

  if (vfs_inode_type(inode) != FS_FILE) {
    vfs_free(inode);
    return -2;
  }

  stat_t stat;
  vfs_stat(inode, &stat);

  char* buf = malloc((stat.size + 1) * sizeof(char));
  vfs_read(inode, buf, stat.size, 0);
  buf[stat.size] = '\0';

  elf_header_t* elf = elf_load((uint8_t*)buf);

  if (!elf) {
    vfs_free(inode);
    free(buf);
    return -3;
  }

  typedef int callable(int arvc, char* argv[]);
  callable* c = (callable*)(elf->entry);
  int retval = c(argc, argv);

  if (retval != 0) {
    printf("%s returned non-zero exit status %d.\n", argv[0], retval);
  }

  elf_unload(elf);

  vfs_free(inode);
  free(buf);

  return 0;
}

void overflow()
{
  char c[12];
  strcpy(c, "123456789012345678901234567890");
}

void run_command()
{
  if (*readline == 0) {
    return;
  }

  int argc = 1;
  char* _readline = strdup(readline);
  strtok(_readline, " ");
  while (strtok(NULL, " ") != NULL) {
    argc++;
  }
  free(_readline);

  char** argv = malloc(sizeof(char*) * argc);
  argv[0] = strtok(readline, " ");
  DEBUG_OUT("command='%s' argc=%d", argv[0], argc);

  for (int i = 1; i < argc; i++) {
    argv[i] = strtok(NULL, " ");
    DEBUG_OUT("argv[%d]=%s", i, argv[i]);
  }

  if (strcmp(argv[0], "help") == 0) {
    help(argc, argv);
  } else if (strcmp(argv[0], "ls") == 0) {
    ls(argc, argv);
  } else if (strcmp(argv[0], "selftest") == 0) {
    selftest();
  } else if (strcmp(argv[0], "overflow") == 0) {
    overflow();
  } else if (strcmp(argv[0], "net") == 0) {
    net(argc, argv);
  } else if (strcmp(argv[0], "ping") == 0) {
    ping(argc, argv);
  } else if (strcmp(argv[0], "host") == 0) {
    host(argc, argv);
  } else {
    if (try_exec(argc, argv) != 0) {
      printf("invalid kshell command\n");
    }
  }

  free(argv);
}

void kshell_print_prompt()
{
  printf("\033[0;36m%s\033[0m", PROMPT);
}

void reset_readline()
{
  readline_index = 0;

  for (unsigned int i = 0; i < READLINE_SIZE; i++) {
    readline[i] = 0;
  }
}

void kshell_run(uint8_t scancode)
{
  if (scancode == 0) {
    return;
  }

  bool key_was_released = false;

  if (scancode > 128) {
    key_was_released = true;
    scancode -= 128;
  }

  switch (scancode) {
    case KB_CAPS_LOCK:
      if (key_was_released) {
        caps_lock_mode = !caps_lock_mode;
      }

      break;

    case KB_ARROW_UP:
      if (key_was_released && strlen(last_readline) > 0) {
        reset_readline();
        strncpy(readline, last_readline, READLINE_SIZE);
        memset(last_readline, 0, READLINE_SIZE);
        printf("%s", readline);
        readline_index = strlen(readline);
      }

      break;

    case KB_LSHIFT:
    case KB_RSHIFT:
      if (key_was_released) {
        shift_mode = false;
      } else {
        shift_mode = true;
      }

      break;

    case KB_LCTRL:
      if (key_was_released) {
        ctrl_mode = false;
      } else {
        ctrl_mode = true;
      }

      break;

    case KB_BACKSPACE:
      if (key_was_released && readline_index > 0) {
        // destructive backspace
        printf("\b \b");
        readline_index--;
        readline[readline_index] = 0;
      }

      break;

    case KB_ENTER:
      if (key_was_released) {
        printf("\n");
        strncpy(last_readline, readline, READLINE_SIZE);
        run_command();
        reset_readline();
        kshell_print_prompt();
      }

      break;

    case KB_TAB:
      if (key_was_released) {
        printf("  ");
        readline[readline_index++] = 0;
        readline[readline_index++] = 0;
      }

      break;

    default:
      if (key_was_released) {
        unsigned char c = get_char(scancode, shift_mode, caps_lock_mode);

        if (c) {
          // Handle keyboard shortcuts.
          if (ctrl_mode) {
            switch (c) {
              case 'c':
                readline[readline_index++] = '^';
                readline[readline_index++] = 'C';
                printf("^C\n");
                kshell_print_prompt();
                break;

              case 'l':
                printf("\033[H\033[2J");
                reset_readline();
                kshell_print_prompt();
                break;
            }
          } else {
            printf("%c", c);
            readline[readline_index++] = c;
          }
        }
      }
  }
}
