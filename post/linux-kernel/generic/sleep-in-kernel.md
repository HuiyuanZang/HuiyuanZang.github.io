# Delays and sleep in the kernel
Without going into the details, there are two types of delays, depending on the context your
code runs in: atomic or nonatomic. The mandatory header to handle delays in the kernel is
``` C
# include <linux/delay> .
```
## Atomic context
Tasks in the atomic context (such as ISR) can't sleep, and can't be scheduled; it is the reason
why busy-wait loops are used for delaying purposes in an atomic context. The kernel
exposes the Xdelay family of functions that will spend time in a busy loop, long
enough(based on jiffies) to achieve the desired delay:
``` C
ndelay(unsigned long nsecs)
udelay(unsigned long usecs)
mdelay(unsigned long msecs)
``` 

You should always use udelay() since ndelay() precision depends on how accurate your
hardware timer is (not always the case on an embedded SOC). Use of mdelay() is also
discouraged.
Timer handlers (callbacks) are executed in an atomic context, meaning that sleeping is not
allowed at all. By sleeping, I mean any function that may result in sending the caller to sleep,
such as allocating memory, locking a mutex, an explicit call to the sleep() function, and so
on.
## Nonatomic context
In a nonatomic context, the kernel provides the sleep[_range] family of functions and
which function to use depends on how long you need to delay by:
- udelay(unsigned long usecs) : Busy-wait loop-based. You should use this function if you need to sleep for a few μsecs ( < ~10 us ).
- usleep_range(unsigned long min, unsigned long max) : Relies on hrtimers, and it is recommended to let this sleep for a few ~μsecs or small msecs (10 us - 20 ms), avoiding the busy-wait loop of udelay() .
- msleep(unsigned long msecs) : Backed by jiffies/legacy_timers. You should use this for larger, msecs sleep (10 ms+).

Sleep and delay topics are well explained in
Documentation/timers/timers-howto.txt in the kernel source.
