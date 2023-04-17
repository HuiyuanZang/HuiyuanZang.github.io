# Preemptive Kernel Vs Nonpreemptive Kernel

Both in preemptive and nonpreemptive kernels, a process running in Kernel Mode can voluntarily relinquish the CPU, for instance because it has to sleep waiting for
some resource. We will call this kind of process switch a **planned process switch**. However, a preemptive kernel differs from a nonpreemptive kernel on the way a
process running in Kernel Mode reacts to asynchronous events that could induce
a process switch—for instance, an interrupt handler that awakes a higher priority
process. We will call this kind of process switch a **forced process switch**.

All process switches are performed by the switch_to macro. In both preemptive
and nonpreemptive kernels, a process switch occurs when a process has finished
some thread of kernel activity and the scheduler is invoked. However, in nonpre-
emptive kernels, the current process cannot be replaced unless it is about to switch
to User Mode.

Therefore, the main characteristic of a preemptive kernel is that a process running in
Kernel Mode can be replaced by another process while in the middle of a kernel
function.

Let’s give a couple of examples to illustrate the difference between preemptive and
nonpreemptive kernels.

While process A executes an exception handler (necessarily in Kernel Mode), a higher
priority process B becomes runnable. This could happen, for instance, if an IRQ occurs
and the corresponding handler awakens process B. If the kernel is preemptive, a forced
process switch replaces process A with B. The exception handler is left unfinished and
will be resumed only when the scheduler selects again process A for execution. Con-
versely, if the kernel is nonpreemptive, no process switch occurs until process A either
finishes handling the exception handler or voluntarily relinquishes the CPU.

For another example, consider a process that executes an exception handler and
whose time quantum expires “The scheduler_tick( ) Function”. If the kernel is preemptive, the process may be replaced immediately;
however, if the kernel is nonpreemptive, the process continues to run until it finishes
handling the exception handler or voluntarily relinquishes the CPU.

The main motivation for making a kernel preemptive is to reduce the dispatch latency
of the User Mode processes, that is, the delay between the time they become runna-
ble and the time they actually begin running.Processes performing timely scheduled
tasks (such as external hardware controllers, environmental monitors, movie play-
ers, and so on) really benefit from kernel preemption, because it reduces the risk of
being delayed by another process running in Kernel Mode.


kernel preemption is disabled when the preempt_count field in the thread_info descriptor referenced by the current_thread_info() macro is greater than zero. The field encodes
three different counters, so it is greater than
zero when any of the following cases occurs:
1. The kernel is executing an interrupt service routine.
2. The deferrable functions are disabled (always true when the kernel is executing a
softirq or tasklet).
3. The kernel preemption has been explicitly disabled by setting the preemption
counter to a positive value. 


The above rules tell us that the kernel can be preempted only when it is executing an exception handler (in particular a system call) and the kernel preemption has not
been explicitly disabled.

- preempt_count() Selects the preempt_count field in the thread_info descriptor
- preempt_disable() Increases by one the value of the preemption counter
- preempt_enable_no_resched() Decreases by one the value of the preemption counter
- preempt_enable()Decreases by one the value of the preemption counter, and invokes
preempt_schedule() if the TIF_NEED_RESCHED flag in the
thread_info descriptor is set
- get_cpu()Similar to preempt_disable(), but also returns the number of the
local CPU
- put_cpu()Same as preempt_enable()
- put_cpu_no_resched()Same as preempt_enable_no_resched()