#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/io.h>

#define PC_CFG1 	0x0300B04C
#define PC_DAT		0x0300B058
#define PC_PULL0  	0x0300B064


struct device_test{
	dev_t dev_num;
	int major;
	int minor;
	struct cdev cdev_test;
	struct class *class;
	struct device *device;
	char kbuf[32];
	unsigned int *vir_pc_dat;
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

	if(test_dev->kbuf[0]==1){
		*(test_dev->vir_pc_dat)|=0x00001000;
	}else if(test_dev->kbuf[0]==0){
		*(test_dev->vir_pc_dat)&=~0x00001000;
	}

	printk("test_dev->kbuf[0] is %d\n",test_dev->kbuf[0]);
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
		goto err_chrdev;
	}
	printk("alloc_chrdev_region is ok\n");
	
	dev1.major=MAJOR(dev1.dev_num);
	dev1.minor=MINOR(dev1.dev_num);
	printk("major is %d\n",dev1.major);
	printk("minor is %d\n",dev1.minor);

	dev1.cdev_test.owner=THIS_MODULE;
	cdev_init(&dev1.cdev_test,&cdev_test_ops);
	ret=cdev_add(&dev1.cdev_test,dev1.dev_num,1);
	if(ret<0){
		goto err_chr_add;
	}

	dev1.class=class_create(THIS_MODULE,"test");
	if(IS_ERR(dev1.class)){
		ret=PTR_ERR(dev1.class);
		goto err_class_create;
	}

	dev1.device=device_create(dev1.class,NULL,dev1.dev_num,NULL,"test");
	if(IS_ERR(dev1.device)){
                ret=PTR_ERR(dev1.device);
                goto err_device_create;
        }

	dev1.vir_pc_dat=ioremap(PC_DAT,4);
	if(IS_ERR(dev1.vir_pc_dat)){
                ret=PTR_ERR(dev1.vir_pc_dat);
                goto err_ioremap;
        }

	return 0;

err_ioremap:
	iounmap(dev1.vir_pc_dat);

err_device_create:
	class_destroy(dev1.class);

err_class_create:
	cdev_del(&dev1.cdev_test);

err_chr_add:
	unregister_chrdev_region(dev1.dev_num,1);

err_chrdev:
	return ret;
}

static void modulecdev_exit(void){
	unregister_chrdev_region(dev1.dev_num,1);
	cdev_del(&dev1.cdev_test);
	device_destroy(dev1.class,dev1.dev_num);
	class_destroy(dev1.class);
	
	iounmap(dev1.vir_pc_dat);

	printk("bye bye\n");
}

module_init(modulecdev_init);
module_exit(modulecdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soul");
MODULE_VERSION("V1.0");

