#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krivosheev Andrey");
MODULE_VERSION("0.1");

int networkfs_init(void) {
    printk(KERN_INFO "Hello, World!\n");
    return 0;
}

void networkfs_exit(void) {
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(networkfs_init);
module_exit(networkfs_exit);