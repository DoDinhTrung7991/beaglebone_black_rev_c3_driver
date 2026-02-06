#include "asm-generic/io.h"

void my_reg_write(void __iomem *addr, uint32_t offset_u32, unsigned int val)
{
    iowrite32(val, addr + offset_u32);
}

unsigned int my_reg_read(void __iomem *addr, uint32_t offset_u32)
{
    return ioread32(addr + offset_u32);
}
