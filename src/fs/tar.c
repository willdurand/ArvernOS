#include "tar.h"
#include <stdint.h>
#include <string.h>
#include <core/debug.h>

uint64_t get_size(const char* in);
uint64_t tar_read_headers(uint64_t address);
uint64_t debug_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
inode_t tar_finddir(inode_t inode, const char* name);
dirent_t* tar_readdir(inode_t inode, uint64_t num);

tar_header_t* headers[32];

vfs_driver_t tar_driver = {
    0, // open
    0, // close
    debug_read, // read
    0, // write
    0, // link
    0, // unlink
    0, // stat
    0, // isatty
    0, // mkdir
    tar_readdir, // readdir
    tar_finddir, // finddir
};

inode_t tar_fs_init(uint64_t address) {
    inode_t node = malloc(sizeof(vfs_node_t));

    strcpy(node->name, "tar");
    node->driver = &tar_driver;
    node->type = FS_DIRECTORY;

    tar_read_headers(address);

    return node;
}

uint64_t tar_read_headers(uint64_t address) {
    uint64_t i;

    for (i = 0; ; i++) {
        tar_header_t* header = (tar_header_t*)address;

        if (header->filename[0] == '\0') {
            break;
        }

        uint64_t size = get_size(header->size);
        address += ((size / 512) + 1) * 512;

        if (size % 512) {
            address += 512;
        }

        headers[i] = header;

    }

    return i;

}

uint64_t get_size(const char* in) {

    uint64_t size = 0;
    uint64_t j;
    uint64_t count = 1;

    for (j = 11; j > 0; j--, count *= 8) {
        size += ((in[j - 1] - '0') * count);
    }

    return size;

}

uint64_t debug_read(inode_t node, void* buffer, uint64_t size, uint64_t offset) {
    DEBUG("name=%s type=%d", node->name, node->type);

    if (node->type == FS_FILE) {
        tar_header_t* header = headers[node->entry];
        uint64_t size = get_size(header->size);
        memcpy(buffer, (void*)header + 512, size);
        return size;
    }

    return 0;
}

inode_t tar_finddir(inode_t inode, const char* name) {
    DEBUG("finding name=%s", name);
    inode_t node = malloc(sizeof(vfs_node_t));

    tar_header_t* header = 0;

    for (uint64_t i = 0; headers[i] != 0; i++) {
        DEBUG("reading filename=%s name=%s", headers[i]->filename, name);

        if (strncmp(name, headers[i]->filename, strlen(name)) == 0) {
            header = headers[i];
            node->entry = i;
            break;
        }
    }

    if (!header) {
        DEBUG("did not find name=%s", name);
        free(node);
        return 0;
    }

    strcpy(node->name, header->filename);
    node->driver = &tar_driver;

    switch (header->type) {
        case TAR_FILE:
            node->type = FS_FILE;
            break;

        case TAR_DIRECTORY:
            node->type = FS_DIRECTORY;
            break;
    }

    DEBUG("found name=%s type=%d", node->name, node->type);

    return node;
}

dirent_t* tar_readdir(inode_t inode, uint64_t num) {
    dirent_t* dir = malloc(sizeof(dirent_t));
    inode_t node = malloc(sizeof(vfs_node_t));

    tar_header_t* header = 0;

    uint64_t j = 0;

    for (uint64_t i = 0; headers[i] != 0; i++) {
        DEBUG("reading filename=%s", headers[i]->filename);

        if (strncmp(inode->name, headers[i]->filename, strlen(inode->name)) == 0) {
            j++;
        }

        if (num == j) {
            header = headers[i];
            node->entry = i;
            break;
        }
    }

    if (!header) {
        free(node);
        free(dir);
        return 0;
    }

    strcpy(node->name, header->filename);
    node->driver = &tar_driver;

    switch (header->type) {
        case TAR_FILE:
            node->type = FS_FILE;
            break;

        case TAR_DIRECTORY:
            node->type = FS_DIRECTORY;
            break;
    }

    strcpy(dir->name, node->name);
    dir->inode = node;

    return dir;
}
