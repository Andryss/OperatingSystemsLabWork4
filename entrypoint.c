#include "entrypoint.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krivosheev Andrey");
MODULE_VERSION("0.1");

char token[32];

struct dentry* networkfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flag) {
    ino_t root;
    struct inode *inode;
    const char *name = child_dentry->d_name.name;
    root = parent_inode->i_ino;

    printk(KERN_INFO "Lookup in %d for %s\n", (int) root, name);

    struct lookup_response response;
    char inode_str[11];
    snprintf(inode_str, sizeof(inode_str), "%lu", root);
    if (networkfs_http_call(token, "lookup", &response, sizeof(response),
                            2, "parent", inode_str, "name", name) != 0) {
        return -1;
    }

    printk(KERN_INFO "Found inode %lu with type %d\n", response.ino, (int) response.type);

    umode_t type = (response.type == DT_REG ? S_IFREG : S_IFDIR);
    inode = networkfs_get_inode(parent_inode->i_sb, NULL, type, response.ino);
    d_add(child_dentry, inode);
    return NULL;
}

int networkfs_iterate(struct file *filp, struct dir_context *ctx) {
    struct dentry *dentry;
    struct inode *inode;
    unsigned long offset;
    ino_t ino;
    dentry = filp->f_path.dentry;
    inode = dentry->d_inode;
    offset = filp->f_pos;
    ino = inode->i_ino;

    printk(KERN_INFO "Iterate through %lu offset %lu\n", ino, offset);

    struct list_response response;
    char inode_str[11];
    snprintf(inode_str, sizeof(inode_str), "%lu", ino);
    if (networkfs_http_call(token, "list", &response, sizeof(response),
                            1, "inode", inode_str) != 0) {
        return -1;
    }

    if (offset >= response.count + 2) {
        return 0;
    }

    dir_emit(ctx, ".", 1, ino, DT_DIR);
    printk(KERN_INFO "Dir emit \".\" pos %lld\n", ctx->pos);
    ctx->pos += 1;
    dir_emit(ctx, "..", 2, dentry->d_parent->d_inode->i_ino, DT_DIR);
    printk(KERN_INFO "Dir emit \"..\" pos %lld\n", ctx->pos);
    ctx->pos += 1;

    printk(KERN_INFO "Found %d entries\n", response.count);

    for (int16_t i = 0; i < response.count; i++) {
        char *name = response.entries[i].name;
        dir_emit(ctx, name, strlen(name), (ino_t) response.entries[i].ino, response.entries[i].type);
        printk(KERN_INFO "Dir emit \"%s\" pos %lld\n", name, ctx->pos);
        ctx->pos += 1;
    }

    return response.count;
}

struct inode *networkfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, int i_ino) {
    struct inode *inode;
    inode = new_inode(sb);
    inode->i_ino = i_ino;
    inode->i_op = &networkfs_inode_ops;
    inode->i_fop = &networkfs_dir_ops;
    mode |= S_IRWXU | S_IRWXG | S_IRWXO;
    printk(KERN_INFO "Get inode of %d\n", i_ino);
    if (inode != NULL) {
        inode_init_owner(&init_user_ns, inode, dir, mode);
    }
    return inode;
}

int networkfs_fill_super(struct super_block *sb, void *data, int silent) {
    struct inode *inode;
    inode = networkfs_get_inode(sb, NULL, S_IFDIR, 100);
    sb->s_root = d_make_root(inode);
    if (sb->s_root == NULL) {
        return -ENOMEM;
    }
    printk(KERN_INFO "Root initialized\n");
    return 0;
}

struct dentry* networkfs_mount(struct file_system_type *fs_type, int flags, const char *t, void *data) {
    struct dentry *ret;
    strcpy(token, t);
    ret = mount_nodev(fs_type, flags, data, networkfs_fill_super);
    if (ret == NULL) {
        printk(KERN_ERR "Can't mount file system");
    } else {
        printk(KERN_INFO "Mounted successfuly");
    }
    return ret;
}

void networkfs_kill_sb(struct super_block *sb) {
    printk(KERN_INFO "networkfs super block is destroyed. Unmount successfully.\n");
}

int networkfs_init(void) {
    printk(KERN_INFO "Init module start\n");
    if (register_filesystem(&networkfs_fs_type) != 0) {
        printk(KERN_ERR "Init module error\n");
        return 1;
    }
    printk(KERN_INFO "Init module successful\n");
    return 0;
}

void networkfs_exit(void) {
    printk(KERN_INFO "Exit module start\n");
    if (unregister_filesystem(&networkfs_fs_type) != 0) {
        printk(KERN_ERR "Exit module error\n");
        return;
    }
    printk(KERN_INFO "Exit module successful\n");
}

module_init(networkfs_init);
module_exit(networkfs_exit);