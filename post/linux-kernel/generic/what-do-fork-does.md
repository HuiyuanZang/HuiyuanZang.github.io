# what does do_fork() and copy_process() does?

## do_fork()
1. Allocates a new PID for the child by looking in the pidmap_array bitmap .

2. Checks the ptrace field of the parent (current->ptrace): if it is not zero, the par-
ent process is being traced by another process, thus do_fork() checks whether the debugger wants to trace the child on its own (independently of the value of
the CLONE_PTRACE flag specified by the parent); in this case, if the child is not a kernel thread (CLONE_UNTRACED flag cleared), the function sets the CLONE_PTRACE flag.

3. Invokes copy_process() to make a copy of the process descriptor. If all needed
resources are available, this function returns the address of the task_struct
descriptor just created. This is the workhorse of the forking procedure, and we
will describe it right after do_fork().

4. If either the CLONE_STOPPED flag is set or the child process must be traced, that is,
the PT_PTRACED flag is set in p->ptrace, it sets the state of the child to TASK_
STOPPED and adds a pending SIGSTOP signal to it. The state of the child will remain TASK_STOPPED until
another process (presumably the tracing process or the parent) will revert its
state to TASK_RUNNING, usually by means of a SIGCONT signal.

5. If the CLONE_STOPPED flag is not set, it invokes the wake_up_new_task() function,
which performs the following operations:

    a. Adjusts the scheduling parameters of both the parent and the child.

    b. If the child will run on the same CPU as the parent,* and parent and child do
not share the same set of page tables (CLONE_VM flag cleared), it then forces
the child to run before the parent by inserting it into the parent’s runqueue
right before the parent. This simple step yields better performance if the
child flushes its address space and executes a new program right after the
forking. If we let the parent run first, the Copy On Write mechanism would
give rise to a series of unnecessary page duplications.

    c. Otherwise, if the child will not be run on the same CPU as the parent, or if
parent and child share the same set of page tables (CLONE_VM flag set), it
inserts the child in the last position of the parent’s runqueue.

6. If the CLONE_STOPPED flag is set, it puts the child in the TASK_STOPPED state.

7. If the parent process is being traced, it stores the PID of the child in the ptrace_
message field of current and invokes ptrace_notify(), which essentially stops the
current process and sends a SIGCHLD signal to its parent. The “grandparent” of
the child is the debugger that is tracing the parent; the SIGCHLD signal notifies the
debugger that current has forked a child, whose PID can be retrieved by looking
into the current->ptrace_message field.

8. If the CLONE_VFORK flag is specified, it inserts the parent process in a wait queue
and suspends it until the child releases its memory address space

9. Terminates by returning the PID of the child.

## copy_process()

The copy_process() function sets up the process descriptor and any other kernel data
structure required for a child’s execution. Its parameters are the same as do_fork(),
plus the PID of the child. Here is a description of its most significant steps:

1. Checks whether the flags passed in the clone_flags parameter are compatible. In
particular, it returns an error code in the following cases:

   a. Both the flags CLONE_NEWNS and CLONE_FS are set.

   b. The CLONE_THREAD flag is set, but the CLONE_SIGHAND flag is cleared (light-
weight processes in the same thread group must share signals).

   c. The CLONE_SIGHAND flag is set, but the CLONE_VM flag is cleared (lightweight
processes sharing the signal handlers must also share the memory descriptor).

2. Performs any additional security checks by invoking security_task_create()
and, later, security_task_alloc(). The Linux kernel 2.6 offers hooks for secu-
rity extensions that enforce a security model stronger than the one adopted by
traditional Unix.


3. Invokes dup_task_struct() to get the process descriptor for the child. This func-
tion performs the following actions:

    a. Invokes _ _unlazy_fpu() on the current process to save, if necessary, the
contents of the FPU, MMX, and SSE/SSE2 registers in the thread_info struc-
ture of the parent. Later, dup_task_struct() will copy these values in the
thread_info structure of the child.

    b. Executes the alloc_task_struct() macro to get a process descriptor (task_
struct structure) for the new process, and stores its address in the tsk local
variable.

    c. Executes the alloc_thread_info macro to get a free memory area to store the
thread_info structure and the Kernel Mode stack of the new process, and
saves its address in the ti local variable. As explained in the earlier section
“Identifying a Process,” the size of this memory area is either 8 KB or 4 KB.


    d. Copies the contents of the current’s process descriptor into the task_struct
structure pointed to by tsk, then sets tsk->thread_info to ti.


    e. Copies the contents of the current’s thread_info descriptor into the struc-
ture pointed to by ti, then sets ti->task to tsk.

    f. Sets the usage counter of the new process descriptor (tsk->usage) to 2 to
specify that the process descriptor is in use and that the corresponding pro-
cess is alive (its state is not EXIT_ZOMBIE or EXIT_DEAD).
    g. Returns the process descriptor pointer of the new process (tsk).

4. Checks whether the value stored in current->signal->rlim[RLIMIT_NPROC].rlim_
cur is smaller than or equal to the current number of processes owned by the
user. If so, an error code is returned, unless the process has root privileges. The function gets the current number of processes owned by the user from a per-user
data structure named user_struct. This data structure can be found through a
pointer in the user field of the process descriptor.

5. Increases the usage counter of the user_struct structure (tsk->user->_ _count
field) and the counter of the processes owned by the user (tsk->user->
processes).

6. Checks that the number of processes in the system (stored in the nr_threads
variable) does not exceed the value of the max_threads variable. The default value
of this variable depends on the amount of RAM in the system. The general rule
is that the space taken by all thread_info descriptors and Kernel Mode stacks
cannot exceed 1/8 of the physical memory. However, the system administrator
may change this value by writing in the /proc/sys/kernel/threads-max file.

7. If the kernel functions implementing the execution domain and the executable
format of the new process are included in kernel modules, it
increases their usage counters .

8. Sets a few crucial fields related to the process state:

    a. Initializes the big kernel lock counter tsk->lock_depth to -1 

    b. Initializes the tsk->did_exec field to 0: it counts the number of execve() system calls issued by the process.

    c. Updates some of the flags included in the tsk->flags field that have been
copied from the parent process: first clears the PF_SUPERPRIV flag, which
indicates whether the process has used any of its superuser privileges, then
sets the PF_FORKNOEXEC flag, which indicates that the child has not yet issued
an execve() system call.

9. Stores the PID of the new process in the tsk->pid field.

10. If the CLONE_PARENT_SETTID flag in the clone_flags parameter is set, it copies the
child’s PID into the User Mode variable addressed by the parent_tidptr parameter.


11. Initializes the list_head data structures and the spin locks included in the child’s
process descriptor, and sets up several other fields related to pending signals,
timers, and time statistics.

12. Invokes copy_semundo(), copy_files( ), copy_fs( ), copy_sighand( ), copy_
signal(), copy_mm( ), and copy_namespace( ) to create new data structures and
copy into them the values of the corresponding parent process data structures,
unless specified differently by the clone_flags parameter.

13. Invokes copy_thread( ) to initialize the Kernel Mode stack of the child process
with the values contained in the CPU registers when the clone( ) system call was
issued (these values have been saved in the Kernel Mode stack of the parent). However, the function forces the value 0 into the field corresponding to the eax register (this is the child’s return value of the fork() or clone() system call). The thread.esp field in the descriptor of the child process is
initialized with the base address of the child’s Kernel Mode stack, and the address
of an assembly language function (ret_from_fork( )) is stored in the thread.eip
field. If the parent process makes use of an I/O Permission Bitmap, the child gets
a copy of such bitmap. Finally, if the CLONE_SETTLS flag is set, the child gets the
TLS segment specified by the User Mode data structure pointed to by the tls
parameter of the clone() system call.*

14. If either CLONE_CHILD_SETTID or CLONE_CHILD_CLEARTID is set in the clone_flags
parameter, it copies the value of the child_tidptr parameter in the tsk->set_
chid_tid or tsk->clear_child_tid field, respectively. These flags specify that the
value of the variable pointed to by child_tidptr in the User Mode address space
of the child has to be changed, although the actual write operations will be done
later.

15. Turns off the TIF_SYSCALL_TRACE flag in the thread_info structure of the child, so
that the ret_from_fork() function will not notify the debugging process about
the system call termination


16. Initializes the tsk->exit_signal field with the signal number encoded in the low
bits of the clone_flags parameter, unless the CLONE_THREAD flag is set, in which
case initializes the field to -1. As we’ll see in the section “Process Termination”
later in this chapter, only the death of the last member of a thread group (usu-
ally, the thread group leader) causes a signal notifying the parent of the thread
group leader.

17. Invokes sched_fork() to complete the initialization of the scheduler data structure of the new process. The function also sets the state of the new process to
TASK_RUNNING and sets the preempt_count field of the thread_info structure to 1,
thus disabling kernel preemption . Moreover, in order to keep process scheduling fair, the function
shares the remaining timeslice of the parent between the parent and the child.


18. Sets the cpu field in the thread_info structure of the new process to the number
of the local CPU returned by smp_processor_id().


19. Initializes the fields that specify the parenthood relationships. In particular, if
CLONE_PARENT or CLONE_THREAD are set, it initializes tsk->real_parent and tsk->parent to the value in current->real_parent; the parent of the child thus appears
as the parent of the current process. Otherwise, it sets the same fields to current.

20. If the child does not need to be traced (CLONE_PTRACE flag not set), it sets the tsk->
ptrace field to 0. This field stores a few flags used when a process is being traced
by another process. In such a way, even if the current process is being traced, the
child will not.
21. Executes the SET_LINKS macro to insert the new process descriptor in the pro-
cess list.
22. If the child must be traced (PT_PTRACED flag in the tsk->ptrace field set), it sets
tsk->parent to current->parent and inserts the child into the trace list of the
debugger.
23. Invokes attach_pid( ) to insert the PID of the new process descriptor in the
pidhash[PIDTYPE_PID] hash table.
24. If the child is a thread group leader (flag CLONE_THREAD cleared):
a. Initializes tsk->tgid to tsk->pid.
b. Initializes tsk->group_leader to tsk.
c. Invokes three times attach_pid() to insert the child in the PID hash tables of
type PIDTYPE_TGID, PIDTYPE_PGID, and PIDTYPE_SID.
25. Otherwise, if the child belongs to the thread group of its parent (CLONE_THREAD
flag set):
a. Initializes tsk->tgid to tsk->current->tgid.
b. Initializes tsk->group_leader to the value in current->group_leader.
c. Invokes attach_pid() to insert the child in the PIDTYPE_TGID hash table
(more specifically, in the per-PID list of the current->group_leader process).
26. A new process has now been added to the set of processes: increases the value of
the nr_threads variable.
27. Increases the total_forks variable to keep track of the number of forked pro-
cesses.
28. Terminates by returning the child’s process descriptor pointer (tsk)