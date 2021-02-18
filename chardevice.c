#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include "chardevice.h"

static struct cd_dev chardevice;

static int device_minor = 0, device_major = 0;
module_param(device_minor, int, S_IRUGO);

// doesn't handle race condition...
ssize_t cd_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
        ssize_t retval = 0;

        if (*f_pos + count > SIZE)
                count = SIZE - *f_pos;

        if (copy_to_user(buf, &chardevice.buf + *f_pos, count))
        {
                retval = -EFAULT;
                goto out;
        }

        *f_pos += (retval = count);

out:
        return retval;
}

ssize_t cd_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
        ssize_t retval = -ENOMEM;

        if (*f_pos + count > SIZE)
                count = SIZE - *f_pos;

        if (copy_from_user(&chardevice.buf + *f_pos, buf, count))
        {
                retval = -EFAULT;
                goto out;
        }

        *f_pos += (retval = count);

out:
        return retval;
}

int cd_open(struct inode *inode, struct file *filp)
{
        return 0;
}

int cd_release(struct inode *inode, struct file *filp)
{
        return 0;
}

static struct file_operations cd_fops = {
    .owner = THIS_MODULE,
    .read = cd_read,
    .write = cd_write,
    .open = cd_open,
    .release = cd_release,
};

static void char_device_exit(void)
{
        dev_t dev = MKDEV(device_major, device_minor);
        cdev_del(&chardevice.cdev);

        unregister_chrdev_region(dev, 1);
        printk(KERN_ALERT "Chardevice has been unloaded \n");
}

static int __init char_device_init(void)
{
        int status;
        dev_t dev;

        status = alloc_chrdev_region(&dev, device_minor, 1, "chardevice");
        if (status < 0)
                goto error;
        device_major = MAJOR(dev);

        cdev_init(&chardevice.cdev, &cd_fops);
        chardevice.cdev.owner = THIS_MODULE;
        chardevice.cdev.ops = &cd_fops;
        status = cdev_add(&chardevice.cdev, dev, 1);
        if (status)
                goto error;

        printk(KERN_WARNING "Chardevice has been loaded into the kernel\n");
        return 0;

error:
        printk(KERN_WARNING "Failed to load char-device\n");
        char_device_exit();
        return status;
}

module_init(char_device_init);
module_exit(char_device_exit);

MODULE_AUTHOR("honzuki");
MODULE_DESCRIPTION("Simple Character Device");
MODULE_LICENSE("Dual BSD/GPL");
