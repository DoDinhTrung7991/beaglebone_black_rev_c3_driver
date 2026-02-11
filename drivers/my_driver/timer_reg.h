#ifndef _TIMER_REG_H
#define _TIMER_REG_H

#include "std_util.h"
#include <linux/clk.h>

#define TIMER4_BASE             0x48044000U
#define TIMER4_END              0x48044FFFU

#define TIDR_OFFSET             0x00
#define TIOCP_CFG_OFFSET        0x10
#define IRQ_EOI_OFFSET          0x20
#define IRQSTATUS_RAW_OFFSET    0x24
#define IRQSTATUS_OFFSET        0x28
#define IRQENABLE_SET_OFFSET    0x2C
#define IRQENABLE_CLR_OFFSET    0x30
#define IRQWAKEEN_OFFSET        0X34
#define TCLR_OFFSET             0x38
#define TCRR_OFFSET             0x3c
#define TLDR_OFFSET             0x40
#define TTGR_OFFSET             0x44
#define TWPS_OFFSET             0x48
#define TMAR_OFFSET             0x4c
#define TCAR1_OFFSET            0x50
#define TSICR_OFFSET            0x54
#define TCAR2_OFFSET            0x58

struct hardware_timer_data
{
    struct clk *clk;
    unsigned int freq;
    unsigned int duty_cycle;
    hardware_mem_data_t *mem_data_ptr;
};

typedef struct hardware_timer_data hardware_timer_data_t;

int PWM_gen_init(hardware_timer_data_t *timer_data_ptr, struct device *device_ptr);
int PWM_gen_exit(hardware_timer_data_t *timer_data_ptr);

#endif
