#ifndef FT_DEBUGFS_H
#define FT_DEBUGFS_H

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/mm.h>

extern struct mutex foo_mutex;

void cleanup_foo_ressources(void);

/* id */
ssize_t id_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t id_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

/* Jiffies */
long jiffies_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);

/* foo */
ssize_t foo_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t foo_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
int foo_open(struct inode *inode, struct file *file);
int foo_release(struct inode *inode, struct file *file);

#endif
