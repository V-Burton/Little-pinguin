// SPDX-License-Identifier: GPL-2.0-only
/*
 * foo files
 */

#include "ft_debugfs.h"

static char my_buffer[PAGE_SIZE];
struct mutex foo_mutex;
static size_t current_data_len;

ssize_t foo_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	if (mutex_lock_interruptible(&foo_mutex))
		return -ERESTARTSYS;
	ret = simple_read_from_buffer(buf, count, ppos, my_buffer, current_data_len);
	mutex_unlock(&foo_mutex);
	return ret;
}

ssize_t foo_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	if (mutex_lock_interruptible(&foo_mutex))
		return -ERESTARTSYS;

	ret = simple_write_to_buffer(my_buffer, PAGE_SIZE, ppos, buf, count);
	if (ret > 0) {
		if (*ppos > current_data_len)
			current_data_len = *ppos;
		i_size_write(file_inode(file), current_data_len);
	}
	mutex_unlock(&foo_mutex);
	return ret;
}

int foo_open(struct inode *inode, struct file *file)
{
	if (file->f_flags & O_TRUNC) {
		current_data_len = 0;
		i_size_write(inode, 0);
		memset(my_buffer, 0, PAGE_SIZE);
		pr_info("Truncated write");
	} else if (file->f_flags & O_APPEND) {
		file->f_pos = current_data_len;
		pr_info("Append write");
	}
	i_size_write(inode, current_data_len);
	pr_info("File open");
	return 0;
}

int foo_release(struct inode *inode, struct file *file)
{
	pr_info("File release");
	return 0;
}
