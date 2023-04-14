# Bottom Half 


- Critical

Actions such as acknowledging an interrupt to the PIC, reprogramming the PIC
or the device controller, or updating data structures accessed by both the device
and the processor. These can be executed quickly and are critical, because they
must be performed as soon as possible. Critical actions are executed within the
interrupt handler immediately, with maskable interrupts disabled.

- Noncritical

Actions such as updating data structures that are accessed only by the processor
(for instance, reading the scan code after a keyboard key has been pushed).
These actions can also finish quickly, so they are executed by the interrupt han-
dler immediately, with the interrupts enabled.

- Noncritical deferrable

Actions such as copying a buffer’s contents into the address space of a process
(for instance, sending the keyboard line buffer to the terminal handler process).
These may be delayed for a long time interval without affecting the kernel opera-
tions; the interested process will just keep waiting for the data. Noncritical defer-
rable actions are performed by means of separate functions that are discussed in
the later section “Softirqs and Tasklets.”


All bottom half techniques mentioned here are noncritical deferrable

## Softirqs and Tasklets

Softirqs and tasklets are strictly correlated, because tasklets are implemented on top
of softirqs. As a matter of fact, the term “softirq,” which appears in the kernel source
code, often denotes both kinds of deferrable functions. Another widely used term is interrupt context: it specifies that the kernel is currently executing either an interrupt
handler or a deferrable function.

Softirqs are statically allocated (i.e., defined at compile time), while tasklets can also
be allocated and initialized at runtime (for instance, when loading a kernel module).
Softirqs can run concurrently on several CPUs, even if they are of the same type.
Thus, softirqs are reentrant functions and must explicitly protect their data struc-
tures with spin locks. Tasklets do not have to worry about this, because their execu-
tion is controlled more strictly by the kernel. Tasklets of the same type are always
serialized: in other words, the same type of tasklet cannot be executed by two CPUs
at the same time. However, tasklets of different types can be executed concurrently
on several CPUs. Serializing the tasklet simplifies the life of device driver developers,
because the tasklet function needs not be reentrant.


Generally speaking, four kinds of operations can be performed on deferrable functions:
- Initialization

Defines a new deferrable function; this operation is usually done when the ker-
nel initializes itself or a module is loaded.

- Activation

Marks a deferrable function as “pending”—to be run the next time the kernel
schedules a round of executions of deferrable functions. Activation can be done
at any time (even while handling interrupts).

- Masking

Selectively disables a deferrable function so that it will not be executed by the kernel even if activated. We’ll see in the section 

-Execution

Executes a pending deferrable function together with all other pending deferrable functions of the same type; execution is performed at well-specified times,
explained later in the section “Softirqs.”

Activation and execution are bound together: a deferrable function that has been
activated by a given CPU must be executed on the same CPU. There is no self-evi-
dent reason suggesting that this rule is beneficial for system performance. Binding
the deferrable function to the activating CPU could in theory make better use of the
CPU hardware cache. After all, it is conceivable that the activating kernel thread
accesses some data structures that will also be used by the deferrable function. How-
ever, the relevant lines could easily be no longer in the cache when the deferrable
function is run because its execution can be delayed a long time. Moreover, binding
a function to a CPU is always a potentially “dangerous” operation, because one CPU
might end up very busy while the others are mostly idle.


In Linux kernel 5, there are 8 different softirq types, each with a different priority level. These softirq types are:

- HI_SOFTIRQ: This softirq is used for high-priority tasks, such as network processing.

- TIMER_SOFTIRQ: This softirq is used for timer-related tasks, such as scheduling delayed work items.

- NET_TX_SOFTIRQ: This softirq is used for network transmission tasks, such as sending packets.

- NET_RX_SOFTIRQ: This softirq is used for network receive tasks, such as processing incoming packets.

- BLOCK_SOFTIRQ: This softirq is used for block device tasks, such as handling I/O requests.

- IRQ_POLL_SOFTIRQ: This softirq is used for handling deferred interrupt processing.

- TASKLET_SOFTIRQ: This softirq is used for handling tasklets, which are small deferred work items that can be scheduled by interrupt handlers.

- SCHED_SOFTIRQ: This softirq is used for scheduling tasks, such as waking up sleeping processes or running scheduled work items.

Each softirq type has its own priority level, which determines the order in which softirqs are processed when they are scheduled. The priority level is specified using a numeric value, where lower values correspond to higher priority levels. For example, HI_SOFTIRQ has the highest priority level, while SCHED_SOFTIRQ has the lowest priority level. The softirq processing mechanism in the Linux kernel ensures that higher-priority softirqs are processed before lower-priority softirqs, in order to minimize latency and ensure timely processing of time-sensitive tasks.


## Work Queues


