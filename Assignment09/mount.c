// SPDX-License-Identifier: GPL-2.0-only
/*
 * A module that list mount points on our system
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/nsproxy.h>
#include <linux/ns_common.h>
#include <linux/mnt_namespace.h>
#include <../fs/mount.h>
#include <linux/rbtree.h>
#include <linux/seq_file.h>
#include <linux/path.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define PROCFS_NAME "mymounts"

static void *my_mounts_start(struct seq_file *m, loff_t *pos)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct rb_node *n;
	loff_t i = 0;

	n = rb_first(&ns->mounts);

	while (n && i < *pos) {
		n = rb_next(n);
		i++;
	}
	return n;
}

static void *my_mounts_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct rb_node *n = (struct rb_node *)v;

	(*pos)++;

	return rb_next(n);
}

static void my_mounts_stop(struct seq_file *m, void *v)
{
	//Rien a faire ici juste ne pas avoir de pointeur null
}

static int my_mounts_show(struct seq_file *m, void *v)
{
	struct mount *mnt = rb_entry(v, struct mount, mnt_node);
	struct path mnt_path;
	char *path_buf;
	char *resolved_path;

	path_buf = kmalloc(PATH_MAX, GFP_KERNEL);
	if (!path_buf)
		return -ENOMEM;

	mnt_path.mnt = &mnt->mnt;
	mnt_path.dentry = mnt->mnt.mnt_root;
	resolved_path = d_path(&mnt_path, path_buf, PATH_MAX);

	if (!IS_ERR(resolved_path)) {

		if (strcmp(mnt->mnt_devname, "rootfs") != 0) {
			seq_printf(m, "%-15s	%s\n",
				mnt->mnt_devname ? mnt->mnt_devname : "none",
				resolved_path);
		}

	}

	kfree(path_buf);
	return 0;
}

static const struct seq_operations my_mounts_seq_ops = {
	.start = my_mounts_start,
	.next = my_mounts_next,
	.stop = my_mounts_stop,
	.show = my_mounts_show,
};

static int my_mounts_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_mounts_seq_ops);
}

static const struct proc_ops proc_mount_fops = {
	.proc_open = my_mounts_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
};

static struct proc_dir_entry *proc_mount;

static int __init my_init(void)
{
	pr_info("Mount module loaded");
	proc_mount = proc_create(PROCFS_NAME, 0444, NULL, &proc_mount_fops);
	if (proc_mount == NULL) {
		pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME);
		return -ENOMEM;
	}
	return 0;
}

static void __exit my_exit(void)
{
	proc_remove(proc_mount);
	pr_info("Cleaning up mount module");
}

module_init(my_init);
module_exit(my_exit);
