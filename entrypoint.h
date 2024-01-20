#ifndef NETWORK_FILE_SYSTEM_ENTRYPOINT_H
#define NETWORK_FILE_SYSTEM_ENTRYPOINT_H

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "http.h"


int networkfs_init(void);
void networkfs_exit(void);

struct dentry* networkfs_mount(struct file_system_type *, int, const char *, void *);
void networkfs_kill_sb(struct super_block *);

int networkfs_fill_super(struct super_block *, void *, int);
struct inode *networkfs_get_inode(struct super_block *, const struct inode *, umode_t, int);

struct file_system_type networkfs_fs_type =
        {
                .name = "networkfs",
                .mount = networkfs_mount,
                .kill_sb = networkfs_kill_sb
        };


int networkfs_iterate(struct file *, struct dir_context *);

struct file_operations networkfs_dir_ops =
        {
                .iterate = networkfs_iterate,
        };


struct dentry* networkfs_lookup(struct inode *, struct dentry *, unsigned int);

struct inode_operations networkfs_inode_ops =
        {
                .lookup = networkfs_lookup,
        };


// http
struct list_response {
    uint32_t count;
    struct entry {
        char name[64];
        uint32_t ino;
        uint8_t type;
    } entries[8];
};

struct lookup_response {
    uint32_t ino;
    uint8_t type;
};

#endif //NETWORK_FILE_SYSTEM_ENTRYPOINT_H
