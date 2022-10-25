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
