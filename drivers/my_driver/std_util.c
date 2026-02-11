#include <linux/io.h>
#include <linux/types.h>

void my_reg_write(volatile void __iomem *addr, unsigned int offset_u32, u32 val)
{
    iowrite32(val, (void __iomem *)((u8 __iomem *)addr + offset_u32));
}

u32 my_reg_read(volatile void __iomem *addr, unsigned int offset_u32)
{
    return ioread32((const volatile void __iomem *)((u8 __iomem *)addr + offset_u32));
}
