/*
 *  main.c - Example kernel module for registering a misc device
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");

static ssize_t id_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
	char *answer = "vburton";
	size_t len = strlen(answer);

	if (*ppos >= len)
		return 0;
	pr_info("Read operation invoked: Login is %s\n", answer);
	
	if (count > len - *ppos) {
		count = len - *ppos;
	}
	
	if (copy_to_user(buf, answer, count)){
		pr_info("Failed to copy to user\n");
		return -EFAULT;
	}
	*ppos += count;
	return count;
}

static ssize_t id_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
	char data[64];

	if (count > 64)
		count = 63;

	if (copy_from_user(data, buf, count)){
		pr_info("Failed to copy from user\n");
		return -EFAULT;
	}

	data[count] = '\0';

	if (count > 0 && data[count-1] == '\n') {
		data[count-1] = '\0';
	}

	if (strcmp(data, "vburton")){
		pr_info("Unsuccessful login: %s\n", data);
		return -EINVAL;
	}
	pr_info("Successful login: %s\n", data);
	return count;
}

static long int jiffies_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
	char data[32];
	size_t len = sprintf(data, "%lu\n", jiffies);
	/* simple_read_from_buffer gère :
     - La copie vers l'utilisateur (copy_to_user)
     - La mise à jour de l'offset (*ppos)
     - La vérification que l'utilisateur ne demande pas trop de données
	*/
    return simple_read_from_buffer(buf, count, ppos, data, len);
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static const struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffies_read,
};

static struct dentry *my_dir;

static int __init my_init(void)
{
	printk(KERN_INFO "ft_debugfs: Init module.\n");

	my_dir = debugfs_create_dir("fortytwo", NULL);
	if (!my_dir) {
        pr_err("ft_debugfs: Failed to create directory\n");
        return -ENOMEM;
    }

	struct dentry *id;
	struct dentry *jiffies;
	struct dentry *foo;

	/**/
	id = debugfs_create_file("id", 0777, my_dir, NULL, &id_fops);
	jiffies = debugfs_create_file("jiffies", 0444, my_dir, NULL, &jiffies_fops);
	/*
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "Cleaning up debufg directory and files.\n");
	debugfs_remove_recursive(my_dir);
}


module_init(my_init);
module_exit(my_exit);
