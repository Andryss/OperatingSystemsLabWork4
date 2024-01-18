#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krivosheev Andrey");
MODULE_VERSION("0.1");

struct inode *networkfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, int i_ino) {
    struct inode *inode;
    inode = new_inode(sb);
    inode->i_ino = i_ino;
    if (inode != NULL) {
        inode_init_owner(&init_user_ns, inode, dir, mode);
    }
    return inode;
}

int networkfs_fill_super(struct super_block *sb, void *data, int silent) {
    struct inode *inode;
    inode = networkfs_get_inode(sb, NULL, S_IFDIR, 1000);
    sb->s_root = d_make_root(inode);
    if (sb->s_root == NULL) {
        return -ENOMEM;
    }
    printk(KERN_INFO "return 0\n");
    return 0;
}

struct dentry* networkfs_mount(struct file_system_type *fs_type, int flags, const char *token, void *data) {
    struct dentry *ret;
    ret = mount_nodev(fs_type, flags, data, networkfs_fill_super);
    if (ret == NULL) {
        printk(KERN_ERR "Can't mount file system");
    }
    else {
        printk(KERN_INFO "Mounted successfuly");
    }
    return ret;
}

void networkfs_kill_sb(struct super_block *sb) {
    printk(KERN_INFO "networkfs super block is destroyed. Unmount successfully.\n");
}

struct file_system_type networkfs_fs_type =
        {
                .name = "networkfs",
                .mount = networkfs_mount,
                .kill_sb = networkfs_kill_sb
        };

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
    if (register_filesystem(&networkfs_fs_type) != 0) {
        printk(KERN_ERR "Exit module error\n");
        return;
    }
    printk(KERN_INFO "Exit module successful\n");
}

module_init(networkfs_init);
module_exit(networkfs_exit);