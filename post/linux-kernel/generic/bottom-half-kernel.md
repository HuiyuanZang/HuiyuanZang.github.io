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

### The ksoftirqd kernel threads


each CPU has its own ksoftirqd/n kernel thread (where n is
the logical number of the CPU). Each ksoftirqd/n kernel thread runs the ksoftirqd()
function, which essentially executes the following loop:
``` C
for(;;) {
    set_current_state(TASK_INTERRUPTIBLE);
    schedule();
    /* now in TASK_RUNNING state */
    while (local_softirq_pending()) {
        preempt_disable();
        do_softirq();
        preempt_enable();
        cond_resched();
   }
}
```

When awakened, the kernel thread checks the local_softirq_pending() softirq bit
mask and invokes, if necessary, do_softirq(). If there are no softirqs pending, the
function puts the current process in the TASK_INTERRUPTIBLE state and invokes then
the cond_resched() function to perform a process switch if required by the current
process (flag TIF_NEED_RESCHED of the current thread_info set).

The ksoftirqd/n kernel threads represent a solution for a critical trade-off problem.

### Tasklets

Tasklets are the preferred way to implement deferrable functions in I/O drivers. As
already explained, tasklets are built on top of two softirqs named HI_SOFTIRQ and
TASKLET_SOFTIRQ. Several tasklets may be associated with the same softirq, each
tasklet carrying its own function. There is no real difference between the two soft-
irqs, except that do_softirq() executes HI_SOFTIRQ’s tasklets before TASKLET_
SOFTIRQ’s tasklets.


Tasklets and high-priority tasklets are stored in the tasklet_vec and tasklet_hi_vec
arrays, respectively. Both of them include NR_CPUS elements of type tasklet_head, and
each element consists of a pointer to a list of tasklet descriptors. The tasklet descrip-
tor is a data structure of type tasklet_struct.


Let’s suppose you’re writing a device driver and you want to use a tasklet: what has
to be done? First of all, you should allocate a new tasklet_struct data structure and
initialize it by invoking tasklet_init(); this function receives as its parameters the
address of the tasklet descriptor, the address of your tasklet function, and its
optional integer argument.


The tasklet may be selectively disabled by invoking either tasklet_disable_nosync()
or tasklet_disable(). Both functions increase the count field of the tasklet descrip-
tor, but the latter function does not return until an already running instance of the
tasklet function has terminated. To reenable the tasklet, use tasklet_enable().


To activate the tasklet, you should invoke either the tasklet_schedule() function or
the tasklet_hi_schedule() function, according to the priority that you require for
the tasklet.

## Work Queues

The main difference is that deferrable functions run in interrupt context while
functions in work queues run in process context.No
process switch can take place in interrupt context. Neither deferrable functions nor
functions in a work queue can access the User Mode address space of a process.In
fact, a deferrable function cannot make any assumption about the process that is
currently running when it is executed. On the other hand, a function in a work
queue is executed by a kernel thread, so there is no User Mode address space to
access.


The main data structure associated with a work queue is a descriptor called
**workqueue_struct**, which contains, among other things, an array of NR_CPUS elements,
the maximum number of CPUs in the system.Each element is a descriptor of type
**cpu_workqueue_struct**, whose fields are
```C
struct cpu_workqueue_struct {
    struct workqueue_struct *wq; //Pointer to the parent workqueue_struct of this per-CPU workqueue.
    struct list_head worklist; // List of pending work items in this per-CPU workqueue.
    spinlock_t lock;  //pinlock used to protect access to the worklist.
    wait_queue_head_t more_work;//Wait queue used to notify the per-CPU workqueue thread that there is more work to do.
    struct task_struct *thread;//Pointer to the kernel thread that runs this per-CPU workqueue.
    int thread_started;//Flag that indicates whether the per-CPU workqueue thread has been started.
    bool frozen;// Flag that indicates whether the per-CPU workqueue is frozen (i.e., not accepting new work items).
    int nr_unbound;//Number of unbound work items in this per-CPU workqueue.
    int nice; //Nice value of the per-CPU workqueue thread.
    struct rcu_head rcu;//RCU (read-copy-update) head used to synchronize freeing of the 
};
```

The create_workqueue("foo") function receives as its parameter a string of charac-
ters and returns the address of a workqueue_struct descriptor for the newly created work queue. The function also creates n worker threads (where n is the number of
CPUs effectively present in the system), named after the string passed to the func-
tion: foo/0, foo/1, and so on. The create_singlethread_workqueue() function is simi-
lar, but it creates just one worker thread, no matter what the number of CPUs in the
system is. To destroy a work queue the kernel invokes the destroy_workqueue() func-
tion, which receives as its parameter a pointer to a workqueue_struct array.

queue_work() inserts a function (already packaged inside a work_struct descriptor) in
a work queue; it receives a pointer wq to the workqueue_struct descriptor and a
pointer work to the work_struct descriptor.The queue_delayed_work() function is nearly identical to queue_work(), except that it
receives a third parameter representing a time delay in system ticks

Every worker thread continuously executes a loop inside the worker_thread() func-
tion; most of the time the thread is sleeping and waiting for some work to be queued.
Once awakened, the worker thread invokes the run_workqueue() function, which
essentially removes every work_struct descriptor from the work queue list of the
worker thread and executes the corresponding pending function. Because work
queue functions can block, the worker thread can be put to sleep and even migrated
to another CPU when resumed.*
Sometimes the kernel has to wait until all pending functions in a work queue have
been executed. The flush_workqueue() function receives a workqueue_struct descrip-
tor address and blocks the calling process until all functions that are pending in the
work queue terminate. The function, however, does not wait for any pending func-
tion that was added to the work queue following flush_workqueue() invocation; the 
remove_sequence and insert_sequence fields of every cpu_workqueue_struct descrip-
tor are used to recognize the newly added pending functions.

In most cases, creating a whole set of worker threads in order to run a function is
overkill. Therefore, the kernel offers a predefined work queue called events, which
can be freely used by every kernel developer. The predefined work queue is nothing
more than a standard work queue that may include functions of different kernel lay-
ers and I/O drivers; its workqueue_struct descriptor is stored in the keventd_wq array.