# SPI user mode driver
There are two ways of using the user mode SPI device driver. To be able to do that, you
need to enable your device with the **spidev** driver. An example would be as follows:
``` Shell

spidev@0x00 {
    compatible = "spidev";
    spi-max-frequency = <800000>; /*It depends on your device*/
    reg = <0>; /*correspond to chip select 0*/
};
```

You can call either the read/write functions or an ioctl() . By calling read/write, you can
only read or write at a time. If you need full-duplex read and write, you have to use the
Input Output Control (ioctl) commands. Examples for both are provided. This is the
read/write example. You can compile it either with the cross-compiler of the platform or
with the native compiler on the board:


``` C
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    int i,fd;
    char wr_buf[]={0xff,0x00,0x1f,0x0f};
    char rd_buf[10];
    if (argc<2) {
        printf("Usage:\n%s [device]\n", argv[0]);
        exit(1); 
    }
    fd = open(argv[1], O_RDWR);
    if (fd<=0) {
        printf("Failed to open SPI device %s\n",argv[1]);
        exit(1);
    }
    if (write(fd, wr_buf, sizeof(wr_buf)) != sizeof(wr_buf))
        perror("Write Error");
    if (read(fd, rd_buf, sizeof(rd_buf)) != sizeof(rd_buf))
        perror("Read Error");
    else
       for (i = 0; i < sizeof(rd_buf); i++)
          printf("0x%02X ", rd_buf[i]);
    close(fd);
    return 0;
}

```

The advantage of using IOCTL is that you can work in full duplex. The best example you
can find is documentation/spi/spidev_test.c in the kernel source tree, of course.
The preceding example using read/write did not change any SPI configuration. However,
the kernel exposes to the user space a set of IOCTL commands, which you can use in order
to set up the bus according to your needs, just like what is done in the DT. The following
example shows how you can change the bus settings:


```C
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
static int pabort(const char *s)
{
perror(s);
return -1;
}
static int spi_device_setup(int fd)
{
int mode, speed, a, b, i;
int bits = 8;
/*
* spi mode: mode 0
*/
mode = SPI_MODE_0;
a = ioctl(fd, SPI_IOC_WR_MODE, &mode); /* write mode */
b = ioctl(fd, SPI_IOC_RD_MODE, &mode); /* read mode */
if ((a < 0) || (b < 0)) {
return pabort("can't set spi mode");
}
/*
* Clock max speed in Hz
*/
speed = 8000000; /* 8 MHz */
a = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed); /* Write speed */
b = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); /* Read speed */
if ((a < 0) || (b < 0)) {
    return pabort("fail to set max speed hz");
}
/*
* setting SPI to MSB first.
* Here, 0 means "not to use LSB first".
* In order to use LSB first, argument should be > 0
*/
i = 0;
a = ioctl(dev, SPI_IOC_WR_LSB_FIRST, &i);
b = ioctl(dev, SPI_IOC_RD_LSB_FIRST, &i);
if ((a < 0) || (b < 0)) {
pabort("Fail to set MSB first\n");
}
/*
* setting SPI to 8 bits per word
*/
bits = 8;
a = ioctl(dev, SPI_IOC_WR_BITS_PER_WORD, &bits);
b = ioctl(dev, SPI_IOC_RD_BITS_PER_WORD, &bits);
if ((a < 0) || (b < 0)) {
pabort("Fail to set bits per word\n");
}
return 0;
}
```

You can have a look at Documentation/spi/spidev for more information on spidev
ioctl commands. When it comes to sending data over the bus, you can use
a SPI_IOC_MESSAGE(N) request, which offers full-duplex access, and composite
operations without chip select deactivation, thus offering multi-transfer support. It is the
equivalent of the kernel's spi_sync() . Here, a transfer is represented as an instance of
struct spi_ioc_transfer , which is the equivalent of the kernel's struct
spi_transfer , and whose definition can be found in
include/uapi/linux/spi/spidev.h . The following is an example of its use:

```C
static void do_transfer(int fd)
{
int ret;
char txbuf[] = {0x0B, 0x02, 0xB5};
char rxbuf[3] = {0, };
char cmd_buff = 0x9f;
struct spi_ioc_transfer tr[2] = {
0 = {
.tx_buf = (unsigned long)&cmd_buff,
.len = 1,
.cs_change = 1; /* We need CS to change */
.delay_usecs = 50, /* wait after this transfer */
.bits_per_word = 8,
},
[1] = {
.tx_buf = (unsigned long)tx,
.rx_buf = (unsigned long)rx,
.len = txbuf(tx),
.bits_per_word = 8,
},
};
ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
if (ret == 1){
perror("can't send spi message");
exit(1);
}
for (ret = 0; ret < sizeof(tx); ret++)
printf("%.2X ", rx[ret]);
printf("\n");
}
int main(int argc, char **argv)
{
char *device = "/dev/spidev0.0";
int fd;
int error;
fd = open(device, O_RDWR);
if (fd < 0)
return pabort("Can't open device ");
error = spi_device_setup(fd);
if (error)
exit (1);
do_transfer(fd);
close(fd);
return 0;
}
```
