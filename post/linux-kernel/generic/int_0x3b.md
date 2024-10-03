# asm("int $0x3B")


This used to work on older kernel versions, but fails on later
versions. The reason is that the generic IRQ handler do_IRQ() has been
changed for better IRQ handling performance. Instead of using the
irq_to_desc() function to get the IRQ descriptor, it reads it from the
per-CPU data. The descriptor is put there during the physical device
initialization. Since this pseudo device driver don't have a physical
device, do_IRQ() don't find it there and returns with an error. If we
want to simulate IRQ using software interrupt, we must first write the
IRQ descriptor to the per-CPU data. Unfortunately, the symbol
vector_irq, the array of the IRQ descriptors in the per-CPU data, is
not exported to kernel modules during kernel compilation. The only way
to change it, is to recompile the whole kernel. If you think it worth
the effort, you can add the line:

EXPORT_SYMBOL (vector_irq);

in the file: arch/x86/kernel/irq.c

right after all the include lines. After compiling and booting from
the newly compiled kernel, change your driver as follows:

Add an include line:

    #include <asm/hw_irq.h>

change the read function to:

static ssize_t etx_read(struct file *filp,
                char __user *buf, size_t len, loff_t *off)
{
        struct irq_desc *desc;

        printk(KERN_INFO "Read function\n");
        desc = irq_to_desc(11);
        if (!desc) return -EINVAL;
        __this_cpu_write(vector_irq[59], desc);
        asm("int $0x3B");  // Corresponding to irq 11
        return 0;
}

