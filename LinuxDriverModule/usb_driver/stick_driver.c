#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// probe function
// called on device insertion if and only if no other driver has beat us
static int pen_probe(struct usb_interface* interface, const struct usb_device_id* id){
	printk(KERN_INFO "[*] NKuzmic Pen drive (%04X:%04X) plugged in.\n",id->idVendor,id->idProduct);
	return 0; // We will manage this device
}

static void pen_disconnect(struct usb_interface* interface){
	printk(KERN_INFO "[*] NKuzmic Pen drive removed.\n");
	return;
}

static struct usb_device_id pen_table[] = {
	//0781:5406
	{ USB_DEVICE(0x8584, 0x1000) }, // information obtained using "lsusb"
	{} // Terminating entry.
};

MODULE_DEVICE_TABLE(usb,pen_table);

static struct usb_driver pen_driver = {
	.name = "NKuzmic-USB Stick-Driver",
	.id_table = pen_table, // usb_device_id
	.probe = pen_probe,
	.disconnect = pen_disconnect
};

static int __init pen_init(void){
	int ret = 1;
	printk(KERN_INFO "[*] NKuzmic Constructor of driver.\n");
	printk(KERN_INFO "\t Registring driver with Kernel.\n");
	ret = usb_register(&pen_driver);
	printk(KERN_INFO "Registration is now complete.\n");

	return ret;
}

static void __exit pen_exit(void){
	// unregister
	printk(KERN_INFO "[*] NKuzmic destructor of driver.\n");
	usb_deregister(&pen_driver);
	printk(KERN_INFO "\tUnregistration complete!\n");
}

module_init(pen_init);
module_exit(pen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NKuzmic97");
MODULE_DESCRIPTION("USB Pen Registration Driver");