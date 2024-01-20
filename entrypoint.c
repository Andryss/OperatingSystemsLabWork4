#include "entrypoint.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krivosheev Andrey");
MODULE_VERSION("0.1");

struct dentry* networkfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flag) {
    ino_t root;
    struct inode *inode;
    const char *name = child_dentry->d_name.name;
    root = parent_inode->i_ino;
    printk(KERN_INFO "Lookup in %d for %s\n", (int) root, name);
    if (root == 100 && !strcmp(name, "test.txt")) {
        inode = networkfs_get_inode(parent_inode->i_sb, NULL, S_IFREG, 101);
        d_add(child_dentry, inode);
    }
    return NULL;
}

int networkfs_iterate(struct file *filp, struct dir_context *ctx) {
    char fsname[10];
    struct dentry *dentry;
    struct inode *inode;
    unsigned long offset;
    int stored;
    unsigned char ftype;
    ino_t ino;
    ino_t dino;
    dentry = filp->f_path.dentry;
    inode = dentry->d_inode;
    offset = filp->f_pos;
    stored = 0;
    ino = inode->i_ino;

    printk(KERN_INFO "Iterate through %d\n", (int) ino);

    dir_emit(ctx, ".", 1, ino, DT_DIR);
    ctx->pos += 1;
    dir_emit(ctx, "..", 2, dentry->d_parent->d_inode->i_ino, DT_DIR);
    ctx->pos += 1;
    dir_emit(ctx, "test.txt", 8, 101, DT_REG);
    ctx->pos += 1;

    return 0;
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

struct dentry* networkfs_mount(struct file_system_type *fs_type, int flags, const char *token, void *data) {
    struct dentry *ret;
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