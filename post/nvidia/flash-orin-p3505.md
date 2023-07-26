# How to flash nvidia jetson orin nx and nano on P3505 compatible board

## EEPROM Modifications
EEPROM is an optional component for a customized carrier board. If the carrier board is designed without an EEPROM, the following modifications will be needed on the MB2 BCT file:

Linux_for_Tegra/bootloader/t186ref/BCT/tegra234-mb2-bct-misc-p3767-0000.dts

```diff
- cvb_eeprom_read_size = <0x100>
+ cvb_eeprom_read_size = <0x0>
```
So fist thing is to check if there is EEPROM on carrier board with hardware engineer.

## How to put the target device into Force Recovery Mode

- Power off your target device, disconnect HDMI and USB TYPE-c cable
- Put jumper into REC PIN and GND PIN
- Power on to see green LED
- Connect you USB TYPE-C with host ubuntu machine(only support on 18.04 or 20.04) 
- Connect HDMI as optional step

## NVMe
There is no eMMC and SD slot on Orin NX and Nano, so only NVMe and USB driver are only supported. But from my test, I cannot flash USB drive for some reason.


## Modifications to support HDMI 4k/8K

If you flash Orin NX or nano by either GUI Nvidia SDKManager or shell script nvsdkmanager_flash.sh, the jetson-orin-nano-devkit.conf is intented to be used as default configuration which is designed for the 3767 carrier board device tree. In 3767 carrier board, Nvidia only support 4k/8K Display port rather than HDMI 4k/8k. Therefore, 

### If you want to flash Orin either by SDKManager or nvsdkmanager_flash.sh:

- step 1
Go to <Your Nvidia SDK Path>/JetPack_5.x.x_Linux_JETSON_ORIN_NX_TARGETS/Linux_for_Tegra for Orin Nx 
Go to <Your Nvidia SDK Path>/JetPack_5.x.x_Linux_JETSON_ORIN_NANO_TARGETS/Linux_for_Tegra for Orin Nx 

- step 2
 please replace jetson-orin-nano-devkit.conf with the following content

``` shell
source "${LDK_DIR}/p3509-a02+p3767-0000.conf";
EMMC_CFG="flash_t234_qspi_nvme.xml";

```
- step 3
   Put the target device into Force Recovery mode, then run Nvidia SDK Manager or nvsdkmanager_flash.sh, choose NVMe to store all rootfs

### If you want to flash Orin by command line l4t_initrd_flash.sh

- step 1
Go to <Your Nvidia SDK Path>/JetPack_5.x.x_Linux_JETSON_ORIN_NX_TARGETS/Linux_for_Tegra for Orin Nx 
Go to <Your Nvidia SDK Path>/JetPack_5.x.x_Linux_JETSON_ORIN_NANO_TARGETS/Linux_for_Tegra for Orin Nx 
- step 2
 install the flash requirements by 

 ```shell
$ sudo tools/l4t_flash_prerequisites.sh
 ```

 - step 3 
 Put the target device into Force Recovery mode, then run 
 
 ```shell
 $ sudo ./tools/kernel_flash/l4t_initrd_flash.sh --external-device nvme0n1p1 -c tools/kernel_flash/flash_l4t_external.xml -p "-c bootloader/t186ref/cfg/flash_t234_qspi.xml" --showlogs --network usb0 p3509-a02+p3767-0000 internal

 ```

 NOTE: when I used the command flash.sh, the HDMI stop workding. So I don't recommend to use flash.sh 

   

