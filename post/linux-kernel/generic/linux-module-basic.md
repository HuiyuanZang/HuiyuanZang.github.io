# Basic module knowledge on Linux kernel 


## depmod utility

depmod is a tool that you run during the kernel build process to generate module dependency files. It does that by reading each module in
/lib/modules/<kernel_release>/ to determine what symbols it should export and what symbols it needs. The result of that process is written to the modules.dep file and its binary version modules.dep.bin . It is a kind of module indexing.

**Example**

The following is a series of commands that helps to illustrate a usual way to use depmod command in Linux. Each command is prefixed with sudo since each of them requires proper root permissions:

Retrieving file from default location:

``` shell
$  ln -s /path/to/your-kernel-module.ko /lib/modules/`uname -r`
$  /sbin/depmod -a
$  modprobe your-kernel-module
```
``` shell
$ ln -s lkm.ko /lib/modules/2.6.32-21-generic/
$  depmod -a
$ modprobe lkm
$  modprobe -r lkm
```

## /etc/modules-load.d/<filename>.conf
If you want a module to be loaded at boot time, just create the file /etc/modules-load.d/<filename>.conf , and add the module's name that should be loaded, one per line. <filename> should be meaningful to you, and people usually use modules: /etc/modules-load.d/modules.conf . You may create as many .conf files as you need: An example of /etc/modules-load.d/mymodules.conf is as follows:
```
# this line is a comment
uio
iwlwifi
```



## Auto-loading
The depmod utility doesn't only build modules.dep and modules.dep.bin files. It does more than that. When a kernel developer actually writes a driver, they know exactly what hardware the driver will support. They are then responsible for feeding the driver with the product and vendor IDs of all devices supported by the driver. depmod also processes module files in order to extract and gather that information, and generates a modules.alias file, located in /lib/modules/<kernel_release>/modules.alias , which will map devices to their drivers.
An excerpt from modules.alias is as follows:
```
alias usb:v0403pFF1Cd*dc*dsc*dp*ic*isc*ip*in* ftdi_sio
alias usb:v0403pFF18d*dc*dsc*dp*ic*isc*ip*in* ftdi_sio
alias usb:v0403pDAFFd*dc*dsc*dp*ic*isc*ip*in* ftdi_sio
alias usb:v0403pDAFEd*dc*dsc*dp*ic*isc*ip*in* ftdi_sio
alias usb:v0403pDAFDd*dc*dsc*dp*ic*isc*ip*in* ftdi_sio
alias usb:v0403pDAFCd*dc*dsc*dp*ic*isc*ip*in* ftdi_sio
alias usb:v0D8Cp0103d*dc*dsc*dp*ic*isc*ip*in* snd_usb_audio
alias usb:v*p*d*dc*dsc*dp*ic01isc03ip*in* snd_usb_audio
alias usb:v200Cp100Bd*dc*dsc*dp*ic*isc*ip*in* snd_usb_au
```

In this step, you'll need a user space hot-plug agent (or device manager), usually udev (or
mdev ) that will register with the kernel in order to get notified when a new device appears.
The notification is done by the kernel, sending the device's description (pid, vid, class,
device class, device subclass, interface, and all other information that may identify a device)
to the hot-plug daemon, which in turn calls modprobe with this information. modprobe
then parses the modules.alias file in order to match the driver associated with the device.
Before loading the module, modprobe will look for its dependencies in module.dep . If it
finds any, the dependencies will be loaded prior to the associated module loading;
otherwise, the module is loaded directly.



## __init and__exit attributes

__init and__exit are actually kernel macros, defined in include/linux/init.h ,
shown as follows:
```
# define __init__section(.init.text)
# define __exit__section(.exit.text)
```

The __init keyword tells the linker to place the code in a dedicated section into the kernel
object file. This section is known in advance to the kernel, and freed when the module is
loaded and the init function finished. This applies only to built-in drivers, not to loadable
modules. The kernel will run the init function of the driver for the first time during its
boot sequence.

You may customize these sections, change their default location, or even add additional sections by
providing a linker script, called a linker definition file (LDF) or linker definition script
(LDS). Now, all you have to do is to inform the linker of the symbol placement through
compiler directives. The GNU C compiler provides attributes for that purpose. In the case
of the Linux kernel, there is a custom LDS file provided, located in
```
arch/<arch>/kernel/vmlinux.lds.S 
```
 __init and__exit are then used to mark symbols to be placed onto dedicated sections mapped in the kernel's LDS files.
In conclusion, __init and__exit are Linux directives (actually macros), which wrap the
C compiler attribute used for symbol placement. They instruct the compiler to put the code
they prefix respectively in the .init.text and .exit.text sections, even though the
kernel can access different object sections.

## print out different sections
You can run objdump -h module.ko in order to print out different
sections that constitute the given module.ko kernel module

## dump .modinfo or modinfo

You can dump the contents of the .modeinfo section of a kernel module using the
objdump -d -j .modinfo command on the given module:
``` shell
$ objdump my-module.ko -d -j .modinfo 
```

OR
``` shell
$ modinfo my-module.ko 
```

## Reference 

[1] John Madieu, "Linux Device Drivers Development Develop customized drivers for embedded Linux"
