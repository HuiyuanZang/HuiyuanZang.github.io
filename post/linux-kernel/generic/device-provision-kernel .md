# Device Provision in Linux Kernel


At the opposite end to hot-pluggable devices, the kernel has no idea what devices are
present on your system, what they are capable of, or what they need in order to work
properly. There is no auto-negotiation process, so any information provided to the kernel
would be welcome. There are two methods to inform the kernel about the resources (IRQ,
DMA, memory region, I/O ports, buses) and data (any custom and private data structure
you may want to pass to the driver) that the device needs, which are discussed here.

## Device provisioning – the old and deprecated way
This method is to be used with the kernel version that does not support a device tree. With
this method, drivers remain generic and devices are registered in board-related source files.
### Resources
Resources represent all the elements that characterize the device from the hardware point
of view, and that the device needs in order to be set up and work properly. There are only
six types of resources in the kernel, all listed in include/linux/ioport.h , and used as
flags to describe the resource's type:

``` C
# define IORESOURCE_IO  0x00000100
# define IORESOURCE_MEM 0x00000200
# define IORESOURCE_REG 0x00000300
# define IORESOURCE_IRQ 0x00000400
# define IORESOURCE_DMA 0x00000800
# define IORESOURCE_BUS 0x00001000

```

### Platform data
Any other data whose type is not a part of the resource types enumerated in the preceding
section falls here (for example, GPIO). Whatever their type is, the struct
platform_device contains a struct device field, which in turn contains a struct
platform_data field. Usually, you should embed that data in a structure and pass it to the
platform_device.device.platform_data field. Let's say, for example, that you declare
a platform device that needs two GPIO numbers as platform data, one IRQ number, and
two memory regions as resources.


### Device provisioning – the new and recommended way
In the first method, any modification will necessitate rebuilding the whole kernel. If the
kernel had to include any application/board-specific configurations, its size would greatly
increase. In order to keep things simple and separate device declarations (since they are not
really part of the kernel) from the kernel source, a new concept has been introduced: the
device tree. The main goal of DTS is to remove very specific and never-tested code from the
kernel. With the device tree, platform data and resources are homogeneous. The device tree
is a hardware description file and has a format similar to a tree structure, where every
device is represented with a node, and any data or resource or configuration data is
represented as the node's property. This way, you only need to recompile the device tree
when you make some modifications. 