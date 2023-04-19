# Scheduling Algorithm

The scheduler always succeeds in finding a process to be executed; in fact, there is
always at least one runnable process: the swapper process, which has PID 0 and exe-
cutes only when the CPU cannot execute other processes.Every CPU of a multiprocessor system has its own swapper process with PID equal to 0.Every Linux process is always scheduled according to one of the following scheduling classes:

- SCHED_FIFO

  A First-In, First-Out real-time process. When the scheduler assigns the CPU to
the process, it leaves the process descriptor in its current position in the run-
queue list. If no other higher-priority real-time process is runnable, the process
continues to use the CPU as long as it wishes, even if other real-time processes
that have the same priority are runnable.

- SCHED_RR

  A Round Robin real-time process. When the scheduler assigns the CPU to the
process, it puts the process descriptor at the end of the runqueue list. This pol-
icy ensures a fair assignment of CPU time to all SCHED_RR real-time processes that
have the same priority.

- SCHED_NORMAL

  This is the default scheduling algorithm used by Linux, and it uses a dynamic priority system to schedule processes. Processes are assigned a priority based on their resource usage and history. Higher priority processes are given more CPU time than lower priority processes. This scheduling algorithm is suitable for most general-purpose applications.

- SCHED_BATCH

  This is a scheduling algorithm that is optimized for batch processing jobs. Processes are scheduled in batches, which reduces context switching overhead and improves overall system performance.

- SCHED_IDLE

  This is a scheduling algorithm that is used for low-priority background tasks. Processes are only scheduled to run when there are no other higher priority tasks to execute.

- SCHED_DEADLINE(since Linux 4.13, )

 This is a real-time scheduling algorithm that guarantees that a process will complete its task within a specific deadline. It is often used for tasks that require precise timing, such as multimedia applications or control systems.

 ## Scheduling of conventional process(SCHED_NORMAL)

Every conventional process has its own static priority, which is a value used by the
scheduler to rate the process with respect to the other conventional processes in the
system. The kernel represents the static priority of a conventional process with a
number ranging from 100 (highest priority) to 139 (lowest priority); notice that static
priority decreases as the values increase.

A new process always inherits the static priority of its parent. However, a user can
change the static priority of the processes that he owns by passing some “nice val-
ues” to the nice( ) and setpriority( ) system calls

### Base time quantum

base time quantum (in milliseconds) = (140 - static priority)*20 if static priority < 120

base time quantum (in milliseconds) = (140 - static priority)*5 if static priority >= 120


|Description|Static priority|Nice value|Base time quantum|Interactivedelta|Sleep time threshold|
|   ---     |     ---       |     ---  |     ---         |   ---           | ---               |
|  Highest static priority  |     100       |     -20  |     800 ms         |   -3           | 299ms              |
|  High static priority  |     110       |    -10  |     600 ms         |   -1           | 499ms              |
|  Default static priority  |     120       |     0  |     100 ms         |   +2           | 799ms               |
|  Low static priority  |     130       |    +10  |     50ms        |   +4           | 999ms               |
|  Lowest static priority  |     139       |     +19  |     5 ms         |   +6           | 1199ms              |


### Dynamic priority and average sleep time

Besides a static priority, a conventional process also has a dynamic priority, which is
a value ranging from 100 (highest priority) to 139 (lowest priority). The dynamic pri-
ority is the number actually looked up by the scheduler when selecting the new pro-
cess to run. It is related to the static priority by the following empirical formula:

      dynamic priority = max( 100, min( static priority − bonus + 5, 139 )) 

The bonus is a value ranging from 0 to 10; a value less than 5 represents a penalty that
lowers the dynamic priority, while a value greater than 5 is a premium that raises the
dynamic priority. The value of the bonus, in turn, depends on the past history of the
process; more precisely, it is related to the average sleep time of the process.

Roughly, the average sleep time is the average number of nanoseconds that the pro-
cess spent while sleeping. Be warned, however, that this is not an average operation
on the elapsed time. For instance, sleeping in TASK_INTERRUPTIBLE state contributes to
the average sleep time in a different way from sleeping in TASK_UNINTERRUPTIBLE state.

Moreover, the average sleep time decreases while a process is running. Finally, the
average sleep time can never become larger than 1 second.

|  Average sleep time      | Bonus  |  Granularity |
|   ---     |     ---       |     ---  |
|Greater than or equal to 0 but smaller than 100 ms| 0 | 5120 |
|Greater than or equal to 100 ms but smaller than 200 ms| 1 | 2560 |
|Greater than or equal to 200 but smaller than 300 ms| 2 | 1280 |
|Greater than or equal to 300 but smaller than 400 ms| 3 | 640 |
|Greater than or equal to 400 but smaller than 500 ms| 4 | 320 |
|Greater than or equal to 500 but smaller than 600 ms| 5 | 160 |
|Greater than or equal to 600 but smaller than 700 ms| 6 | 80 |
|Greater than or equal to 700 but smaller than 800 ms| 7 | 40 |
|Greater than or equal to 800 but smaller than 900 ms| 8 | 20 |
|Greater than or equal to 900 but smaller than 1000 ms| 9 | 10 |
|1 Second| 10 | 10 |



The average sleep time is also used by the scheduler to determine whether a given
process should be considered interactive or batch. More precisely, a process is con-
sidered “interactive” if it satisfies the following formula:

    dynamic priority ≤ 3 × static priority / 4 + 28

which is equivalent to the following:
   
    bonus - 5 ≥ static priority / 4 − 28

The expression static priority / 4 − 28 is called the interactive delta;It should be noted that it is far easier for high priority than for low priority processes to become interactive. For instance, a process hav-
ing highest static priority (100) is considered interactive when its bonus value exceeds
2, that is, when its average sleep time exceeds 200 ms. Conversely, a process having
lowest static priority (139) is never considered as interactive, because the bonus value
is always smaller than the value 11 required to reach an interactive delta equal to 6. A
process having default static priority (120) becomes interactive as soon as its average
sleep time exceeds 700 ms.


### Active and expired processes


Even if conventional processes having higher static priorities get larger slices of the CPU
time, they should not completely lock out the processes having lower static priority. To
avoid process starvation, when a process finishes its time quantum, it can be replaced
by a lower priority process whose time quantum has not yet been exhausted. To imple-
ment this mechanism, the scheduler keeps two disjoint sets of runnable processes:

- Active processes
These runnable processes have not yet exhausted their time quantum and are
thus allowed to run.
- Expired processes
These runnable processes have exhausted their time quantum and are thus for-
bidden to run until all active processes expire.


However, the general schema is slightly more complicated than this, because the
scheduler tries to boost the performance of interactive processes. An active batch pro-
cess that finishes its time quantum always becomes expired. An active interactive pro-
cess that finishes its time quantum usually remains active: the scheduler refills its time
quantum and leaves it in the set of active processes. However, the scheduler moves an
interactive process that finished its time quantum into the set of expired processes if
the eldest expired process has already waited for a long time, or if an expired process
has higher static priority (lower value) than the interactive process. As a consequence,
the set of active processes will eventually become empty and the expired processes
will have a chance to run.


 ## Scheduling of Real-TIME process(SCHED_FIFO,SCHED_RR, SCHED_DEADLINE )

Every real-time process is associated with a real-time priority, which is a value rang-
ing from 1 (highest priority) to 99 (lowest priority). The scheduler always favors a
higher priority runnable process over a lower priority one; in other words, a real-time process inhibits the execution of every lower-priority process while it remains runnable.Contrary to conventional processes, real-time processes are always considered active (see the previous section). The user can change the real-time priority of a pro-
cess by means of the sched_setparam() and sched_setscheduler() system calls
A real-time process is replaced by another process only when one of the following
events occurs:
- The process is preempted by another process having higher real-time priority.
- The process performs a blocking operation, and it is put to sleep (in state TASK_
INTERRUPTIBLE or TASK_UNINTERRUPTIBLE).
- The process is stopped (in state TASK_STOPPED or TASK_TRACED), or it is killed (in
state EXIT_ZOMBIE or EXIT_DEAD).
- The process voluntarily relinquishes the CPU by invoking the sched_yield()
system call
- The process is Round Robin real-time (SCHED_RR), and it has exhausted its time
quantum.

The nice() and setpriority() system calls, when applied to a Round Robin real-
time process, do not change the real-time priority but rather the duration of the base
time quantum. In fact, the duration of the base time quantum of Round Robin real-
time processes does not depend on the real-time priority, but rather on the static pri-
ority of the process,


## Example 1

To fork a new process in Linux and set its static priority less than 100 and SCHED_RR scheduling policy, you can use the fork() and sched_setscheduler() system calls.

``` C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

int main() {
    int pid = fork();

    if (pid == 0) { // Child process
        // Set the scheduling policy to SCHED_RR and priority to 50
        struct sched_param param;
        param.sched_priority = 50;
        if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
            perror("sched_setscheduler failed");
            exit(EXIT_FAILURE);
        }

        // Child process code goes here
        printf("Child process running with static priority %d and SCHED_RR scheduling policy.\n", param.sched_priority);
        sleep(10);
        printf("Child process exiting.\n");
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0) { // Parent process
        printf("Forked child process with PID %d.\n", pid);
        exit(EXIT_SUCCESS);
    }
    else { // Fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

```

Note that in the sched_setscheduler() call, the first parameter 0 specifies that we want to set the scheduling policy for the current process (i.e., the child process). If you want to set the scheduling policy for a specific process identified by its process ID, you can pass that process ID as the first parameter instead of 0.

Also, keep in mind that setting the static priority of a process too low may cause it to be preempted frequently by higher-priority processes, which can negatively impact its performance. So be careful when setting the static priority of a process.


## Example 2

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

void* thread_func(void* arg) {
    // Set the scheduling policy to SCHED_RR and priority to 50
    struct sched_param param;
    param.sched_priority = 50;
    if (pthread_setschedparam(pthread_self(), SCHED_RR, &param) != 0) {
        perror("pthread_setschedparam failed");
        exit(EXIT_FAILURE);
    }

    // Thread code goes here
    printf("Thread running with static priority %d and SCHED_RR scheduling policy.\n", param.sched_priority);
    sleep(10);
    printf("Thread exiting.\n");
    return NULL;
}

int main() {
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, thread_func, NULL);
    if (ret != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }
    printf("Created thread with ID %lu.\n", thread);

    pthread_join(thread, NULL);
    printf("Thread joined.\n");

    return 0;
}
```
In this code, we create a new thread using pthread_create(). In the thread function, we set the scheduling policy to SCHED_RR and the priority to 50 using the pthread_setschedparam() function. We then run some code to demonstrate that the thread is indeed running with the desired scheduling policy and priority. Finally, we exit the thread function using return.

Note that in the pthread_setschedparam() call, the first parameter pthread_self() specifies that we want to set the scheduling policy for the current thread (i.e., the thread that is currently executing the function). If you want to set the scheduling policy for a specific thread identified by its thread ID, you can pass that thread ID as the first parameter instead of pthread_self().

Also, keep in mind that setting the static priority of a thread too low may cause it to be preempted frequently by higher-priority threads, which can negatively impact its performance. So be careful when setting the static priority of a thread.

## Example 3

``` C
#include <linux/kthread.h>
#include <linux/sched.h>

static struct task_struct *my_thread;

int my_thread_function(void *arg) {
    // Set the scheduling policy to SCHED_RR and priority to 50
    struct sched_param param;
    param.sched_priority = 50;
    sched_setscheduler(current, SCHED_RR, &param);

    // Kernel thread code goes here
    printk(KERN_INFO "Kernel thread running with static priority %d and SCHED_RR scheduling policy.\n", param.sched_priority);
    msleep(10000);
    printk(KERN_INFO "Kernel thread exiting.\n");
    return 0;
}

int init_module(void) {
    // Create a new kernel thread
    my_thread = kthread_create(my_thread_function, NULL, "my_thread");
    if (IS_ERR(my_thread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(my_thread);
    }

    // Set the priority of the kernel thread to 50
    set_user_nice(my_thread, -20);

    // Wake up the kernel thread and let it start running
    wake_up_process(my_thread);

    return 0;
}

void cleanup_module(void) {
    // Stop and destroy the kernel thread
    kthread_stop(my_thread);
}

```

In this code, we create a new kernel thread using kthread_create() and pass a function pointer to it as the first argument. In the function, we set the scheduling policy to SCHED_RR and the priority to 50 using the sched_setscheduler() function.

We also set the priority of the kernel thread to -20 using the set_user_nice() function. The set_user_nice() function is used to adjust the priority of the thread in the range of -20 to 19, with lower values indicating higher priority.

Finally, we wake up the kernel thread using wake_up_process() to let it start running. In the cleanup_module() function, we stop and destroy the kernel thread using kthread_stop().

Note that setting the priority of a kernel thread too low may cause it to be preempted frequently by higher-priority threads, which can negatively impact its performance. So be careful when setting the priority of a kernel thread.