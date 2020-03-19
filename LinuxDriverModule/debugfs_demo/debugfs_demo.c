#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>

MODULE_DESCRIPTION("Demo for Debugfs");
MODULE_AUTHOR("Nemanja Kuzmic");
MODULE_LICENSE("GPL");

struct semaphore my_sem;
char kbuffer[] = "This is kernel data which can be modified.";
struct dentry* my_dirent;

static ssize_t debugfs_read(struct file* file, char* buf, size_t lbuf, loff_t* ppos){
    int status = -1;

    if(down_interruptible(&my_sem) == 0){
        status = simple_read_from_buffer(buf,strlen(kbuffer),ppos,kbuffer,strlen(kbuffer));
        up(&my_sem);
    }

    return status;
}

static ssize_t debugfs_write(struct file* file, const char* buf, size_t lbuf, loff_t* ppos){
    int status = -1;
    
    if(lbuf > strlen(kbuffer)){
        pr_info("Write: returning EINVAL\n");
    }

    if(down_interruptible(&my_sem) == 0){
        status = simple_write_to_buffer(kbuffer,strlen(buf),ppos,buf,lbuf);
        kbuffer[lbuf] = '\0';
        up(&my_sem);
    }

    return status;
}

const struct file_operations fops_debugfs = {
    .read = debugfs_read,
    .write = debugfs_write,

};

static int __init init_debugfs(void){
    sema_init(&my_sem,1);

    my_dirent = debugfs_create_dir("nkuzmic97",NULL);
    debugfs_create_file("demo_file",0666,my_dirent,(void*) kbuffer, &fops_debugfs);

    return 0;
}

static void __exit exit_debugfs(void){
    pr_info("Kernel buffer during exit is: <%s>\n",kbuffer);
    debugfs_remove_recursive(my_dirent);
}

module_init(init_debugfs);
module_exit(exit_debugfs);