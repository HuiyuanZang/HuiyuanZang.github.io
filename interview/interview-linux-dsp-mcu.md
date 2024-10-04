# Linux Kernel

## what kind of embedded linux driver did you implement?

## what different between Platform driver and normal device driver? Can you give some examples to indicate what kind drivers are platerform driver; and some examples of normal drivers?

### platform driver
See <linux/platform_device.h> for the driver model interface to the platform bus: platform_device, and platform_driver. This pseudo-bus is used to connect devices on busses with minimal infrastructure, like those used to integrate peripherals on many system-on-chip processors, or some “legacy” PC interconnects; as opposed to large formally specified ones like PCI or USB.

Platform drivers are tied to platform devices, which are typically statically declared (e.g., in a board file or device tree) because they cannot be dynamically discovered.

The platform driver framework is used in systems where the hardware cannot be probed and detected, so the kernel or bootloader provides information about the device, such as its memory addresses and interrupt numbers.

These drivers interact with devices on system-specific buses like memory-mapped I/O, often in embedded systems or SoCs.


**examples:**

   - GPIO controllers on an SoC

   - I2C controllers


### Normal (Traditional) Device Driver

A normal device driver typically refers to a driver written for devices that reside on discoverable buses like PCI, USB, or other standard buses. These devices are dynamically detected and managed by the kernel using a bus-specific mechanism. Normal drivers are commonly used in desktop or server environments and in systems where the hardware can be hot-plugged or discovered during boot.

Characteristics:
Dynamic Discovery: Normal device drivers manage devices that can be dynamically discovered by the kernel using bus-specific probing mechanisms. For example, PCI devices are discovered by scanning the PCI bus.

Bus-Specific: Normal device drivers are written for devices on standard buses such as PCI, USB, I2C, SPI, or others. These buses typically define their own probing and binding mechanisms.

Device Matching: Devices are matched with drivers based on properties such as device IDs (e.g., PCI vendor and device IDs for PCI devices or USB IDs for USB devices).

Hot-Plug Support: These drivers often support hot-pluggable devices, meaning devices can be added or removed while the system is running (e.g., USB devices).

Use Case: Normal device drivers are prevalent in general-purpose computing systems like PCs, servers, and laptops where hardware components such as network cards, graphics cards, and USB peripherals are dynamically managed.


## As you might know, when we talk about platform device driver, the concept--platform device, and platform driver and bus play critical roles. Do you think we need real physical bus here? 


In the context of platform device drivers in the Linux kernel, the term "bus" does not necessarily refer to a physical bus like PCI, USB, or I2C. Instead, it represents a logical bus or a software abstraction used by the Linux kernel to group and manage platform devices and platform drivers.

No Physical Bus Needed in Platform Drivers
Unlike traditional buses like PCI or USB that refer to real physical connections with standardized protocols, the platform bus in platform device drivers is purely a software construct. It does not correspond to any physical hardware bus but is used by the kernel to bind platform devices with their respective platform drivers.

Key Concepts:
Platform Device:

A platform device is a device that does not reside on a discoverable bus such as PCI, USB, or I2C. Instead, it is typically an on-chip peripheral or a device directly connected to the CPU through memory-mapped I/O (MMIO) or other integrated connections.
These devices are usually statically declared, either in the kernel code (in older systems) or in a device tree or ACPI table (in modern systems).
Platform Driver:

A platform driver is the driver that manages a platform device. It handles initialization, shutdown, and communication with the device.
Platform drivers are matched with platform devices based on a string identifier (such as the compatible property in the device tree or a hardcoded device name).
Platform Bus:

The platform bus is the software abstraction used by the Linux kernel to manage platform devices and platform drivers. It is not a physical bus.
The platform bus is responsible for matching platform devices with their respective platform drivers and ensuring that the driver code runs when the device is needed.
From the kernel's perspective, the platform bus works like a typical Linux bus (similar to PCI or USB buses) but without any associated physical hardware bus.
Logical Role of the Platform Bus
The platform bus exists to support the kernel’s internal device-driver model. In the kernel, every device and driver exists within a bus subsystem, and even though no physical bus exists in the case of platform devices, the kernel uses the platform bus as the mechanism to:

Enumerate platform devices.
Match platform devices with the correct drivers.
Provide a uniform API to register and manage platform drivers and devices.
This logical bus ensures that the kernel treats platform devices similarly to other devices in the system, even if the devices are not discovered dynamically but are declared statically.

Platform Bus in Action
The platform bus in Linux essentially plays the role of matching and binding drivers to devices that are not on dynamically discoverable hardware buses. It does so by using static descriptions (from device tree or ACPI) that tell the kernel what devices are present, what resources they use (such as memory regions and interrupts), and which drivers should manage them.

Here’s a simplified conceptual flow:

Platform Device Declaration: The device is described in the device tree or ACPI, which provides the kernel with the necessary details (e.g., memory address, interrupts, compatible string).

Platform Bus: The platform bus subsystem collects the declared devices and attempts to match them with the correct platform driver using the compatible string or device name.

Platform Driver Binding: Once a match is found, the platform bus subsystem binds the platform driver to the platform device. The driver’s probe() function is then called to initialize the device.

Summary
The platform bus is not a physical bus but a logical/software bus used to manage platform devices and drivers.
It exists as an abstraction to ensure consistency in the Linux device-driver model, allowing platform devices (which cannot be dynamically discovered) to be handled in a way similar to devices on physical buses.
The real role of the platform bus is to facilitate the matching of statically declared devices with the appropriate drivers, ensuring proper device initialization and operation.
In short, no real physical bus is required in the context of platform device drivers; the platform bus is just a kernel-level mechanism to manage statically defined devices within the Linux device-driver model.


### Can you tell me what different kmalloc vmalloc devm_kmalloc?

- kmalloc()
Description:

Allocates physically contiguous memory in the kernel's virtual address space.
Memory is directly accessible by the CPU and can be used for DMA operations if the memory is physically contiguous.
The allocated memory is within the kernel's low memory space, making it fast and efficient for small allocations.

- vmalloc()
Description:

Allocates virtually contiguous memory in the kernel's virtual address space but not necessarily physically contiguous.
The memory is still mapped to physical pages, but the pages do not have to be contiguous in physical RAM.



- devm_kmalloc()
Description:

A managed version of kmalloc() that automatically frees the memory when the device it is associated with is removed.
This is part of the device-managed (devm_) family of APIs, which simplifies resource management in device drivers.

### Can I use kmalloc() and vmalloc in interrupt handler?
You can use kmalloc() in interrupt handlers, but there are some caveats.Atomic Context: kmalloc() can be used in interrupt context if the GFP_ATOMIC flag is specified, which tells the kernel to allocate memory in a context where sleeping is not allowed.

Not recommended to use vmalloc() in interrupt handlers.vmalloc() may involve more complex memory management and can potentially sleep or block while allocating memory, which is not allowed in interrupt context.
It can lead to longer latency and might result in deadlocks if used improperly.



### Device Tree， If there is only one CSI connector in my hardware board, but I want to support multiple image sensors such as imx477, imx219， and lt6911 sensors, all the drivers source code are done now, I changed the sensor, but I don't want to rebuild the kernel and device tree, neither reprogram kernel image or update device tree binary blob. What should I do? And How?

fragment@2 {
		target = <&cam_module0_drivernode0>;
		__overlay__ {
			status = "okay";
			pcl_id = "v4l2_sensor";
			devname = "imx477 9-001a";
			proc-device-tree = "/proc/device-tree/cam_i2cmux/i2c@0/rbpcv3_imx477_a@1a";
		};
	};


### Explain the difference between kernel space and user space in Linux. How many ways to pass user space pointer to the kernel?
Answer:

In Linux, memory is divided into two primary regions: kernel space and user space. 

Kernel space is where the kernel itself operates and manages core system functions. It has unrestricted access to the hardware and contains critical processes integral to the operating system's functionality. Misbehavior in kernel space can lead to system crashes or other serious issues. 

On the other hand, user space is where regular applications and user-level processes run. It has limited access to hardware and must communicate with the kernel via system calls to perform actions like reading from a disk or displaying on a screen. 

Essentially, kernel space is for core system operations, user space is for everyday applications, and they are isolated to enhance stability and security.


- copy_from_user() and copy_to_user() in ioctl() 

- mmap()： The mmap() system call allows user space to map device memory or shared memory into its address space.

- Netlink Sockets

- Signal Handlers

### Can you design a linux driver for HIGH speed SPI communication device? Like, I got L3GD20H in my hand,  A 3-axis gyroscope that supports SPI clock frequencies up to 10 MHz for high-speed communication. 

1. Basic SPI Driver Structure:
Linux SPI drivers are usually written by implementing a set of callbacks, such as probe() (for device initialization) and remove() (for device cleanup).
The driver will use the SPI subsystem to communicate with the device, read sensor data, and configure device settings.
2. L3GD20H Communication:
The L3GD20H communicates using a standard SPI protocol.
You’ll need to implement the initialization code, sensor configuration (such as setting the output data rate, power mode, etc.), and reading the gyroscope values.

Synchronization:

The driver uses a mutex (mutex_lock() and mutex_unlock()) to ensure that SPI operations are thread-safe, preventing race conditions when accessing the device.
Driver Binding:

The of_match_table specifies the device tree compatibility strings, allowing the SPI subsystem to match the driver with the appropriate hardware.

### Then I realize that gyroscope provide one output pin, so called data-ready interrupt pin, can you use that pin to improve your driver? and How?

Yes, you can integrate the Data Ready interrupt provided by the L3GD20H gyroscope into the driver to improve efficiency. This would allow the driver to avoid continuous polling and instead trigger data reads when the gyroscope signals that new data is available. The actual data reading would then be performed in the bottom half of the interrupt handler to avoid blocking the interrupt context for too long.


### I still want to get better performance, then I think I can bring SPI DMA here, so how should I 
implement DMA with SPI here?


Yes, using DMA (Direct Memory Access) with SPI can significantly improve performance when dealing with large amounts of data or frequent transfers, as it offloads the CPU from managing the data transfer, allowing the SPI controller and the DMA engine to handle it directly.

The concept of DMA in SPI communication involves setting up the DMA controller to transfer data from the SPI peripheral to a buffer in memory without CPU intervention. This is especially useful in high-speed applications like sensor data acquisition, where data is transmitted in bursts (e.g., from sensors like gyroscopes).

Here’s how you can modify the SPI driver to use DMA for reading data from the L3GD20H gyroscope:

Allocate DMA-Capable Buffers:

Ensure that the buffer used for SPI transfers is allocated using DMA-safe functions like dma_alloc_coherent() or devm_kzalloc() with proper alignment to ensure the buffer is DMA-capable.
Configure SPI Controller for DMA:

You will need to configure the SPI controller to use DMA for transfers. This is typically done in the SPI subsystem and driver setup.
SPI Transfer Setup:

In the spi_transfer structure, you must specify the DMA buffers and flags to indicate that the transfer should use DMA.
DMA Capable SPI Controller:

Ensure that the SPI controller on your platform supports DMA transfers. The controller driver should have support for configuring and managing DMA transfers.

``` C
 spi_message_init(&m);
    spi_message_add_tail(&t, &m);

    // Start the DMA-based SPI transfer
    ret = spi_sync(data->spi, &m);

    dma_alloc_coherent


    spi0: spi@0 {
    compatible = "myboard,spi";
    reg = <0x0 0x1000>;
    spi-max-frequency = <10000000>;  // 10 MHz
    dmas = <&dma0 0>, <&dma0 1>;
    dma-names = "tx", "rx";
    status = "okay";
};

```
### what the key things if I want to support multiple users in user space accessing the same SPI device?

you’ll need to implement a few key features. This typically involves ensuring that the driver handles concurrent access safely and manages user space interactions properly. Below is a general outline of the necessary steps to achieve this:


### what if the critical part shared between Kernel thread and top half of interrupt handler, what can you do to avoid race condition in Linux kernel?

spin_lock 


### what typical bottom half used in Linux interrupt handler? and what their context? process context or interrupt context respectively?

In Linux, the bottom half of an interrupt handler is a deferred mechanism used to complete tasks that do not need to be executed in the top half (the actual interrupt service routine, or ISR). The top half runs in interrupt context, which is a highly time-sensitive environment where only a limited set of operations is allowed (e.g., no blocking or sleeping operations). The bottom half allows these deferred tasks to be executed in a less time-constrained environment.

Typical Bottom Halves Used in Linux:
SoftIRQs:

Description: SoftIRQs are low-level mechanisms for handling bottom halves. They are predefined in the kernel and used for specific purposes such as network packet processing or task scheduling.
Context: SoftIRQs run in interrupt context, but they are executed outside of the top-half ISR. They cannot sleep or block because they run in interrupt context.
Usage: Mostly used for high-frequency tasks like networking (e.g., NET_RX_SOFTIRQ, NET_TX_SOFTIRQ), timers, and tasklet processing.
Tasklets:

Description: Tasklets are built on top of SoftIRQs and provide a simplified interface for deferred execution. Each tasklet is scheduled to run at a lower priority than SoftIRQs but still in interrupt context.
Context: Tasklets also run in interrupt context and cannot block, sleep, or use functions that require user-space interaction. They are serialized in execution, meaning a tasklet cannot preempt another tasklet of the same type.
Usage: Tasklets are often used in networking, device drivers, and other kernel subsystems where quick deferred work is needed.
Workqueues:

Description: Workqueues are kernel mechanisms that allow deferring work to a process context. This means that the deferred work can sleep and perform more complex operations, such as interacting with user space, acquiring locks that could sleep, and doing I/O operations.
Context: Workqueues run in process context. This allows them to sleep, block, and access user-space memory, making them much more flexible than tasklets or softIRQs.
Usage: Workqueues are used when deferred work needs to sleep or block, such as in file system operations, I/O scheduling, or other complex tasks in drivers.



devm_request_threaded_irq() op Half (Interrupt Context):

The my_top_half_handler function is the ISR (top half), running in interrupt context. It does minimal work, possibly acknowledging the interrupt and returning IRQ_WAKE_THREAD to signal that the threaded handler should be executed.
Bottom Half (Process Context):

The my_threaded_handler function is the deferred processing (bottom half), running in process context. It can sleep and block as it executes in a kernel thread, making it suitable for handling more complex tasks like I/O, waiting for hardware, or interacting with user space.
IRQF_ONESHOT Flag:

The IRQF_ONESHOT flag ensures that the interrupt is masked until the bottom half (thread_fn) has finished executing. This is useful to prevent re-entrancy and ensures that the bottom half fully processes one interrupt before handling another.
Benefits of Using devm_request_threaded_irq():
Simplified Resource Management: The device-managed API automatically frees the IRQ when the device is removed, reducing the complexity of resource management.
Separation of Concerns: By splitting the work between the top half (interrupt context) and the bottom half (process context), you can ensure that time-sensitive work is done quickly while more complex or blocking tasks are deferred to a safer execution context.


# RTOS

## In RTOS, the round-rubin combining with different priority group is normammly used for scheduler,  can you design data structures to implement this kind of task scheduling?

Task Structure: This structure represents a task and includes its properties, such as priority, task ID, and task state.

Priority Queue: An array of queues, where each queue corresponds to a different priority group. Each queue will hold the tasks of that priority.

Scheduler Structure: This structure holds the queues and manages scheduling.


```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PRIORITIES 5 // Number of priority levels
#define MAX_TASKS 10     // Maximum tasks per priority level

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_TERMINATED
} TaskState;

typedef struct Task {
    int task_id;                // Unique identifier for the task
    TaskState state;            // Current state of the task
    int priority;               // Priority of the task (0 is highest priority)
    void (*task_func)(void);    // Pointer to the task function
    struct Task* next;          // Pointer to the next task in the queue
} Task;

typedef struct {
    Task* front;                // Front of the queue
    Task* rear;                 // Rear of the queue
} TaskQueue;

typedef struct {
    TaskQueue queues[MAX_PRIORITIES]; // Array of queues for each priority
} Scheduler;

```

### What is task priority inversion, and how can it be mitigated in FreeRTOS?

Discuss the problem of priority inversion and techniques like priority inheritance.
How do you implement a watchdog timer in FreeRTOS?

### Describe the concept of a watchdog timer and how to implement it in FreeRTOS.




# Linux User Space C and C++

### Consumer and producter pattern in Linux user space,

### what if producter can feed faster event than cousumer take, 


### 

# DSP




# MCU STM32

### Can you write down a function or predefined macro to determin if the CPU is little endian or big endian?

### I got a MCU, and there is read-only status register which I want to access, the address of this register is given such as 0x0004; this MCU is 32 bits. Can you tell me how to define a variable to 
get the value of register.


### static usage in C





# I2C, SPI, CAN

# BuildRoot

# Yocto