#include "debug.h"
#include <stdio.h>
#include <string.h>

uint64_t debug_stat(inode_t node, struct stat* st);
uint64_t debug_isatty(inode_t node);
uint64_t debug_write(inode_t node, void* buffer, uint64_t size, uint64_t offset);

vfs_driver_t debug_driver = {
    0, // open
    0, // close
    0, // read
    debug_write, // write
    0, // link
    0, // unlink
    debug_stat, // stat
    debug_isatty, // isatty
    0, // mkdir
    0, // readdir
    0 // finddir
};

inode_t debug_fs_init() {
    inode_t node = malloc(sizeof(vfs_node_t));

    strcpy(node->name, "debug");
    node->driver = &debug_driver;
    node->type = FS_CHARDEVICE;

    return node;
}

uint64_t debug_stat(inode_t node, stat_t* st) {
    memset(st, 0, sizeof(stat_t));
    st->size = 0;
    return 0;
}

uint64_t debug_isatty(inode_t node) {
    return 1;
}

uint64_t debug_write(inode_t node, void* buffer, uint64_t size, uint64_t offset) {
    if (offset < size) {
        printf(&buffer[offset]);
        return size - offset;
    }

    return 0;
}
