// SPDX-License-Identifier: GPL-2.0-only
/*
 * Jiffies file
 */

#include "ft_debugfs.h"

long jiffies_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	char data[32];
	size_t len = sprintf(data, "%lu\n", jiffies);

	return simple_read_from_buffer(buf, count, ppos, data, len);
}
