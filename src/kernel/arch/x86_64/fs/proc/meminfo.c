#include <fs/proc.h>

#include <initcall.h>
#include <inttypes.h>
#include <mmu/alloc.h>
#include <mmu/frame.h>
#include <stdio.h>
#include <string.h>

int proc_read_meminfo(char* buf, size_t size, uint64_t offset)
{
  uint64_t used_frames = frame_get_used_count();
  uint64_t max_frames = frame_get_max_count();
  uint64_t used_heap = alloc_get_used_count();
  uint64_t max_heap = alloc_get_max_count();

  snprintf(buf,
           size,
           "frames: %6" PRIu64 "/%" PRIu64 "\n"
           "heap  : %6" PRIu64 "/%" PRIu64 " KiB [%" PRIu64 "/%" PRIu64 "]\n",
           used_frames,
           max_frames,
           (used_heap * PAGE_SIZE) / 1024,
           (max_heap * PAGE_SIZE) / 1024,
           used_heap,
           max_heap);

  size_t len = strlen(buf);
  if (offset >= len) {
    buf[0] = '\0';
    return 0;
  }

  if (offset > 0) {
    memmove(buf, buf + offset, len - offset + 1);
  }

  return 0;
}

int proc_meminfo_init()
{
  proc_fs_register_readonly_file("meminfo", &proc_read_meminfo);

  return 0;
}

initcall_fs_register(proc_meminfo_init);
