/*
 *  Copyright (C) 2009 Atmel Corporation
 *	George.Guo <george.guo@atmel.com>
 *
 * 	Key scan device driver with GPIO lines capable of generating interrupts.
 *  This driver supports to control leds on/off through ioctl.
 *
 * 	This program is free software; you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License version 2 as
 * 	published by the Free Software Foundation; either version 2 of the
 * 	License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <mach/gpio.h>
#include <mach/board.h>
#include <linux/gpio.h>

/*define ioctl command*/
#define KEY_MAGIC 'k'
#define LED_ON_CMD _IO(KEY_MAGIC, 1)
#define LED_OFF_CMD _IO(KEY_MAGIC, 2)
#define GET_KEY_STS_CMD _IO(KEY_MAGIC, 3)

/*define led number*/
#define LEFT_LED  (unsigned long)1
#define RIGHT_LED (unsigned long)2

static int key_major;
EXPORT_SYMBOL(key_major);
module_param(key_major, int, 0);

static int key_values = 0;
static int key_event = 0;
static struct fasync_struct *fasync;
static void button_device_release(struct device *dev);

/* Wait queue:
 * If process is call key_read() function, but no key press opertation,
 * then process goes to sleep until key pressed
 */
static DECLARE_WAIT_QUEUE_HEAD(key_waitq);

/*
 * Usually, we put below section
 * in arch/arm/mach-at91/board_xxxx.c
 */
/*********************************From*********************************/
/*
 * BUTTONs
 */
static struct gpio_keys_button atmel_buttons[] = {
	{			/* BP1, "leftclic" */
	 .code = BTN_LEFT,
	 .gpio = AT91_PIN_PA30,
	 .active_low = 1,
	 .desc = "resert_button",
	 .debounce_interval = 50,	/*ms */
	 },
};

static struct gpio_keys_platform_data atmel_button_data = {
	.buttons = atmel_buttons,
	.nbuttons = ARRAY_SIZE(atmel_buttons),
};

static struct platform_device atmel_button_device = {
	.name = "buttons-leds",
	.id = -1,
	.num_resources = 0,
	.dev = {
		.platform_data 	= &atmel_button_data,
		.release		= button_device_release,
		}
};

/*******************************To***************************************/

struct gpio_button_data {
	struct gpio_keys_button *button;
	struct timer_list timer;
};

struct gpio_keys_drvdata {
	int nbuttons;
	/* interrupt event flag, ISR set it 1, key_read will clear it */
	int read_available_fg;
	spinlock_t lock;
	struct gpio_button_data datas[0];
};

static struct gpio_keys_drvdata *atmel_ddata;

/*
 * LEDs
 */
static struct gpio_led atmel_leds[] = {
	{			/* "left" led, green */
	 .name = "sysled",
	 .gpio = AT91_PIN_PB8,
	 .active_low = 1,
	 },
};

static void at91_led_init(struct gpio_led *leds, int nr)
{
	int i;

	if (!nr)
		return;

	for (i = 0; i < nr; i++)
	{
		at91_set_GPIO_periph(leds[i].gpio, 1);
		at91_set_gpio_output(leds[i].gpio, leds[i].active_low);
	}
}

static inline void at91_led_on(unsigned int led)
{
	at91_set_gpio_value(led, 0);
}

static inline void at91_led_off(unsigned int led)
{
	at91_set_gpio_value(led, 1);
}

static void gpio_check_button(unsigned long _data)
{
	struct gpio_button_data *bdata = (struct gpio_button_data *)_data;
	struct gpio_keys_button *button = bdata->button;
	int key_num;
	int real_pressed;
	unsigned long flags;

	if (BTN_LEFT == button->code)
		key_num = 0;

	if (BTN_RIGHT == button->code)
		key_num = 1;

	real_pressed = (at91_get_gpio_value(button->gpio) ? 1 : 0)
	    ^ button->active_low;
	if (real_pressed)
	{
		if( 0 == key_values )
		{
			key_values = 1;
			key_event = 1;
			if(fasync)
    	        	{
    	        	    kill_fasync(&fasync, SIGIO, POLL_IN);
    	        	}
			mod_timer(&bdata->timer, jiffies + msecs_to_jiffies(3000));
		}
		else if( 1 == key_values )
		{
			key_values = 2;
			key_event = 2;
			if(fasync)
    	        	{
    	        	    kill_fasync(&fasync, SIGIO, POLL_IN);
    	        	}
			mod_timer(&bdata->timer, jiffies + msecs_to_jiffies(5000));
		}
		else
		{
			key_values = 3;
			if( 3 != key_event )
			{
				key_event = 3;
				if(fasync)
	    	        	{
	    	        	    kill_fasync(&fasync, SIGIO, POLL_IN);
	    	        	}
			}			
			mod_timer(&bdata->timer, jiffies + msecs_to_jiffies(20));
		}
	}
	else
	{
		if( 0 != key_values )
		{
			/*
			if( 1 == key_values )
			{
				//key_event = 1;
				printk(KERN_INFO "key pressed: event = resert\n");
			}
			else if( 2 == key_values )
			{
				//key_event = 2;
				printk(KERN_INFO "key pressed: event = restore-default\n");
			}
			else
			{
				//key_event = 3;
				printk(KERN_INFO "key pressed: event = starting-safemode\n");
			}
			*/
			spin_lock_irqsave(&atmel_ddata->lock, flags);
			atmel_ddata->read_available_fg = 1;	/* set flag */
			spin_unlock_irqrestore(&atmel_ddata->lock, flags);
			wake_up_interruptible(&key_waitq);	/* wakeup the process */
		}
		/*
		else
		{
			printk(KERN_INFO "skip a jitter\n");
		}
		*/
		key_values = 0;
		//at91_led_off(atmel_leds[0].gpio);
	}
}

static irqreturn_t gpio_keys_isr(int irq, void *dev_id)
{
	struct gpio_button_data *bdata = dev_id;
	struct gpio_keys_button *button = bdata->button;	

	//at91_led_on(atmel_leds[0].gpio);
	
	/*debounce */
	mod_timer(&bdata->timer, jiffies + msecs_to_jiffies(button->debounce_interval));

	return IRQ_HANDLED;
}

static int hi_tsvr_fasync(int fd, struct file *file, int on)
{
	int retval=0;
	retval = fasync_helper(fd, file, on, &fasync);
	return retval;
}

static int key_open(struct inode *inode, struct file *file)
{
	//return 0;
	return nonseekable_open(inode, file);
}

static int key_close(struct inode *inode, struct file *file)
{
	hi_tsvr_fasync(-1,file,0);
	return 0;
}

static int key_read(struct file *filp, char __user *buff,
		    size_t count, loff_t *offp)
{
	unsigned long err;
	unsigned long flags;

	if (!atmel_ddata->read_available_fg) {
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		else
			/* if read_available_fg is 0, then sleep */
			wait_event_interruptible(key_waitq,
						 atmel_ddata->read_available_fg);
	}

	/* clear read_available_fg */
	spin_lock_irqsave(&atmel_ddata->lock, flags);
	atmel_ddata->read_available_fg = 0;
	spin_unlock_irqrestore(&atmel_ddata->lock, flags);

	/* transfer key status to user space and then clear key_event */
	err = copy_to_user(buff, (const void *)&key_event,
			   min(sizeof(key_event), count));
	memset((void *)&key_event, 0, sizeof(key_event));

	return err ? -EFAULT : min(sizeof(key_event), count);
}

static long key_ioctl(struct file *file,
		     unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	switch (cmd) {
	case LED_ON_CMD:
		if (LEFT_LED == arg)
			at91_led_on(atmel_leds[0].gpio);
		break;
	case LED_OFF_CMD:
		if (LEFT_LED == arg)
			at91_led_off(atmel_leds[0].gpio);
		break;
	case GET_KEY_STS_CMD:
	{
		if(copy_to_user(argp, (const void *)&key_event,  sizeof(key_event)))
				return -EFAULT;
		break;
	}
	default:
		break;
	}
	return 0;
}

/******************************************************************
* When calling select() in user space, this function will be called.
* If key pressed, select() function will return at once.
* If no key pressed, this function will call poll_wait() to waiting.
*******************************************************************/
static unsigned int key_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;

	poll_wait(file, &key_waitq, wait);
	if (atmel_ddata->read_available_fg)
		mask |= POLLIN | POLLRDNORM;
	return mask;
}

static const struct file_operations key_fops = {
	.owner = THIS_MODULE,
	.open = key_open,
	.release = key_close,
	.read = key_read,
	.poll = key_poll,
	.unlocked_ioctl = key_ioctl,
	.fasync =   hi_tsvr_fasync,
};

/*
 * Set up the cdev structure for a device.
 */
static void key_setup_cdev(struct cdev *dev, int minor,
			   const struct file_operations *fops)
{
	int err, devno = MKDEV(key_major, minor);

	cdev_init(dev, fops);
	dev->owner = THIS_MODULE;
	dev->ops = fops;
	err = cdev_add(dev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding key%d", err, minor);
}

/*
 * We export one key device.  There's no need for us to maintain any
 * special housekeeping info, so we just deal with raw cdev.
 */
static struct cdev key_cdev;

static int __init key_scan_probe(struct platform_device *pdev)
{
	int error, i, irq, nbuttons;
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	struct gpio_keys_button *button;
	struct gpio_button_data *bdata;
	dev_t dev;

	key_major = 0;
	dev = MKDEV(key_major, 0);

	atmel_ddata = kzalloc(sizeof(struct gpio_keys_drvdata) +
			      pdata->nbuttons * sizeof(struct gpio_button_data),
			      GFP_KERNEL);

	if (!atmel_ddata) {
		error = -ENOMEM;
		printk(KERN_ERR "No mem for kzalloc for ddata\n");
		goto fail1;
	}

	atmel_ddata->read_available_fg = 0;
	spin_lock_init(&atmel_ddata->lock);
	platform_set_drvdata(pdev, atmel_ddata);
	atmel_ddata->nbuttons = pdata->nbuttons;
	nbuttons = atmel_ddata->nbuttons;
	for (i = 0; i < nbuttons; i++) {
		button = &pdata->buttons[i];
		bdata = &atmel_ddata->datas[i];
		bdata->button = button;

		setup_timer(&bdata->timer,
			    gpio_check_button, (unsigned long)bdata);

		error =
		    gpio_request(button->gpio, button->desc ? : "gpio_keys");
		if (error < 0) {
			printk(KERN_ERR "gpio-keys: failed to request GPIO %d,"
			       " error %d\n", button->gpio, error);
			goto fail2;
		}

		error = gpio_direction_input(button->gpio);
		if (error < 0) {
			printk(KERN_ERR "gpio-keys: failed to configure input"
			       " direction for GPIO %d, error %d\n",
			       button->gpio, error);
			gpio_free(button->gpio);
			goto fail2;
		}

		irq = gpio_to_irq(button->gpio);
		if (irq < 0) {
			error = irq;
			printk(KERN_ERR "gpio-keys: Unable to get irq number"
			       " for GPIO %d, error %d\n", button->gpio, error);
			gpio_free(button->gpio);
			goto fail2;
		}

		error = request_irq(irq, gpio_keys_isr,
				    IRQF_SHARED | IRQF_TRIGGER_RISING |
				    IRQF_TRIGGER_FALLING,
				    button->desc ? button->desc : "gpio_keys",
				    bdata);
		if (error) {
			printk(KERN_ERR
			       "gpio-keys: Unable to claim irq %d; error %d\n",
			       irq, error);
			gpio_free(button->gpio);
			goto fail2;
		}

	}

	return 0;

fail2:
	while (--i >= 0) {
		free_irq(gpio_to_irq(pdata->buttons[i].gpio),
			 &atmel_ddata->datas[i]);
		if (pdata->buttons[i].debounce_interval)
			del_timer_sync(&atmel_ddata->datas[i].timer);
		gpio_free(pdata->buttons[i].gpio);
	}

fail1:
	kfree(atmel_ddata);

	return error;
}

static int __devexit key_scan_remove(struct platform_device *pdev)
{
	int i;
	int irq;
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;

	for (i = 0; i < pdata->nbuttons; i++) {
		irq = gpio_to_irq(pdata->buttons[i].gpio);
		free_irq(irq, &atmel_ddata->datas[i]);
		if (pdata->buttons[i].debounce_interval)
			del_timer_sync(&atmel_ddata->datas[i].timer);
		gpio_free(pdata->buttons[i].gpio);
	}

	return 0;
}

static struct platform_driver atmel_button_driver = {
	.probe = key_scan_probe,
	.remove = __devexit_p(key_scan_remove),
	.driver = {
		   .name = "buttons-leds",
		   .owner = THIS_MODULE,
		   }
};

static void button_device_release(struct device *dev)
{
}

/*
 * Usually, we put below
 * in arch/arm/mach-at91/board_xxxx.c
 */
/*************************From***********************/
static void atmel_add_device_buttons(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(atmel_buttons); i++) {
		at91_set_GPIO_periph(atmel_buttons[i].gpio, 1);
		at91_set_deglitch(atmel_buttons[i].gpio, 1);
	}

	platform_device_register(&atmel_button_device);
}

/*************************To*************************/

static int __init key_scan_init(void)
{
	int result;
	dev_t dev;

	/*
	 * Usually, we put below
	 * in arch/arm/mach-at91/board_xxxx.c
	 */
	/************************From********************/
	atmel_add_device_buttons();
	/*************************To*********************/

	platform_driver_register(&atmel_button_driver);

	/* Figure out our device number. */
	if (key_major)
		result = register_chrdev_region(dev, 1, "at91btns");
	else {
		result = alloc_chrdev_region(&dev, 0, 1, "at91btns");
		key_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "at91btns: unable to get major %d\n", key_major);
		return result;
	}
	if (key_major == 0)
		key_major = result;

	/* Now set up cdev. */
	key_setup_cdev(&key_cdev, 0, &key_fops);

	at91_led_init(atmel_leds, ARRAY_SIZE(atmel_leds));

	printk(KERN_INFO "at91btns device installed, with major number %d\n",
	       key_major);
	printk(KERN_INFO
	       "Create device node using 'mknod /dev/at91btns c major_number 0'\n");

	//at91_led_on(atmel_leds[0].gpio);
	
	return 0;
}
static void __exit key_scan_exit(void)
{
	platform_device_unregister(&atmel_button_device);
	platform_driver_unregister(&atmel_button_driver);

	cdev_del(&key_cdev);
	unregister_chrdev_region(MKDEV(key_major, 0), 1);

	printk(KERN_INFO "at91btns device uninstalled\n");
}

module_init(key_scan_init);
module_exit(key_scan_exit);

MODULE_AUTHOR("George Guo <george.guo@atmel.com>");
MODULE_DESCRIPTION("Atmel At91 driver template");
MODULE_LICENSE("Dual BSD/GPL");
