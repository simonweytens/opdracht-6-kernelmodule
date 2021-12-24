#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static struct task_struct *p1 = NULL;
static struct task_struct *p2 = NULL;

static int del1=500;
static int del2=1000;
//Gpios
static struct gpio leds[] = 
{
		{4, GPIOF_OUT_INIT_HIGH, "LED1" },
		{17, GPIOF_OUT_INIT_HIGH, "LED2" },
};
// Kernel licenties van voorbeeld
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using GPIOs to drive LEDs");

static int leds[2] = { -1, -1 };
static int ledslen = 0;

static int toggle[2] = { 2, 2 };
static int togglelen = 0;

static int states[2] = { 1, 1 };
static int stateslen = 0;



/*
* module_param_array(name, type, num, perm);
* The first param is the parameter's (in this case the array's) name
* The second param is the data type of the elements of the array
*/
module_param_array(leds, int, &ledslen, 0000);
MODULE_PARM_DESC(leds, "An array of integers");
module_param_array(states, int, &stateslen, 0000);
MODULE_PARM_DESC(states, "An array of integers");
module_param_array(toggle, int, &togglelen, 0000);
MODULE_PARM_DESC(toggle, "An array of integers");

static int functionP1(void *data)
{
while(!kthread_should_stop())
{
    gpio_set_value(leds[0].gpio, !gpio_get_value(leds[0].gpio));
    mdelay(del1);
}
return 0;
}

static int functionP2(void *data)
{
while(!kthread_should_stop())
{
    gpio_set_value(leds[1].gpio, !gpio_get_value(leds[1].gpio));
    mdelay(del2);
}
return 0;
}


static int __init gpiomod_init(void)
{
int i;
int req = 0;
printk(KERN_INFO "Hello world\n");
printk(KERN_INFO "%s\n", __func__);
printk(KERN_INFO "--- INPUTS ---\n");

for (i = 0; i < (sizeof leds / sizeof (int)); i++)
{
printk(KERN_INFO "leds[%d] = %d\n", i, leds[i]);
}

printk(KERN_INFO "got %d integers for leds.\n", leds);


for (i = 0; i < (sizeof states / sizeof (int)); i++)
{
printk(KERN_INFO "STATES[%d] = %d\n", i, states[i]);
}

printk(KERN_INFO "got %d integers for states.\n", stateslen);

for (i = 0; i < (sizeof toggle / sizeof (int)); i++)
{
printk(KERN_INFO "toggle[%d] = %d\n", i, toggle[i]);
}

printk(KERN_INFO "got %d integers for toggle.\n", togglelen);

for (i = 0; i < (sizeof leds / sizeof (int)); i++)
{
    leds[i].gpio = leds[i];
}

req = gpio_request_array(leds, ARRAY_SIZE(leds));

if (req) 
{
		printk(KERN_ERR "Unable to request GPIOs: %d\n", req);
}

for(i = 0; i < ARRAY_SIZE(leds); i++) 
{
	
	gpio_set_value(leds[i].gpio,states[i]);
	
}

if(toggle[0] != 0)
{
    del1 = toggle[0]*500;
    p1 = kthread_create(functionP1, NULL, "functionP1");
    if(p1) 
	{
        wake_up_process(p1);
    }
        else 
		{
            printk(KERN_ERR "Cannot create functionP1\n");
        }
}


if(toggle[1] != 0)
{
    del2 = toggel[1]*500;
    p2 = kthread_create(functionP2, NULL, "functionP2");
    if(p2) 
	{
        wake_up_process(p2);
    }
        else 
		{
            printk(KERN_ERR "Cannot create functionP2\n");
        }
}


return req;
/*
* A non 0 return means init_module failed; module can't be loaded.
*/
}

static void __exit gpiomod_exit(void)
{
int i;

printk(KERN_INFO "%s\n", __func__);

if(p1) {kthread_stop(p1);}
if(p2) {kthread_stop(p1);}


for(i = 0; i < ARRAY_SIZE(leds); i++) 
{
		gpio_set_value(leds[i].gpio,0);
}
// unregister all GPIOs
gpio_free_array(leds, ARRAY_SIZE(leds));


printk(KERN_INFO "Goodbye world 1.\n");
}

module_init(gpiomod_init);
module_exit(gpiomod_exit);