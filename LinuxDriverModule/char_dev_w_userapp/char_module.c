#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>        // file_operations structure - open/close, r/w to the device
#include <linux/cdev.h>      // char driver
#include <linux/semaphore.h> // synchronization
#include <linux/uaccess.h>

// Create a structure for our fake device

struct fake_device {
	char data[100];
	struct semaphore sem;
} virtual_device;

// To register our device we need a cdev object and other vars.

struct cdev *my_cdev;
int major_number; // used to store our major number - extracted from dev_t using macro MAJOR()
int ret; 	  // will be used to hold return values of functions - kernel stack is very small
dev_t dev_num;    // holds major number that kernel gives us

#define DEVICE_NAME "nkuzmic_device"
		    // appears in /proc/devices

// Step 7 - Called on device_file open
//	inode reference to the file on disk
//	and contains information about that file
// 	struct file represents an abstract open file

int device_open(struct inode* inode, struct file* filp){
	// only allow one process to open this device by using a semaphore
	// as mutual exclusive lock - MUTEX

	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "nkuzmic_device: Could not lock device during open!\n");
		return -1;
	}

	printk(KERN_INFO "nkuzmic_device: Opened device!\n");
	return 0;
}

// Step 8 - Called when user wants to get information from the device
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset){
	// take data from kernel space(device) to user space(process)
	// copy_to_user(dest,src,sizeToTransfer)

	printk(KERN_INFO "nkuzmic_device: Reading from device..\n");
	ret = copy_to_user(bufStoreData, virtual_device.data,bufCount);
	return ret;
}


// Step 9 - Called when user wants to send information to the deivce
ssize_t device_write(struct file* filp,const char* bufSourceData, size_t bufCount, loff_t* curOffset){
        // send data from user to kernle
        // copy_from_user(dest,src,count)

        printk(KERN_INFO "nkuzmic_device: Writing to device..\n");
        ret = copy_from_user(virtual_device.data,bufSourceData,bufCount);
        return ret;
}

// Step 10 - Called upon user close
int device_close(struct inode* inode, struct file* filp){
	// by calling up, which is opposite of down for semaphore
	// we release the mutex that we obtained at device open

	up(&virtual_device.sem);
	printk(KERN_INFO "nkuzmic_device: Closed device!\n");
	return 0;
}


// Step 6 - Tell the kernel which functions to call when user operates on our device file
struct file_operations fops = {
	.owner = THIS_MODULE, // prevent unloading of this module where operations are queued
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read
};

static int driver_entry(void){
	// Register our device with the system - two steps:

	// Step 1 - Use dynamic allocation to assign our device
	// a major number -- alloc_chrdev_region(dev_t*, uint fminor, uint count, char* name)

	ret = alloc_chrdev_region(&dev_num, 0, 1,DEVICE_NAME);

	if(ret < 0) { // signals an error from kernel
		printk(KERN_ALERT "nkuzmic_device: Failed to allocate a major number!\n");
		return ret; // propagate
	}

	major_number = MAJOR(dev_num); // extracts major number
	printk(KERN_INFO "nkuzmic_device: Major number is %d.\n",major_number);
	printk(KERN_INFO "\t use \"mknod /dev/%s c %d 0\" for device file.\n",DEVICE_NAME,major_number);

	// Step 2

	my_cdev = cdev_alloc();
	my_cdev->ops = &fops;
	my_cdev->owner = THIS_MODULE;
	// after we created cdev, we add it to the kernel
	// int cdev_add(struct cdev* dev, dev_t num, unsigned int count);

	ret = cdev_add(my_cdev,dev_num,1);

	if(ret < 0){
		printk(KERN_ALERT "nkuzmic_device: Unable to add cdev to kernel!\n");
		return ret;
	}

	// Step 3 - Initialize our semaphore

	sema_init(&virtual_device.sem,1); // init value of 1

	return 0;
}

static void driver_exit(void){
	// Step 5 - unregister everything in reverse order

	cdev_del(my_cdev);
	unregister_chrdev_region(dev_num,1);
	printk(KERN_ALERT "nkuzmic_device: Unloaded module!\n");
}

// Inform the kernel where to start and stop with our driver
module_init(driver_entry);
module_exit(driver_exit);
