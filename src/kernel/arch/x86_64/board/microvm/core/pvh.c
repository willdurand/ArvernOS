#include "pvh.h"

#include <stdio.h>

static const char* cmdline = NULL;

void pvh_init(hvm_start_info_t* info)
{
  cmdline = (char*)info->cmdline_paddr;
}

const char* pvh_get_cmdline()
{
  return cmdline;
}
