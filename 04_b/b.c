#include <linux/module.h>
#include <linux/init.h>

extern int add(int a,int b);

static int helloworld_init(void)
{
	int a;
	a = add(1,2);
	printk("a=%d\n",a);
	return 0;
}

static void helloworld_exit(void){
	printk("hello world bye!\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soul");
MODULE_VERSION("V1.0");

