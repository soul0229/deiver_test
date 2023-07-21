#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

static int major=0;
static int minor=0;

module_param(major,int,S_IRUGO);
module_param(minor,int,S_IRUGO);

dev_t dev_num;

static int chrdev_init(void)
{
	int ret;
	if(major){
		printk("major is %d\n",major);
		printk("minor is %d\n",minor);
		dev_num=MKDEV(major,minor);
		ret=register_chrdev_region(dev_num,1,"chrdev_num");
		if(ret<0){
			printk("register_chrdev_region is error\n");
		}
		printk("register_chrdev_region is ok\n");
	}else{
		ret=alloc_chrdev_region(&dev_num,0,1,"alloc_name");
		if(ret<0){
			printk("alloc_chrdev_region is error\n");
		}
		printk("alloc_chrdev_region is ok\n");
		major=MAJOR(dev_num);
		minor=MINOR(dev_num);
		printk("major is %d\n",major);
		printk("minor is %d\n",minor);
	}
	return 0;
}

static void chrdev_exit(void){
	unregister_chrdev_region(dev_num,1);
	printk("bye bye\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soul");
MODULE_VERSION("V1.0");

