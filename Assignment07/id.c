// SPDX-License-Identifier: GPL-2.0-only
/*
 *  functions for id file
 */

#include "ft_debugfs.h"

ssize_t id_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	char *answer = "vburton";
	size_t len = strlen(answer);

	if (*ppos >= len)
		return 0;
	pr_info("Read operation invoked: Login is %s\n", answer);

	if (count > len - *ppos)
		count = len - *ppos;

	if (copy_to_user(buf, answer, count)) {
		pr_info("Failed to copy to user\n");
		return -EFAULT;
	}
	*ppos += count;
	return count;
}

ssize_t id_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
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
