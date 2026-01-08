/*
 *  main.c - The simplest kernel module.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

// La table des IDs : on cible la CLASSE "Keyboard"
static const struct usb_device_id kbd_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			 USB_INTERFACE_SUBCLASS_BOOT,
			 USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{ }
};

/* C'est cette ligne qui permet le chargement automatique */
MODULE_DEVICE_TABLE(usb, kbd_table);

static int __init my_init(void)
{
	printk(KERN_INFO "Hello world!.\n");

	/*
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}


module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
