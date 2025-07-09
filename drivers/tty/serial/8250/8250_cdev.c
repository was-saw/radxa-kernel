#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>

#include "8250_cdev.h"
#include "8250_dwlib.h"

#define UART_NAME "rtuart"
#define UART_DEV_MAX 256

static dev_t uart_devt;
static struct class * uart_cdev_class;
static bool uart_cdev_initialized = false;

int uart_cdev_init(void)
{
	int ret;
	uart_cdev_class = class_create(THIS_MODULE, "uart_cdev");
	if (IS_ERR(uart_cdev_class)){
		pr_err("failed to create uart class\n");
		return PTR_ERR(uart_cdev_class);
	}

	ret = alloc_chrdev_region(&uart_devt, 0, UART_DEV_MAX, UART_NAME);
	if (ret < 0) {
		pr_err("uart_cdev: failed to allocate char dev region\n");
		return ret;
	}
    uart_cdev_initialized = true;

	return 0;
}

void uart_cdev_exit(void)
{
	if (uart_cdev_initialized) {
		unregister_chrdev_region(uart_devt, UART_DEV_MAX);
		class_destroy(uart_cdev_class);
		uart_cdev_initialized = false;
	}
}

static int uart_cdev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "uart_cdev_open\n");
	struct uart_8250_port *p;
	p = container_of(inode->i_cdev, struct uart_8250_port, cdev);
	file->private_data = p;
	printk(KERN_INFO "uart_cdev_open: %d\n", p->port.line);
    return 0;
}

static int uart_cdev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "uart_cdev_release\n");
    return 0;
}

static ssize_t uart_cdev_read(struct file *file, char __user *buf,
    size_t count, loff_t *ppos)
{
	printk(KERN_INFO "uart_cdev_read\n");
    return 0;
}

static ssize_t uart_cdev_oob_read(struct file *file, char __user *buf,
	size_t count)
{
	printk(KERN_INFO "uart_cdev_oob_read\n");
    return 0;
}

static ssize_t uart_cdev_write(struct file *file, const char __user *buf,
    size_t count, loff_t *ppos)
{
	printk(KERN_INFO "uart_cdev_write\n");
    return 0;
}

static ssize_t uart_cdev_oob_write(struct file *file, const char __user *buf,
    size_t count)
{
	printk(KERN_INFO "uart_cdev_oob_write\n");
    return 0;
}

static long uart_cdev_ioctl(struct file *file, unsigned int cmd,
    unsigned long arg)
{
	printk(KERN_INFO "uart_cdev_ioctl\n");
    return 0;
}

static long uart_cdev_oob_ioctl(struct file *file, unsigned int cmd,
    unsigned long arg)
{
	printk(KERN_INFO "uart_cdev_oob_ioctl\n");
    return 0;
}

static const struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .open = uart_cdev_open,
    .release = uart_cdev_release,
    .read = uart_cdev_read,
    .oob_read = uart_cdev_oob_read,
    .write = uart_cdev_write,
	.oob_write = uart_cdev_oob_write,
    .unlocked_ioctl = uart_cdev_ioctl,
	.oob_ioctl = uart_cdev_oob_ioctl,
};

int uart_cdev_register(struct uart_8250_port *up)
{
	printk(KERN_INFO "=====================uart_cdev_register\n");
	struct uart_port *p = &up->port;
	int ret;
	dev_t devt = MKDEV(MAJOR(uart_devt), p->line);

	if (!uart_cdev_initialized) {
		pr_err("uart_cdev not initialized\n");
		return -ENODEV;
	}

	cdev_init(&up->cdev, &uart_fops);
	up->cdev.owner = THIS_MODULE;

	// 初始化设备
	device_initialize(&up->self_dev);
	up->self_dev.devt = devt;
	up->self_dev.class = uart_cdev_class;
	up->self_dev.parent = NULL;
	dev_set_name(&up->self_dev, "uart%d", p->line);

	ret = cdev_device_add(&up->cdev, &up->self_dev);
	if (ret) {
		pr_err("uart_cdev_register: failed to add cdev device\n");
		put_device(&up->self_dev);
		return ret;
	}

	printk(KERN_INFO "uart_cdev_register: added UART chardev (%d:%d)\n",
		MAJOR(devt), MINOR(devt));
	return 0;
}
EXPORT_SYMBOL_GPL(uart_cdev_register);

void uart_cdev_unregister(struct uart_8250_port *up)
{
	printk(KERN_INFO "=====================uart_cdev_unregister\n");
	
	// 检查设备是否已注册
	if (device_is_registered(&up->self_dev)) {
		printk(KERN_INFO "uart_cdev_unregister: removing device uart%d\n", up->port.line);
		cdev_device_del(&up->cdev, &up->self_dev);
	} else {
		printk(KERN_INFO "uart_cdev_unregister: device uart%d not registered\n", up->port.line);
	}
	put_device(&up->self_dev);
}
EXPORT_SYMBOL_GPL(uart_cdev_unregister);

MODULE_LICENSE("GPL");
