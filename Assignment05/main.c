// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Example kernel module for registering a misc device
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	char *answer = "vburton";
	size_t len = strlen(answer);

	if (*ppos >= len)
		return 0;
	pr_info("Read operation invoked: Login is %s\n", answer);

	if (count > len - *ppos)
		count = len - *ppos;

	if (copy_to_user(buf, answer + *ppos, count)) {
		pr_info("Failed to copy to user\n");
		return -EFAULT;
	}
	*ppos += count;
	return count;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char data[64];

	if (count > 64)
		count = 63;

	if (copy_from_user(data, buf, count)) {
		pr_info("Failed to copy from user\n");
		return -EFAULT;
	}

	data[count] = '\0';

	if (count > 0 && data[count-1] == '\n')
		data[count-1] = '\0';

	if (strcmp(data, "vburton")) {
		pr_info("Unsuccessful login: %s\n", data);
		return -EINVAL;
	}
	pr_info("Successful login: %s\n", data);
	return count;
}

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
};

static struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &my_fops,
};

static int __init my_init(void)
{
	pr_info("Misc_register.\n");
	misc_register(&my_misc_device);

	/*
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Cleaning up module and misc_device.\n");
	misc_deregister(&my_misc_device);
}


module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
