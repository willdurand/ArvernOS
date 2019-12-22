#include "tar.h"
#include <core/debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

uint64_t get_size(const char* in);
uint64_t tar_read_headers(uint64_t address);
uint64_t tar_read(inode_t node, void* buffer, uint64_t size, uint64_t offset);
inode_t tar_finddir(inode_t inode, const char* name);
dirent_t* tar_readdir(inode_t inode, uint64_t num);
bool starts_with(const char* s, const char* prefix);

tar_header_t* headers[32];

vfs_driver_t tar_driver = {
    0, // open
    0, // close
    tar_read, // read
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

        if (header->name[0] == '\0') {
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

uint64_t tar_read(inode_t node, void* buffer, uint64_t size, uint64_t offset) {
    DEBUG("name=%s type=%d", node->name, node->type);

    if (node->type == FS_FILE) {
        tar_header_t* header = headers[node->entry];
        uint64_t size = get_size(header->size);
        DEBUG("reading file size=%u", size);

        memcpy(buffer, (void*)header + 512, size);
        return size;
    }

    // TODO: add support for other types like symlinks (at least)

    return 0;
}

// FIXME: This implementation does not really work well yet...
inode_t tar_finddir(inode_t inode, const char* name) {
    DEBUG("finding name=%s", name);
    inode_t node = malloc(sizeof(vfs_node_t));

    tar_header_t* header = 0;

    for (uint64_t i = 0; headers[i] != 0; i++) {
        char* name = malloc(strlen(headers[i]->name) * sizeof(char));
        strcpy(name, headers[i]->name);

        DEBUG("reading name=%s name=%s", name, name);

        if (headers[i]->type == TAR_DIRECTORY) {
            name[strlen(name) - 1] = '\0';
            DEBUG("updated name=%s", name);
        }

        if (strncmp(name, name, strlen(name)) == 0) {
            header = headers[i];
            node->entry = i;
            free(name);
            break;
        }

        free(name);
    }

    if (!header) {
        DEBUG("did not find name=%s", name);
        free(node);
        return 0;
    }

    strcpy(node->name, header->name);
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

// FIXME: This implementation does not really work well yet...
dirent_t* tar_readdir(inode_t inode, uint64_t num) {
    dirent_t* dir = malloc(sizeof(dirent_t));
    inode_t node = malloc(sizeof(vfs_node_t));

    DEBUG("inode name=%s", inode->name);

    char* prefix = malloc(strlen(inode->name) * sizeof(char));

    if (strncmp(inode->name, "/", strlen(inode->name)) == 0) {
        prefix[0] = '\0';
    } else {
        strcpy(prefix, inode->name);
    }

    DEBUG("prefix=%s", prefix);

    tar_header_t* header = 0;

    uint64_t j = 0;

    for (uint64_t i = 0; headers[i] != 0; i++) {
        DEBUG("reading name=%s", headers[i]->name);

        if (starts_with(headers[i]->name, prefix)) {
            j++;
        }

        if (num == j) {
            header = headers[i];
            node->entry = i;
            break;
        }
    }

    free(prefix);

    if (!header) {
        DEBUG("%s", "no header found");

        free(node);
        free(dir);
        return 0;
    }

    strcpy(node->name, header->name);
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

bool starts_with(const char* s, const char* prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0 ? true : false;
}
