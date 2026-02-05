#include "linux/clk.h"
#include "timer_reg.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "std_util.h"

int PWM_gen_init(hardware_timer_data_t *timer_data_ptr, struct device *device_ptr)
{
    int return_val = E_OK;
    struct clk *clk_st_ptr = NULL;
    unsigned long rate = 0;
    uint32_t period_cycles;
    uint32_t duty_cycles;
    uint32_t tldr_val;
    uint32_t tmar_val;
    uint32_t tclr_val;

    /* Get Clock */
    clk_st_ptr = devm_clk_get(device_ptr, "fck");
    if (IS_ERR(clk_st_ptr))
    {
        printk(KERN_WARNING "Failed to get clock!!!\r\n");
        return_val = PTR_ERR(clk_st_ptr);
        return return_val;
    }
    else
    {
        printk(KERN_INFO "Get clock source successfully!!!\r\n");
        timer_data_ptr->clk = clk_st_ptr;
    }

    return_val = clk_prepare_enable(clk_st_ptr);

    if (return_val)
    {
        printk(KERN_WARNING "Failed to enable clock!!!\r\n"); // write to CM_PER_TIMER4_CLKCTRL
        return return_val;
    }
    else
    {
        printk(KERN_INFO "Enable clock successfully!!!\r\n");
    }

    rate = clk_get_rate(clk_st_ptr);
    printk(KERN_INFO "Timer clock rate: %lu Hz\n", rate);

    if (rate == 0)
    {
        printk(KERN_ERR "Clock rate is 0, cannot calculate PWM parameters!!!\r\n");
        return -EINVAL;
    }

    if (timer_data_ptr->freq == 0)
    {
        printk(KERN_ERR "Target frequency cannot be 0!!!\r\n");
        return -EINVAL;
    }

    /* Calculate PWM parameters */
    period_cycles = rate / timer_data_ptr->freq;
    duty_cycles = period_cycles * timer_data_ptr->duty_cycle / 100;
    tldr_val = 0xFFFFFFFFU - period_cycles + 1;
    tmar_val = tldr_val + duty_cycles;

    /* Configure Timer Registers */
    // Stop timer
    my_reg_write(timer_data_ptr->mem_data_ptr->addr, TCLR_OFFSET, 0);
    // Load registers
    my_reg_write(timer_data_ptr->mem_data_ptr->addr, TLDR_OFFSET, tldr_val);
    my_reg_write(timer_data_ptr->mem_data_ptr->addr, TMAR_OFFSET, tmar_val);
    my_reg_write(timer_data_ptr->mem_data_ptr->addr, TCRR_OFFSET, tldr_val);
    // Start timer in PWM mode (PT=0, TRG=2, CE=1, AR=1, ST=1)
    tclr_val = (0 << 12) | (2 << 10) | (1 << 6) | (1 << 1) | (1 << 0);
    my_reg_write(timer_data_ptr->mem_data_ptr->addr, TCLR_OFFSET, tclr_val);

    return return_val;
}

int PWM_gen_exit(hardware_timer_data_t *timer_data_ptr)
{
    // Module exit code
    if (timer_data_ptr->mem_data_ptr)
    {
        // Stop timer
        my_reg_write(timer_data_ptr->mem_data_ptr->addr, TCLR_OFFSET, 0);

        if (timer_data_ptr->clk)
        {
            clk_disable_unprepare(timer_data_ptr->clk);
        }
    }

    return 0;
}
