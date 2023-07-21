#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/uaccess.h>

struct device_test{
	dev_t dev_num;
	int major;
	int minor;
	struct cdev cdev_test;
	struct class *class;
	struct device *device;
	char kbuf[32];
};

struct device_test dev1;

static int cdev_test_open(struct inode *inode,struct file *file){
	printk("This is cdev_test_open\n");
	file->private_data=&dev1;
	return 0;
}

static ssize_t cdev_test_read(struct file *file,char __user *buf,size_t size,loff_t *off){
	
	struct device_test *test_dev=(struct device_test *)file->private_data;
	
	if(copy_to_user(buf,test_dev->kbuf,strlen(test_dev->kbuf))!=0){
		printk("copy_to_user error!\n");
        	return -1;
	}

	printk("This is cdev_test_read\n");
	return 0;
}

static ssize_t cdev_test_write(struct file *file,const char __user *buf,size_t size,loff_t *off){
	struct device_test *test_dev=(struct device_test *)file->private_data;
	if(copy_from_user(test_dev->kbuf,buf,size)!=0){
                printk("copy_to_user error!\n");
                return -1;
        }
	printk("kbuf is %s\n",test_dev->kbuf);
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

static int modulecdev_init(void)
{
	int ret;
	ret=alloc_chrdev_region(&dev1.dev_num,0,1,"alloc_name");
	if(ret<0){
		printk("alloc_chrdev_region is error\n");
	}
	printk("alloc_chrdev_region is ok\n");
	
	dev1.major=MAJOR(dev1.dev_num);
	dev1.minor=MINOR(dev1.dev_num);
	printk("major is %d\n",dev1.major);
	printk("minor is %d\n",dev1.minor);

	dev1.cdev_test.owner=THIS_MODULE;
	cdev_init(&dev1.cdev_test,&cdev_test_ops);
	cdev_add(&dev1.cdev_test,dev1.dev_num,1);

	dev1.class=class_create(THIS_MODULE,"test");
	dev1.device=device_create(dev1.class,NULL,dev1.dev_num,NULL,"test");

	return 0;
}

static void modulecdev_exit(void){
	unregister_chrdev_region(dev1.dev_num,1);
	cdev_del(&dev1.cdev_test);
	device_destroy(dev1.class,dev1.dev_num);
	class_destroy(dev1.class);

	printk("bye bye\n");
}

module_init(modulecdev_init);
module_exit(modulecdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soul");
MODULE_VERSION("V1.0");

