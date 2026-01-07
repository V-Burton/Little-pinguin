/*  
 *  main.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */

static int __init my_init(void){
	printk(KERN_INFO "Hello world!.\n");

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

static void __exit my_exit(void){
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");