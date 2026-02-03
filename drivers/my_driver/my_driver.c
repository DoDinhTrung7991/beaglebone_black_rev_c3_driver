#include "linux/module.h"
#include "linux/fs.h"
#include "linux/cdev.h"
#include "linux/moduleparam.h"
#include "linux/uaccess.h"
#include "linux/string.h"
#include "linux/ioctl.h"
#include "linux/platform_device.h"
#include "linux/of.h"
#include "std_util.h"
#include "timer_reg.h"

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
static int param_arr[3] = {65, 66, 67};
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

int bbb_driver_probe(struct platform_device *my_platform_device);
int bbb_driver_remove(struct platform_device *my_platform_device);

static const struct of_device_id timer4_bbb_id_st[] =
{
    {
        .compatible = "ti,am3358-bbb_timer4"
    },
    {
        // NULL sentinel
    }
};

static struct platform_driver bbb_driver_st =
{
    .probe = bbb_driver_probe,
    .remove = bbb_driver_remove,
    .driver =
    {
        .name = "bbb_timer4",
        .of_match_table = of_match_ptr(timer4_bbb_id_st),
    }
};

int bbb_driver_probe(struct platform_device *my_platform_device)
{
    int return_val = 0;
    struct device *my_device_ptr = &my_platform_device->dev;
    struct resource *io = NULL;
    hardware_mem_data_t *hardware_drv_mem_st_ptr = NULL;
    hardware_timer_data_t *hardware_timer_data_st_ptr = NULL;
    size_t i;

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

    hardware_drv_mem_st_ptr = devm_kzalloc(my_device_ptr, sizeof(hardware_mem_data_t), GFP_KERNEL);

    if (NULL == hardware_drv_mem_st_ptr)
    {
        printk("Can't allocate memory!!!\r\n");
        return -ENOMEM;
    }
    else
    {
        printk("Allocate memory successfully address %p for Registers!!!\r\n", hardware_drv_mem_st_ptr);
    }
    
    hardware_timer_data_st_ptr = devm_kzalloc(my_device_ptr, sizeof(hardware_timer_data_t), GFP_KERNEL);

    if (NULL == hardware_timer_data_st_ptr)
    {
        printk("Can't allocate memory!!!\r\n");
        return -ENOMEM;
    }
    else
    {
        printk("Allocate memory successfully address %p for Hardware Timer!!!\r\n", hardware_timer_data_st_ptr);
    }

    hardware_drv_mem_st_ptr->addr = devm_ioremap_resource(my_device_ptr, io);

    if (IS_ERR(hardware_drv_mem_st_ptr->addr))
    {
        printk("Can't get base address!!!\r\n");
        return PTR_ERR((const void *)hardware_drv_mem_st_ptr->addr);
    }
    else
    {
        printk(KERN_INFO "Base address is %p\r\n", hardware_drv_mem_st_ptr->addr);
    }

    hardware_timer_data_st_ptr->freq = 50;
    hardware_timer_data_st_ptr->duty_cycle = 50;
    hardware_timer_data_st_ptr->mem_data_ptr = hardware_drv_mem_st_ptr;

    if (PWM_gen_init(hardware_timer_data_st_ptr, my_device_ptr))
    {
        printk(KERN_ERR "Failed to generate PWM!!!!\r\n");
    }
    else
    {
        printk(KERN_INFO "Generate PWM successfully!!!!\r\n");
    }

    platform_set_drvdata(my_platform_device, (void *)hardware_timer_data_st_ptr);

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

    if (IS_ERR(device_create(my_class, NULL, my_dev, NULL, MY_CH_DRIVER)))
    {
        printk("Can't create device!!!\r\n");
        return E_FAIL;
    }
    else
    {
        printk("Create device successfully!!!\r\n");
    }

    if (0 != return_val)
    {
        pr_err("Can not create device driver!!\r\n");
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(my_dev, 1);
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

int bbb_driver_remove(struct platform_device *my_platform_device)
{
    // Module exit code

    hardware_timer_data_t *hardware_timer_data_st_ptr = NULL;

    hardware_timer_data_st_ptr = platform_get_drvdata(my_platform_device);
    PWM_gen_exit(hardware_timer_data_st_ptr);

    device_destroy(my_class, my_dev);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_dev, 1);
    printk("\r\nModule end!!!\r\n");

    return 0;
}

module_platform_driver(bbb_driver_st);
MODULE_DEVICE_TABLE(of, timer4_bbb_id_st);

module_param_array(param_arr, int, &arr_num, (S_IRUSR | S_IWUSR));
module_param_cb(param_cb, &my_kernel_param_ops, &param_cb, (S_IRUSR | S_IWUSR));

ssize_t my_read(struct file *my_file, char __user *user_buff, size_t buff_size, loff_t *my_loff)
{
    unsigned long len = sizeof(my_buff) - *my_loff;
    ssize_t byte_read = 0;

    if (*my_loff >= sizeof(my_buff))
    {
        *my_loff = 0;
        return 0;
    }
    else
    {
        // do nothing
    }

    if (len > buff_size)
    {
        len = buff_size;
    }
    else
    {
        // do nothing
    }

    printk("Size is %zu\r\n", sizeof(my_buff));
    unsigned long bytes_not_copy = copy_to_user(user_buff, (const void *)(my_buff + *my_loff), len);
    byte_read = (ssize_t)(len - bytes_not_copy);
    *my_loff += byte_read;
    
    return byte_read;
}

ssize_t my_write(struct file *my_file, const char __user *user_buff, size_t buff_size, loff_t *my_loff)
{
    unsigned long bytes_not_copy = 0;
    ssize_t bytes_written = 0;
    unsigned long bytes_to_copy = buff_size - *my_loff;

    if (bytes_to_copy > sizeof(my_buff))
    {
        printk(KERN_WARNING "User data is bigger than my_buff!!!\r\n");
        bytes_to_copy = sizeof(my_buff);
    }
    else
    {
        // do nothing
    }

    if (0 == *my_loff)
    {
        memset((void *)my_buff, 0, sizeof(my_buff));
    }
    else
    {
        // do nothing
    }

    bytes_not_copy = copy_from_user((void *)(my_buff + *my_loff), (const void *)user_buff, (unsigned long)bytes_to_copy);

    if (0 != bytes_not_copy)
    {
        printk("Wrote failed, continue!!!\r\n");
        bytes_written = (ssize_t)(bytes_to_copy - bytes_not_copy);
        *my_loff += (loff_t)bytes_written;
    }
    else
    {
        printk("Size is %zu\r\n", buff_size);
        printk("Wrote successfully %s\r\n", my_buff);

        *my_loff = 0;
        bytes_written = (ssize_t)buff_size;
    }

    return bytes_written;
}

int my_open(struct inode *my_inode, struct file *my_file)
{
    printk("my open\r\n");

    return 0;
}

int my_release(struct inode *my_inode, struct file *my_file)
{
    printk("my release\r\n\r\n");

    return 0;
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
                printk("Wrote successfully %d", (int)my_val);
            }
            else
            {
                printk("Wrote failed!!!\r\n");
                ret = -EFAULT;
            }

            break;
        }

        case RD_VALUE:
        {
            if (0 == copy_to_user((void*)arg, (const void*)&my_val, (unsigned long)sizeof(my_val)))
            {
                printk("Read successfully!!!\r\n");
            }
            else
            {
                printk("Read failed!!!\r\n");
                ret = -EFAULT;
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
