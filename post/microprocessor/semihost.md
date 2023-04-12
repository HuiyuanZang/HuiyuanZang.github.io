# Cortex M Semihosting

Semihosting is one of the many interesting features available on ARM Cortex microcontrollers. It allows an embedded program to take advantage of the capabilities of an attached computer when the debugger is running.

## What is Semihosting

According to ARM documentation[1], semihosting is a mechanism that enables code running on an ARM target to communicate and use the Input/Output facilities on a host computer that is running a debugger.
In other words, an ARM based MCU can run C library functions, such as printf(), scanf, or even fopen, and have these interact directly with the host computer attached to the device. By doing so, it can benefit from the screen, the keyboard, or the disk of the host.

## How it works

This is done by halting the CPU target by the debugger agent, either by running into a breakpoint instruction (BKPT 0xAB for ARMv6-M or ARMv7-M) or by sending a supervisor call instruction (SVC 0xAB or SVC 0x123456) depending on the target architecture or processor.This indicates to the host that the target is requesting a semihosting operation.

![](../../asset/microprocessor/semihosting_overview.svg)

The debugger agent then figures out the operation requested by the target by reading the content of r0, and, if necessary, accesses all other function parameters pointed by r1.
While the CPU is still halted, the host will execute the requested operation and return the result in r0 before allowing the processor to continue running its program.
The following is a list of the semihosting operations defined by ARM[2]:
``` C
/* File operations */
SYS_OPEN        EQU 0x01 //Open a file or stream on the host system.
SYS_ISTTY       EQU 0x09 //Check whether a file handle is associated with a file or a stream/terminal such as stdout.
SYS_WRITE       EQU 0x05 //Write to a file or stream.
SYS_READ        EQU 0x06 //Read from a file at the current cursor position.
SYS_CLOSE       EQU 0x02 //Closes a file on the host which has been opened by SYS_OPEN.
SYS_FLEN        EQU 0x0C //Get the length of a file.
SYS_SEEK        EQU 0x0A //Set the file cursor to a given position in a file.
SYS_TMPNAM      EQU 0x0D //Get a temporary absolute file path to create a temporary file.
SYS_REMOVE      EQU 0x0E //Remove a file on the host system. Possibly insecure!
SYS_RENAME      EQU 0x0F //Rename a file on the host system. Possibly insecure!

/* Terminal I/O operations */
SYS_WRITEC      EQU 0x03 //Write one character to the debug terminal.
SYS_WRITE0      EQU 0x04 //Write a 0-terminated string to the debug terminal.
SYS_READC       EQU 0x07 //Read one character from the debug terminal.

/* Time operations */
SYS_CLOCK       EQU 0x10
SYS_ELAPSED     EQU 0x30
SYS_TICKFREQ    EQU 0x31
SYS_TIME        EQU 0x11

/* System/Misc. operations */
SYS_ERRNO       EQU 0x13 //Returns the value of the C library errno variable that is associated with the semihosting implementation.
SYS_GET_CMDLINE EQU 0x15 //Get commandline parameters for the application to run with (argc and argv for main())
SYS_HEAPINFO    EQU 0x16
SYS_ISERROR     EQU 0x08
SYS_SYSTEM      EQU 0x12
```
## How to use it
In this example, I run the program on an STM32 and use the arm-none-eabi toolchain along with openOCD gdbserver. Other tools may be used instead depending on your hardware.
Once you are all set, we can start coding. First, to enable semihosting, you need to link:

- libc.a, the standard C library (newlib3 for arm-none-eabi-gcc),
- librdimon.a, a part of the libgloss4 library (for platform-specific code) to your project.

This is done by adding a few options to the ARM linker:
- removing -nostartfiles flag to allow linking standards libraries,
- adding -lc -lrdimon flags,
- and changing the spec strings file5 to --specs=rdimon.specs to use the semihosted version of the syscalls.

The last point essentially means that the system calls can be used when a debugger is attached (note that the CPU may crash if a debugger is not present).
Now on the C code side, you need to call initialise_monitor_handles() before starting a semihosting operation.

``` C
#ifdef SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

int main(void) {
#ifdef SEMIHOSTING
  	initialise_monitor_handles();
#endif

  	// other tasks ...

#ifdef SEMIHOSTING
    printf("hello world!\n");
    printf("hello world!\n");
#endif

    // other tasks ...

  	return 0;
}

``` 

If you are using crt0 initialization function, initialise_monitor_handles() has already been called for you before main().
Once you have done this, you can compile and program your microcontroller as usual, however, once the microcontroller flashed it should be halted and not run yet.
In my case, I launch OpenOCD gdbserver in one terminal, to flash, reset and halt the CPU. And run gdb from another terminal to connect to the server, and enable the semihosting in the server side6.
``` shell
$ arm-none-eabi-gdb -ex "target extended-remote localhost:3333" -ex "monitor reset halt" -ex "monitor arm semihosting enable"
```
If initialise_monitor_handles() is called before enabling the semihosting in the server, a HardFault may occur due to an unexpected debug event.Once semihosting is enabled, you can run your program from gdb, and the semihosting operation will be handled by the gdbserver.


## Reference
[1] [Arm Semihoting](https://developer.arm.com/documentation/dui0471/i/semihosting/what-is-semihosting-?lang=en)
[2] [Semihosting operations](https://developer.arm.com/documentation/dui0471/i/semihosting/semihosting-operations?lang=en)
[3] [Newlib](https://www.embedded.com/embedding-with-gnu-newlib/)
[4] [gcc spec file](https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Spec-Files.html)
[5] [OpenOCD semihosting commands](https://openocd.org/doc/html/Architecture-and-Core-Commands.html#Architecture-and-Core-Commands)
[6] [Libgloss gcc](https://sca.uwaterloo.ca/coldfire/gcc-doc/docs/porting_1.html)