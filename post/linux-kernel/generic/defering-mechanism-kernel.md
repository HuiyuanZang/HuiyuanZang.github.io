# Work deferring mechanism
Deferring is a method by which you schedule a piece of work to be executed in the future.
It's a way to report an action later. Obviously, the kernel provides facilities to implement
such a mechanism; it allows you to defer functions, whatever their type, to be called and
executed later. There are three of them in the kernel:
- SoftIRQs: Executed in an atomic context
- Tasklets: Executed in an atomic context
- Workqueues: Executed in a process context

## Softirqs and ksoftirqd
A software IRQ (softirq), or software interrupt is a deferring mechanism used only for
very fast processing, since it runs with a disabled scheduler (in an interrupt context). You'll
rarely (almost never) want to deal with softirq directly. There are only networks and block
device subsystems using softirq. Tasklets are an instantiation of softirqs, and will be
sufficient in almost every case when you feel the need to use softirqs.

### ksoftirqd
In most cases, softirqs are scheduled in hardware interrupts, which may arrive very
quickly, faster than they can be serviced. They are then queued by the kernel in order to be
processed later. Ksoftirqds are responsible for late execution (process context this time). A
ksoftirqd is a per-CPU kernel thread raised to handle unserved software interrupts:


In the preceding top sample from my personal computer, you can see ksoftirqd/n
entries, where n is the CPU number that the ksoftirqds runs on. CPU-consuming ksoftirqd
may indicate an overloaded system or a system under interrupts storm, which is never
good. You can have a look at kernel/softirq.c to see how ksoftirqds are designed.


## Tasklets
Tasklets are a bottom-half (we will see what this means later) mechanism built on top of
softirqs. They are represented in the kernel as instances of the struct tasklet_struct :
``` C
struct tasklet_struct
{
    struct tasklet_struct *next;
    unsigned long state;
    atomic_t count;
    void (*func)(unsigned long);
    unsigned long data;
};
```
Tasklets are not re-entrant by nature. Code is called reentrant if it can be interrupted
anywhere in the middle of its execution, and then be safely called again. Tasklets are
designed such that a tasklet can run on one and only one CPU simultaneously (even on an
SMP system), which is the CPU it was scheduled on, but different tasklets may be run
simultaneously on different CPUs. The tasklet API is quite basic and intuitive.


### Tasklets Example


``` C
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
/* for tasklets API */
char tasklet_data[]="We use a string; but it could be pointer to a
structure";
/* Tasklet handler, that just print the data */
void tasklet_work(unsigned long data)
{
    printk("%s\n", (char *)data);
}
DECLARE_TASKLET(my_tasklet, tasklet_function, (unsigned long) tasklet_data);
static int __init my_init(void)
{
    tasklet_schedule(&my_tasklet);
    return 0;
}
void my_exit(void)
{
    tasklet_kill(&my_tasklet);
}
module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("John Madieu <john.madieu@gmail.com>");
MODULE_LICENSE("GPL")
```

NOTE:

- Calling tasklet_schedule on a tasklet already scheduled, but whose execution has not started, will do nothing, resulting in the tasklet being executed only once.
- tasklet_schedule can be called in a tasklet, meaning that a tasklet can reschedule itself.

## Work queues

Added since Linux kernel 2.6, the most used and simple deferring mechanism is the work
queue. It is the last one we will talk about in this chapter. As a deferring mechanism, it
takes an opposite approach to the others we've seen, running only in a pre-emptible
context. It is the only choice when you need to sleep in your bottom half. By sleep, I mean process I/O data, hold
mutexes, delay, and all the other tasks that may lead to sleep or move the task off the run
queue.
Keep in mind that work queues are built on top of kernel threads, and this is the reason
why I decided not to talk about the kernel thread as a deferring mechanism at all. However,
there are two ways to deal with work queues in the kernel. First, there is a default shared
work queue, handled by a set of kernel threads, each running on a CPU. Once you have
work to schedule, you queue that work into the global work queue, which will be executed
at the appropriate moment. The other method is to run the work queue in a dedicated
kernel thread. It means whenever your work queue handler needs to be executed, your
kernel thread is woken up to handle it, instead of one of the default predefined threads.
Structures and functions to call are different, depending on whether you chose a shared
work queue or dedicated ones.

### Kernel-global work queue – the shared queue

Unless you have no choice, you need critical performance, or you need to control
everything from work queue initialization to work scheduling, and if you only submit tasks
occasionally, you should use the shared work queue provided by the kernel. With that
queue being shared over the system, you should be nice, and should not monopolize the
queue for a long time.

Since the execution of the pending task on the queue is serialized on each CPU, you should
not sleep for a long time because no other task on the queue will run until you wake up.
You won't even know who you share the work queue with, so don't be surprised if your
task takes longer to get the CPU. Work in shared work queues is executed in a per-CPU
thread called events/n, created by the kernel.
In this case, the work must also be initialized with the INIT_WORK macro. Since we are
going to use the shared work queue, there is no need to create a work queue structure. We
only need the work_struct structure that will be passed as an argument. There are three
functions to schedule work on the shared work queue:


- The version that ties the work on the current CPU:

    int schedule_work(struct work_struct *work);

- The same, but delayed, function:

    static inline bool schedule_delayed_work(struct delayed_work *dwork,unsigned long delay);


- The function that actually schedules the work on a given CPU:

    int schedule_work_on(int cpu, struct work_struct *work);

- The same as shown previously, but with a delay:

    int scheduled_delayed_work_on(int cpu, struct delayed_work*dwork, unsigned long delay);

#### Example

``` C
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
/* for sleep */
#include <linux/wait.h>
/* for wait queue */
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/slab.h>
/* for kmalloc() */
#include  <linux/workqueue.h>
//static DECLARE_WAIT_QUEUE_HEAD(my_wq);
static int sleep = 0;
struct work_data {
struct work_struct my_work;
wait_queue_head_t my_wq;
    int the_data;
};
static void work_handler(struct work_struct *work)
{
    struct work_data *my_data = container_of(work, \
    struct work_data, my_work);
    printk("Work queue module handler: %s, data is %d\n", __FUNCTION__,my_data->the_data);
    msleep(2000);
    wake_up_interruptible(&my_data->my_wq);
    kfree(my_data);
}
static int __init my_init(void)
{
    struct work_data * my_data;
    my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
    my_data->the_data = 34;
    INIT_WORK(&my_data->my_work, work_handler);
    init_waitqueue_head(&my_data->my_wq);
    schedule_work(&my_data->my_work);
    printk("I'm going to sleep ...\n");
    wait_event_interruptible(my_data->my_wq, sleep != 0);
    printk("I am Waked up...\n");
    return 0;
}
static void __exit my_exit(void)
{
    printk("Work queue module exit: %s %d\n", __FUNCTION__,__LINE__);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Madieu <john.madieu@gmail.com> ");
MODULE_DESCRIPTION("Shared workqueue");

```

### Dedicated work queue

Here, the work queue is represented as an instance of struct workqueue_struct . The
work to be queued into the work queue is represented as an instance of struct
work_struct . There are four steps involved prior to scheduling your work in your own
kernel thread:

1. Declare/initialize a struct workqueue_struct
2. Create your work function
3. Create a struct work_struct so that your work function will be embedded
into it
4. Embed your work function in the work_struct

#### Example


``` C
#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include  <linux/slab.h>
/* for work queue */
/* for kmalloc() */

struct workqueue_struct *wq;
struct work_data {
    struct work_struct my_work;
    int the_data;
};
static void work_handler(struct work_struct *work)
{
    struct work_data * my_data = container_of(work,
    struct work_data, my_work);
    printk("Work queue module handler: %s, data is %d\n",__FUNCTION__, my_data->the_data);
    kfree(my_data);
}
static int __init my_init(void)
{
    struct work_data * my_data;
    printk("Work queue module init: %s %d\n", __FUNCTION__, __LINE__);
    wq = create_singlethread_workqueue("my_single_thread");
    my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);
    my_data->the_data = 34;
    INIT_WORK(&my_data->my_work, work_handler);
    queue_work(wq, &my_data->my_work);
    return 0;
}
static void __exit my_exit(void)
{
    flush_workqueue(wq);
    destroy_workqueue(wq);
    printk("Work queue module exit: %s %d\n", __FUNCTION__, __LINE__);
}
module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Madieu <john.madieu@gmail.com>");

```