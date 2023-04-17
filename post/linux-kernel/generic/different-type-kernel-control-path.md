# Different kernel control path

- exception handlers including kernel-mode proccess
- interrupt handlers
- deferrable functions including softirq and tasklet
- kernel threads including workqueue



A critical region is a section of code
that must be completely executed by the kernel control path that enters it before
another kernel control path can enter it. Interleaving kernel control paths complicates the life of kernel developers: they must
apply special care in order to identify the critical regions in exception handlers, inter-
rupt handlers, deferrable functions, and kernel threads. Once a critical region has been identified, it must be suitably protected to ensure that any time at most one ker-
nel control path is inside that region.

Suppose, for instance, that two different interrupt handlers need to access the same
data structure that contains several related member variables—for instance, a buffer
and an integer indicating its length. All statements affecting the data structure must be
put into a single critical region. If the system includes a single CPU, the critical region
can be implemented by disabling interrupts while accessing the shared data structure,
because nesting of kernel control paths can only occur when interrupts are enabled.


On the other hand, if the same data structure is accessed only by the service routines
of system calls, and if the system includes a single CPU, the critical region can be
implemented quite simply by disabling kernel preemption while accessing the shared
data structure.


All interrupt handlers acknowledge the interrupt on the PIC and also disable the
IRQ line. Further occurrences of the same interrupt cannot occur until the han-
dler terminates.
-  Interrupt handlers, softirqs, and tasklets are both nonpreemptable and non-
blocking, so they cannot be suspended for a long time interval. In the worst case,
their execution will be slightly delayed, because other interrupts occur during
their execution (nested execution of kernel control paths).
-  A kernel control path performing interrupt handling cannot be interrupted by a
kernel control path executing a deferrable function or a system call service routine.
-  Softirqs and tasklets cannot be interleaved on a given CPU.
-  The same tasklet cannot be executed simultaneously on several CPUs.

Each of the above design choices can be viewed as a constraint that can be
exploited to code some kernel functions more easily. Here are a few examples of
possible simplifications:

- Interrupt handlers and tasklets need not to be coded as reentrant functions.
- Per-CPU variables accessed by softirqs and tasklets only do not require synchronization.
- A data structure accessed by only one kind of tasklet does not require synchronization.