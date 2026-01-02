#include "linux/module.h"
#include "linux/fs.h"
#include "linux/cdev.h"
#include "linux/moduleparam.h"
#include "linux/uaccess.h"
#include "linux/string.h"
#include "linux/ioctl.h"
#include "linux/platform_device.h"
#include "linux/of.h"

#define MY_CH_DRIVER "my_driver"
#define BUFF_LEN 10
#define WR_VALUE _IOW('g', 'a', uint32_t*)
#define RD_VALUE _IOR('g', 'b', uint32_t*)

/*create device file and sys class*/
static dev_t my_dev = 0;
static struct cdev my_cdev;
ssize_t my_read(struct file *my_file, char __user *user_buff, size_t buff_size, loff_t *my_loff);
ssize_t my_write(struct file *my_file, const char __user *user_buff, size_t buff_size, loff_t *my_loff);
int my_open(struct inode *my_inode, struct file *my_file);
int my_release(struct inode *my_inode, struct file *my_file);
long my_unlocked_ioctl(struct file *my_file, unsigned int cmd, unsigned long arg);
long my_compat_ioctl(struct file *my_file, unsigned int cmd, unsigned long arg);

static const struct file_operations my_file_operations = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_unlocked_ioctl,
    .compat_ioctl = my_compat_ioctl
};
static char my_buff[BUFF_LEN] = {0};
static uint32_t my_val = 0;
static struct class *my_class = NULL;

/*passing argument*/
static int param_arr[3] = {0, 0, 0};
static unsigned int arr_num = 0;
static int param_cb = 0;
/* Returns 0, or -errno.  arg is in kp->arg. */
int my_set(const char *val, const struct kernel_param *kp);
/* Returns length written or -errno.  Buffer is 4k (ie. be short!) */
int my_get(char *buffer, const struct kernel_param *kp);
static const struct kernel_param_ops my_kernel_param_ops =
{
    .set = my_set,
    .get = my_get
};

module_param_array(param_arr, int, &arr_num, (S_IRUSR | S_IWUSR));
module_param_cb(param_cb, &my_kernel_param_ops, &param_cb, (S_IRUSR | S_IWUSR));

// static int __init my_gpio_init(void);
// static void __exit my_gpio_end(void);

/* static int __init my_gpio_init(void)
{
    int return_val = 0;
    unsigned int i;

    // Module initialization code
    printk(KERN_INFO "\r\nMY GPIO module begin!!!\r\n");

    return_val = alloc_chrdev_region(&my_dev, 0, 1, "my_device");

    if (0 > return_val)
    {
        printk(KERN_ERR "initialize Major number failed!!!\r\n");
        return return_val;
    }
    else
    {
        printk(KERN_INFO "initialize Major number successfully!!!\r\n");
        printk(KERN_INFO "Major = %d, Minor = %d\r\n", MAJOR(my_dev), MINOR(my_dev));
    }

    cdev_init(&my_cdev, &my_file_operations);
    return_val = cdev_add(&my_cdev, my_dev, 1);

    if (0 > return_val)
    {
        printk(KERN_ERR "Failed to register device files to VFS.\r\n");
        return return_val;
    }
    else
    {
        printk(KERN_INFO "Successful to register device files to VFS.\r\n");
    }

    // Create the struct class for device driver. It will create a structure under /sys/class/.
    my_class = class_create(THIS_MODULE, "my_class");
    return_val = (int)IS_ERR(device_create(my_class, NULL, my_dev, NULL, MY_CH_DRIVER));

    if (0 != return_val)
    {
        pr_err("Can not create device driver!!\r\n");
        return return_val;
    }
    else
    {
        printk(KERN_INFO "Finish creating device driver!!!\r\n\r\n");
    }

    // Read input
    for (i = 0; i < arr_num; i ++)
    {
        printk("param_arr[%d] = %d\r\n", i, param_arr[i]);
    }

    return return_val;
} */

/* static void __exit my_gpio_end(void)
{
    // Module exit code
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
    printk("\r\nMY GPIO module end!!!\r\n");
} */

// module_init(my_gpio_init);
// module_exit(my_gpio_end);

int bbb_timer4_probe(struct platform_device *my_platform_device);
int bbb_timer4_remove(struct platform_device *my_platform_device);

struct hardware_drv_data
{
    void __iomem *addr;
    struct platform_device *pdev;
};

static const struct of_device_id timer4_bbb_id_st[] =
{
    {
        .compatible = "ti,am3358-bbb_timer4"
    },
    {
        // NULL sentinel
    }
};

static struct platform_driver bbb_timer4_driver_st =
{
    .probe = bbb_timer4_probe,
    .remove = bbb_timer4_remove,
    .driver =
    {
        .name = "bbb_timer4",
        .of_match_table = of_match_ptr(timer4_bbb_id_st),
    }
};

int bbb_timer4_probe(struct platform_device *my_platform_device)
{
    int return_val = 0;
    unsigned int i;
    struct hardware_drv_data *hardware_drv_data_st = NULL;
    struct device *my_device = &my_platform_device->dev;
    struct resource *io = NULL;

    // Module initialization code
    printk(KERN_INFO "\r\nModule begin!!!\r\n");

    io = platform_get_resource(my_platform_device, IORESOURCE_MEM, 0);

    if (NULL == io)
    {
        printk("Can't get resource!!!\r\n");
        return -ENODEV;
    }
    else
    {
        printk("Get resource successfully!!!\r\n");
    }

    hardware_drv_data_st = devm_kzalloc(my_device, sizeof(struct hardware_drv_data), GFP_KERNEL);

    if (NULL == hardware_drv_data_st)
    {
        printk("Can't allocate memory!!!\r\n");
        return -ENOMEM;
    }
    else
    {
        printk("Allocate memory successfully address %p!!!\r\n", hardware_drv_data_st);
    }
    
    hardware_drv_data_st->addr = devm_ioremap_resource(my_device, io);

    if (IS_ERR(hardware_drv_data_st->addr))
    {
        printk("Can't get base address!!!\r\n");
        return PTR_ERR((const void *)hardware_drv_data_st->addr);
    }
    else
    {
        printk(KERN_INFO "Base address is %p\r\n", hardware_drv_data_st->addr);
    }

    /*-------------------------------------------------------------------------------------*/

    // Register Major number and Minor number for new devices.
    return_val = alloc_chrdev_region(&my_dev, 0, 1, "my_device");

    if (0 > return_val)
    {
        printk(KERN_ERR "initialize Major number failed!!!\r\n");
        return return_val;
    }
    else
    {
        printk(KERN_INFO "initialize Major number successfully!!!\r\n");
        printk(KERN_INFO "Major = %d, Minor = %d\r\n", MAJOR(my_dev), MINOR(my_dev));
    }

    cdev_init(&my_cdev, &my_file_operations);
    return_val = cdev_add(&my_cdev, my_dev, 1);

    if (0 > return_val)
    {
        printk(KERN_ERR "Failed to register device files to VFS.\r\n");
        return return_val;
    }
    else
    {
        printk(KERN_INFO "Successful to register device files to VFS.\r\n");
    }

    // Create the struct class for device driver. It will create a structure under /sys/class/.
    my_class = class_create(THIS_MODULE, "my_class");

    if (IS_ERR(my_class))
    {
        printk("Can't create class!!!\r\n");
        return PTR_ERR(my_class);
    }
    else
    {
        printk("Create class successfully!!!\r\n");
    }

    return_val = (int)IS_ERR(device_create(my_class, NULL, my_dev, NULL, MY_CH_DRIVER));

    if (0 != return_val)
    {
        pr_err("Can not create device driver!!\r\n");
        return return_val;
    }
    else
    {
        printk(KERN_INFO "Finish creating device driver!!!\r\n\r\n");
    }

    // Read input
    for (i = 0; i < arr_num; i ++)
    {
        printk(KERN_INFO "param_arr[%d] = %d\r\n", i, param_arr[i]);
    }

    return return_val;
}

int bbb_timer4_remove(struct platform_device *my_platform_device)
{
    // Module exit code
    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
    printk("\r\nModule end!!!\r\n");

    return 0;
}

module_platform_driver(bbb_timer4_driver_st);
MODULE_DEVICE_TABLE(of, timer4_bbb_id_st);

ssize_t my_read(struct file *my_file, char __user *user_buff, size_t buff_size, loff_t *my_loff)
{
    if (*my_loff >= sizeof(my_buff))
    {
        *my_loff = 0;
        return 0;
    }
    else
    {
        // do nothing
    }

    printk("Size is %d\r\n", sizeof(my_buff));
    unsigned long bytes_not_copy = copy_to_user((void *)(user_buff + *my_loff), (const void *)(my_buff + *my_loff), (unsigned long)(sizeof(my_buff) - *my_loff));
    *my_loff += (sizeof(my_buff) - bytes_not_copy);
    

    return *my_loff;
}

ssize_t my_write(struct file *my_file, const char __user *user_buff, size_t buff_size, loff_t *my_loff)
{
    ssize_t ret = 0;
    static loff_t prev_off = 0;

    if (buff_size < BUFF_LEN)
    {
        memset((void *)my_buff, 0, sizeof(my_buff));
        ret = copy_from_user((void *)(my_buff + *my_loff), (const void *)(user_buff + *my_loff), (unsigned long)(buff_size - *my_loff));
    }
    else
    {
        printk("Out of range!!!\r\n");
        return -ENOBUFS;
    }

    if (0 != ret)
    {
        printk("Wrote failed, continue!!!\r\n");

        *my_loff = (loff_t)(buff_size - ret);
        ret = (ssize_t)(*my_loff - prev_off);
    }
    else
    {
        printk("Size is %d\r\n", buff_size);
        printk("Wrote successfully %s", my_buff);

        *my_loff = 0;
        ret = (ssize_t)buff_size;
    }

    prev_off = *my_loff;

    return ret;
}

int my_open(struct inode *my_inode, struct file *my_file)
{
    int ret = 0;

    printk("my open\r\n");

    return ret;
}

int my_release(struct inode *my_inode, struct file *my_file)
{
    int ret = 0;

    printk("my release\r\n\r\n");

    return ret;
}

long my_unlocked_ioctl(struct file *my_file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    switch(cmd)
    {
        case WR_VALUE:
        {
            if (0 == copy_from_user((void*)&my_val, (const void*)arg, (unsigned long)sizeof(my_val)))
            {
                printk("Wrote successfully %s", my_buff);
            }
            else
            {
                printk("Wrote failed!!!\r\n");
            }

            break;
        }

        case RD_VALUE:
        {
            if (0 == copy_to_user((void*)arg, (const void*)&my_val, (unsigned long)sizeof(my_val)))
            {
                printk("Read successfully %s", my_buff);
            }
            else
            {
                printk("Read failed!!!\r\n");
            }

            break;
        }
    }

    return ret;
}

long my_compat_ioctl(struct file *my_file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    printk("my compat ioctl\r\n");

    return ret;
}

/* Returns 0, or -errno.  arg is in kp->arg. */
int my_set(const char *val, const struct kernel_param *kp)
{
    int return_val = 0;
    int temp_val = 0;

    return_val = kstrtoint(val, 10, &temp_val);
    if (return_val != 0)
    {
        printk("Can't convert from char to int\r\n");
        return return_val;
    }
    else
    {
        *((int *)kp->arg) = temp_val;
        printk("New value of param_cb is %d\r\n", param_cb);
    }

    return return_val;
}

/* Returns length written or -errno.  Buffer is 4k (ie. be short!) */
int my_get(char *buffer, const struct kernel_param *kp)
{
    int return_val = 0;

    return_val = sprintf(buffer, "%d", param_cb);

    if (0 > return_val)
    {
        printk("Failed to read param_cb\r\n");
        return return_val;
    }
    else
    {
        printk("param_cb is %d\r\n", param_cb);
    }

    return return_val;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dev_97");
