#include "vfs.h"
#include <string.h>
#include <stdlib.h>
#include <core/debug.h>

inode_t vfs_find_root(char** path);
inode_t vfs_namei_mount(const char* path, inode_t root);

vfs_node_t* vfs_root;

void vfs_init() {
    vfs_root = malloc(sizeof(vfs_node_t));
    vfs_root->parent = vfs_root;
    strcpy(vfs_root->name, "/");
}

uint64_t vfs_open(inode_t inode, uint64_t mode) {
    if (inode->driver->open) {
        return inode->driver->open(inode, mode);
    }

    return 0;
}

uint64_t vfs_close(inode_t inode) {
    if (inode->driver->close) {
        return inode->driver->close(inode);
    }

    return 0;
}

uint64_t vfs_read(inode_t inode, void* ptr, uint64_t length, uint64_t offset) {
    if (inode->driver->read) {
        return inode->driver->read(inode, ptr, length, offset);
    }

    return 0;
}

uint64_t vfs_write(inode_t inode, void* ptr, uint64_t length, uint64_t offset) {
    if (inode->driver->write) {
        return inode->driver->write(inode, ptr, length, offset);
    }

    return 0;
}

uint64_t vfs_link(inode_t inode, inode_t parent, const char* name) {
    if (inode->driver->link) {
        return inode->driver->link(inode, parent, name);
    }

    return -1;
}

uint64_t vfs_unlink(inode_t inode, const char* name) {
    if (inode->driver->unlink) {
        return inode->driver->unlink(inode, name);
    }

    return -1;
}

uint64_t vfs_stat(inode_t inode, stat_t* stat) {
    if (inode->driver->stat) {
        return inode->driver->stat(inode, stat);
    }

    return -1;
}

uint64_t vfs_isatty(inode_t inode) {
    if (inode->driver->isatty) {
        return inode->driver->isatty(inode);
    }

    return -1;
}

uint64_t vfs_mkdir(inode_t inode, const char* name) {
    if (inode->driver->mkdir) {
        return inode->driver->mkdir(inode, name);
    }

    return -1;
}

dirent_t* vfs_readdir(inode_t inode, uint64_t num) {
    // Support '.' and '..' special files in directories.
    if (vfs_inode_type(inode) == FS_DIRECTORY) {
        if (num == 0) {
            dirent_t* ret = malloc(sizeof(dirent_t));
            ret->inode = inode;
            strcpy(ret->name, ".");
            return ret;
        } else if (num == 1) {
            dirent_t* ret = malloc(sizeof(dirent_t));
            ret->inode = inode->parent;
            strcpy(ret->name, "..");
            return ret;
        }
    }

    if (inode->driver->readdir) {
        return inode->driver->readdir(inode, num);
    }

    return 0;
}

inode_t vfs_finddir(inode_t inode, const char* name) {
    if (vfs_inode_type(inode) == FS_DIRECTORY) {
        if (!strncmp(name, ".", strlen(name))) {
            return inode;
        } else if (!strncmp(name, "..", strlen(name))) {
            return inode->parent;
        }
    }

    if (inode->driver->finddir) {
        return inode->driver->finddir(inode, name);
    }

    if (inode->driver->readdir) {
        int num = 0;
        dirent_t* de;

        while (1) {
            de = vfs_readdir(inode, num);

            if (!de) {
                return 0;
            }

            if (!strncmp(name, de->name, strlen(name))) {
                break;
            }

            free(de->name);
            free(de);
            num++;
        }

        inode_t ret = de->inode;
        free(de->name);
        free(de);

        return ret;
    }

    return 0;
}

inode_t vfs_find_root(char** path) {
    inode_t current = vfs_root;
    inode_t mount = current;

    char* name;

    while ((name = strsep(path, "/"))) {
        DEBUG("current->name=%s name=%s", current->name, name);
        current = current->child;

        while (current) {
            if (strncmp(current->name, name, strlen(current->name)) == 0) {
                mount = current;
                break;
            }

            current = current->older;
        }

        if (!current) {
            if (*path) {
                *path = *path - 1;
                *path[0] = '/';
            }

            *path = name;
            break;
        }
    }

    DEBUG("found root=%s", mount->name);

    return (inode_t)mount;
}

inode_t vfs_namei_mount(const char* path, inode_t root) {
    char* npath = strdup(path);
    char* pth = &npath[1];

    inode_t current = vfs_find_root(&pth);

    char* name;

    while (current && (name = strsep(&pth, "/")) && *name) {
        inode_t next = vfs_finddir(current, name);

        DEBUG("name=%s next=%s", name, next->name);

        if (!next) {
            DEBUG("%s", "no next, returning 0");
            vfs_free(current);
            return 0;
        }

        if (root) {
            next->parent = current;
            next->older = current->child;
            current->child = next;
        }

        vfs_free(current);
        current = next;
    }

    free(npath);

    if (root != 0) {
        root->parent = current->parent;
        current->parent = 0;

        if (root->parent->child == current) {
            root->parent->child = root;
        }

        root->older = current->older;

        if (root->older) {
            root->older->younger = current;
        }

        root->younger = current->younger;

        if (root->younger) {
            root->younger->older = current;
        }

        strcpy(root->name, current->name);
        root->type |= FS_MOUNTPOINT;

        if (current == vfs_root) {
            vfs_root = root;
        }

        vfs_free(current);
        current = root;
    }

    DEBUG("returning current=%s", current->name);

    return current;
}

inode_t vfs_mount(const char* path, inode_t root) {
    DEBUG("mounting path=%s root_fs=%s", path, root->name);
    inode_t node = vfs_namei_mount(path, root);
    DEBUG("mounted path=%s", path);
    return node;
}

inode_t vfs_namei(const char* path) {
    DEBUG("finding path=%s", path);
    return vfs_namei_mount(path, 0);
}

inode_t vfs_umount(const char* path) {
    DEBUG("umounting path=%s", path);
    // TODO: implement me
    return 0;
}

void vfs_free(inode_t inode) {
    if (inode->parent != 0) {
        return;
    }

    free(inode);
}

int vfs_inode_type(inode_t inode) {
    return inode->type & FS_MASK;
}
