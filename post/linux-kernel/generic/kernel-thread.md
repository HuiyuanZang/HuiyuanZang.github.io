# Kernel Thread

In Linux, kernel threads differ from regular processes in the following ways:
- Kernel threads run only in Kernel Mode, while regular processes run alterna-
tively in Kernel Mode and in User Mode.
- Because kernel threads run only in Kernel Mode, they use only linear addresses
greater than PAGE_OFFSET. Regular processes, on the other hand,

## kernel_thread()


kernel_thread()--->[do_fork(flags|CLONE_VM|CLONE_UNTRACED, 0, pregs, 0, NULL, NULL)](./what-do-fork-does.md);

The CLONE_VM flag avoids the duplication of the page tables of the calling process: this
duplication would be a waste of time and memory, because the new kernel thread
will not access the User Mode address space anyway. The CLONE_UNTRACED flag
ensures that no process will be able to trace the new kernel thread, even if the calling
process is being traced.


The pregs parameter passed to do_fork() corresponds to the address in the Kernel
Mode stack where the copy_thread() function will find the initial values of the CPU registers for the new thread. The kernel_thread() function builds up this stack area
so that:
-  The ebx and edx registers will be set by copy_thread() to the values of the param-
eters fn and arg, respectively.
-  The eip register will be set to the address of the following assembly language
fragment:
``` assembly
movl %edx,%eax
pushl %edx
call *%ebx
pushl %eax
call do_exit
```
Therefore, the new kernel thread starts by executing the fn(arg) function. If this
function terminates, the kernel thread executes the _exit() system call passing to it
the return value of fn()
      

## Process 0

The ancestor of all processes, called process 0, the idle process, or, for historical reasons, the swapper process, is a kernel thread created from scratch during the initial-
ization phase of Linux.This ancestor process uses the following
statically allocated data structures (data structures for all other processes are dynamically allocated):

- A process descriptor stored in the init_task variable, which is initialized by the
INIT_TASK macro.

- A thread_info descriptor and a Kernel Mode stack stored in the init_thread_
union variable and initialized by the INIT_THREAD_INFO macro.

- The following tables, which the process descriptor points to:
— init_mm
— init_fs
— init_files
— init_signals
— init_sighand
The tables are initialized, respectively, by the following macros:
— INIT_MM
— INIT_FS
— INIT_FILES
— INIT_SIGNALS
— INIT_SIGHAND


- The master kernel Page Global Directory stored in swapper_pg_dir


The start_kernel( ) function initializes all the data structures needed by the kernel,
enables interrupts, and creates another kernel thread, named process 1 (more com-
monly referred to as the init process):
kernel_thread(init, NULL, CLONE_FS|CLONE_SIGHAND);

The newly created kernel thread has PID 1 and shares all per-process kernel data
structures with process 0. When selected by the scheduler, the init process starts exe-
cuting the init( ) function.

After having created the init process, process 0 executes the cpu_idle( ) function,
which essentially consists of repeatedly executing the hlt assembly language instruc-
tion with the interrupts enabled.Process 0 is selected by the sched-
uler only when there are no other processes in the TASK_RUNNING state.


In multiprocessor systems there is a process 0 for each CPU. Right after the power-
on, the BIOS of the computer starts a single CPU while disabling the others. The
swapper process running on CPU 0 initializes the kernel data structures, then enables
the other CPUs and creates the additional swapper processes by means of the copy_process() function passing to it the value 0 as the new PID. Moreover, the kernel
sets the cpu field of the thread_info descriptor of each forked process to the proper
CPU index.

## Process 1

The kernel thread created by process 0 executes the init( ) function, which in turn
completes the initialization of the kernel. Then init( ) invokes the execve() system
call to load the executable program init. As a result, the init kernel thread becomes a
regular process having its own per-process kernel data structure.The
init process stays alive until the system is shut down, because it creates and monitors
the activity of all processes that implement the outer layers of the operating system.

## Other kernel theads


- keventd Executes the functions in the keventd_wq workqueue
- kapmd Handles the events related to the Advanced Power Management
- kswapd Reclaims memory, “Periodic Reclaiming”
- pdflush Flushes “dirty” buffers to disk to reclaim memory
- kblockd Executes the functions in the kblockd_workqueue workqueue. Essentially, it peri-
odically activates the block device drivers
- ksoftirqd Runs the tasklets， there is one of these kernel threads for each CPU in the system.