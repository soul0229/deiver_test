#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/uaccess.h>
#include <linux/miscdevice.h>

static int cdev_test_open(struct inode *inode,struct file *file){
	printk("This is cdev_test_open\n");
	return 0;
}

static ssize_t cdev_test_read(struct file *file,char __user *buf,size_t size,loff_t *off){
	
	printk("This is cdev_test_read\n");
	return 0;
}

static ssize_t cdev_test_write(struct file *file,const char __user *buf,size_t size,loff_t *off){
	printk("This is cdev_test_write\n");
	return 0;
}

static int cdev_test_release(struct inode *inode,struct file *file){
	printk("This is cdev_test_release\n");
        return 0;
}

static struct file_operations cdev_test_ops={
	.owner=THIS_MODULE,
	.open=cdev_test_open,
	.read=cdev_test_read,
	.write=cdev_test_write,
	.release=cdev_test_release
};

struct miscdevice misc_dev={
	.minor=MISC_DYNAMIC_MINOR,
	.name="test",
	.fops=&cdev_test_ops
};

static int modulecdev_init(void)
{
	int ret;
	
	ret=misc_register(&misc_dev);
	if(ret<0){
		printk("misc_register error\n");
		return -1;
	}

	return 0;
}

static void modulecdev_exit(void){
	misc_deregister(&misc_dev);
	printk("bye bye\n");
}

module_init(modulecdev_init);
module_exit(modulecdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soul");
MODULE_VERSION("V1.0");

