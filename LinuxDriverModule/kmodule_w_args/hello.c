#include <linux/init.h>
#include <linux/module.h>
// Step 1
#include <linux/moduleparam.h>

// Step 2 - Create variable
int arg_var[3] = {0,0,0};

// Step 3 - Register
// module_param(name_var,type,permissions)

module_param_array(arg_var,int,NULL,S_IRUSR | S_IWUSR);

void display(void){
        printk(KERN_ALERT "Test: arg0 = %d\n",arg_var[0]);
        printk(KERN_ALERT "Test: arg1 = %d\n",arg_var[1]);
        printk(KERN_ALERT "Test: arg2 = %d\n",arg_var[2]);
}

static int hello_init(void){
        // register functionalities and allocate resources
        printk(KERN_ALERT "Test: Hello, this is nkuzmic kmodule.\n");
        display();
        return 0;
}

static void hello_exit(void){
        // unregister functionalities and deallocate resources
        printk(KERN_ALERT "Test: Goodbye, uninstalling.");
}

module_init(hello_init);
module_exit(hello_exit);

// Following macros are descripting our kernel module.

MODULE_AUTHOR("nkuzmic97");
MODULE_DESCRIPTION("Building a simple kmodule to accept args.");
MODULE_LICENSE("GPL");