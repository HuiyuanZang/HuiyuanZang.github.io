## Dynamic tick/tickless kernel
With the previous HZ options, the kernel is interrupted HZ times per second in order to
reschedule tasks, even in an idle state. If HZ is set to 1,000, there will be 1,000 kernel
interruptions per second, preventing the CPU from being idle for a long time, thus affecting
CPU power consumption.
Now, let's look at a kernel with no fixed or predefined ticks, where the ticks are disabled
until some task needs to be performed. We call such a kernel a tickless kernel. In fact, tick
activation is scheduled, based on the next action. The right name should be dynamic tick
kernel. The kernel is responsible for task scheduling, and maintains a list of runnable tasks
(the run queue) in the system. When there is no task to schedule, the scheduler switches to
the idle thread, which enables dynamic tick by disabling periodic tick until the next timer
expires (a new task is queued for processing).
Under the hood, the kernel also maintains a list of the task timeouts (it then knows when
and how long it has to sleep). In an idle state, if the next tick is further away than the lowest
timeout in the tasks list timeout, the kernel programs the timer with that timeout value.
When the timer expires, the kernel re-enables periodic ticks and invokes the scheduler,
which then schedules the task associated with the timeout. This is how the tickless kernel
removes periodic ticks and saves power when idle.
