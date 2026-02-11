// SPDX-License-Identifier: GPL-2.0-only
/*
 *  ft_debugfs
 */
#include "ft_debugfs.h"

MODULE_LICENSE("GPL");

static struct dentry *my_dir;

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static const struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffies_read,
};

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
	.open = foo_open,
	.release = foo_release,
};

static int __init my_init(void)
{
	struct dentry *id;
	struct dentry *jiffies;
	struct dentry *foo;

	pr_info("ft_debugfs: Init module.\n");

	my_dir = debugfs_create_dir("fortytwo", NULL);
	if (IS_ERR_OR_NULL(my_dir)) {
		pr_err("ft_debugfs: Failed to create directory\n");
		return -ENOMEM;
	}

	mutex_init(&foo_mutex);

	id = debugfs_create_file("id", 0666, my_dir, NULL, &id_fops);
	jiffies = debugfs_create_file("jiffies", 0444, my_dir, NULL, &jiffies_fops);
	foo = debugfs_create_file("foo", 0644, my_dir, NULL, &foo_fops);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Cleaning up debufg directory and files.\n");
	debugfs_remove_recursive(my_dir);
}


module_init(my_init);
module_exit(my_exit);
