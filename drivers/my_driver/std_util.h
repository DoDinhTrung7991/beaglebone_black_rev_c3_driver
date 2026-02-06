#ifndef __STD_UTIL_H
#define __STD_UTIL_H

#include <linux/platform_device.h>

#define E_OK      0
#define E_FAIL   -1

struct hardware_mem_data
{
    void __iomem *addr;
    struct platform_device *pdev;
};

typedef struct hardware_mem_data hardware_mem_data_t;

void my_reg_write(void __iomem *addr, uint32_t offset_u32, unsigned int val);

unsigned int my_reg_read(void __iomem *addr, uint32_t offset_u32);

#endif
