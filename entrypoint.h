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
ssize_t networkfs_read(struct file *, char *, size_t, loff_t *);
ssize_t networkfs_write(struct file *, const char *, size_t, loff_t *);

struct file_operations networkfs_dir_ops =
        {
                .iterate = networkfs_iterate,
                .read = networkfs_read,
                .write = networkfs_write
        };


struct dentry* networkfs_lookup(struct inode *, struct dentry *, unsigned int);
int networkfs_create(struct user_namespace *, struct inode *, struct dentry *, umode_t, bool);
int networkfs_unlink(struct inode *, struct dentry *);
int networkgs_mkdir(struct user_namespace *, struct inode *, struct dentry *, umode_t);
int networkgs_rmdir(struct inode *,struct dentry *);

struct inode_operations networkfs_inode_ops =
        {
                .lookup = networkfs_lookup,
                .create = networkfs_create,
                .unlink = networkfs_unlink,
                .mkdir = networkgs_mkdir,
                .rmdir = networkgs_rmdir
        };


// http
struct __attribute__((__packed__)) list_response {
    uint32_t count;
    struct __attribute__((__packed__)) entry {
        char name[256];
        uint32_t ino;
        uint8_t type;
    } entries[8];
};

struct __attribute__((__packed__)) lookup_response {
    uint32_t ino;
    uint8_t type;
};

struct __attribute__((__packed__)) create_response {
    uint32_t ino;
};

struct __attribute__((__packed__)) remove_response {
};

struct __attribute__((__packed__)) read_response {
    uint32_t size;
    char content[1024];
};

struct __attribute__((__packed__)) write_response {
};

#endif //NETWORK_FILE_SYSTEM_ENTRYPOINT_H
