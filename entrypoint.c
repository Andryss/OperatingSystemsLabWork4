#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krivosheev Andrey");
MODULE_DESCRIPTION("Network file system for operating systems lab work 4");
MODULE_VERSION("0.0.1");

static int __init networkfs_init(void) {
    printk(KERN_INFO "Hello, World!\n");
    return 0;
}

static void __exit networkfs_exit(void) {
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(networkfs_init);
module_exit(networkfs_exit);