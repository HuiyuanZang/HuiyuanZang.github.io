# Synchronization Primitives



|Technique|               Descritption       |   Scope   |
|:-----:  |             :-----:              |   :----: |
|Per-CPU variables|Duplicate a data structure among the CPUs|All CPUs|
|Atomic operation|Atomic read-modify-write instruction to a counter|Local CPU or All CPUs|
|Memory barrier|Avoid instruction reordering|All CPUs|
|Spin lock|Lock with busy wait|All CPUs|
|Semaphore|Lock with blocking wait (sleep)|All CPUs|
|Seqlocks|Lock based on an access counter|All CPUs|
|Local interrupt disabling|Forbid interrupt handling on a single CPU|Local CPU|
|Local softirq disabling|Forbid deferrable function handling on a single CPU|Local CPU|
|Read-copy-update (RCU)|Lock-free access to shared data structures through pointers|All CPUs|


Keep in mind:


The best synchronization technique consists in designing the kernel so as to avoid
the need for synchronization in the first place. As we’ll see, in fact, every explicit syn-
chronization primitive has a significant performance cost.

## Per-CPU Variables


The simplest and most efficient synchronization technique consists of declaring ker-
nel variables as per-CPU variables. Basically, a per-CPU variable is an array of data
structures, one element per each CPU in the system.

A CPU should not access the elements of the array corresponding to the other CPUs;
on the other hand, it can freely read and modify its own element without fear of race
conditions, because it is the only CPU entitled to do so. This also means, however,
that the per-CPU variables can be used only in particular cases—basically, when it
makes sense to logically split the data across the CPUs of the system.

The elements of the per-CPU array are aligned in main memory so that each data
structure falls on a different line of the hardware cache.Therefore, concurrent accesses to the per-CPU array do not result in cache line snooping and invalidation, which are costly operations in terms of system performance.

While per-CPU variables provide protection against concurrent accesses from sev-
eral CPUs, they do not provide protection against accesses from asynchronous func-
tions (interrupt handlers and deferrable functions). In these cases, additional
synchronization primitives are required.


Furthermore, per-CPU variables are prone to race conditions caused by kernel pre-
emption, both in uniprocessor and multiprocessor systems. As a general rule, a ker-
nel control path should access a per-CPU variable with kernel preemption disabled.
Just consider, for instance, what would happen if a kernel control path gets the
address of its local copy of a per-CPU variable, and then it is preempted and moved
to another CPU: the address still refers to the element of the previous CPU.

lists the main functions and macros offered by the kernel to use per-CPU
variables.

- DEFINE_PER_CPU(type, name) Statically allocates a per-CPU array called name of type data structures
- per_cpu(name, cpu) Selects the element for CPU cpu of the per-CPU array name
- _ _get_cpu_var(name) Selects the local CPU’s element of the per-CPU array name
- get_cpu_var(name) Disables kernel preemption, then selects the local CPU’s element of the
per-CPU array name
- put_cpu_var(name) Enables kernel preemption (name is not used)
- alloc_percpu(type) Dynamically allocates a per-CPU array of type data structures and returns
its address
- free_percpu(pointer) Releases a dynamically allocated per-CPU array at address pointer
- per_cpu_ptr(pointer, cpu) Returns the address of the element for CPU cpu of the per-CPU array at address pointer


## Atomic Operations

- atomic_read(v)Return *v
- atomic_set(v,i)Set *v to i
- atomic_add(i,v)Add i to *v
- atomic_sub(i,v)Subtract i from *v
- atomic_sub_and_test(i, v)Subtract i from *v and return 1 if the result is zero; 0 otherwise
- atomic_inc(v)Add 1 to *v
- atomic_dec(v)Subtract 1 from *v
- atomic_dec_and_test(v)Subtract 1 from *v and return 1 if the result is zero; 0 otherwise
- atomic_inc_and_test(v)Add 1 to *v and return 1 if the result is zero; 0 otherwise
- atomic_add_negative(i, v)Add i to *v and return 1 if the result is negative; 0 otherwise
- atomic_inc_return(v)Add 1 to *v and return the new value of *v
- atomic_dec_return(v)Subtract 1 from *v and return the new value of *v
- atomic_add_return(i, v)Add i to *v and return the new value of *v
- atomic_sub_return(i, v)Subtract i from *v and return the new value of *v
- test_bit(nr, addr)Return the value of the nrth bit of *addr
- set_bit(nr, addr)Set the nrth bit of *addr
- clear_bit(nr, addr)Clear the nrth bit of *addr
- change_bit(nr, addr)Invert the nrth bit of *addr
- test_and_set_bit(nr, addr)Set the nrth bit of *addr and return its old value
- test_and_clear_bit(nr, addr)Clear the nrth bit of *addr and return its old value
- test_and_change_bit(nr, addr)Invert the nrth bit of *addr and return its old value
- atomic_clear_mask(mask, addr)Clear all bits of *addr specified by mask
- atomic_set_mask(mask, addr)Set all bits of *addr specified by mask

## Optimization and memory barriers

When using optimizing compilers, you should never take for granted that instructions will be performed in the exact order in which they appear in the source code.
For example, a compiler might reorder the assembly language instructions in such a way to optimize how registers are used. Moreover, modern CPUs usually execute
several instructions in parallel and might reorder memory accesses. These kinds of
reordering can greatly speed up the program.

When dealing with synchronization, however, reordering instructions must be
avoided. Things would quickly become hairy if an instruction placed after a synchro-
nization primitive is executed before the synchronization primitive itself. Therefore,
all synchronization primitives act as optimization and memory barriers.


### Optimization barrier

An optimization barrier primitive ensures that the assembly language instructions cor-
responding to C statements placed before the primitive are not mixed by the compiler
with assembly language instructions corresponding to C statements placed after the
primitive. In Linux the barrier() macro, which expands into asm volatile("":::
"memory"), acts as an optimization barrier. The asm instruction tells the compiler to
insert an assembly language fragment (empty, in this case). The volatile keyword for-
bids the compiler to reshuffle the asm instruction with the other instructions of the pro-
gram. The memory keyword forces the compiler to assume that all memory locations in
RAM have been changed by the assembly language instruction; therefore, the compiler
cannot optimize the code by using the values of memory locations stored in CPU reg-
isters before the asm instruction. Notice that the optimization barrier does not ensure
that the executions of the assembly language instructions are not mixed by the CPU—
this is a job for a memory barrier.

### Memory barrier

A memory barrier primitive ensures that the operations placed before the primitive
are finished before starting the operations placed after the primitive. Thus, a mem-
ory barrier is like a firewall that cannot be passed by an assembly language instruc-
tion.

In the 80×86 processors, the following kinds of assembly language instructions are
said to be “serializing” because they act as memory barriers:
- All instructions that operate on I/O ports
- All instructions prefixed by the lock byte (see the section “Atomic Operations”)
- All instructions that write into control registers, system registers, or debug regis-
ters (for instance, cli and sti, which change the status of the IF flag in the
eflags register)
- The lfence, sfence, and mfence assembly language instructions, which have been
introduced in the Pentium 4 microprocessor to efficiently implement read mem-
ory barriers, write memory barriers, and read-write memory barriers, respec-
tively.
- A few special assembly language instructions; among them, the iret instruction
that terminates an interrupt or exception handler

In Arm64 processors, the following assembly language instructions are said to be serializing because they act as memory barriers:

- Data Memory Barrier (DMB): This instruction ensures that all memory accesses before the barrier complete before any memory accesses after the barrier start.

- Data Synchronization Barrier (DSB): This instruction ensures that all memory accesses before and after the barrier complete before any subsequent instructions are executed.

- Instruction Synchronization Barrier (ISB): This instruction ensures that all instructions before the barrier are completed before any instructions after the barrier are executed.

These instructions are used to enforce ordering constraints on memory accesses in a multi-threaded environment, ensuring that memory accesses appear in the order they were intended. Memory barriers are particularly important for synchronization primitives such as locks, semaphores, and barriers, which rely on memory ordering to ensure correctness.


Linux uses a few memory barrier primitives, which are shown as below. These
primitives act also as optimization barriers, because we must make sure the compiler
does not move the assembly language instructions around the barrier. “Read memory barriers” act only on instructions that read from memory, while “write memory barri-
ers” act only on instructions that write to memory.Memory barriers can be useful in
both multiprocessor and uniprocessor systems. The smp_xxx() primitives are used
whenever the memory barrier should prevent race conditions that might occur only in
multiprocessor systems; in uniprocessor systems, they do nothing. The other mem-
ory barriers are used to prevent race conditions occurring both in uniprocessor and
multiprocessor systems.

- mb()Memory barrier for MP and UP
- rmb()Read memory barrier for MP and UP
- wmb()Write memory barrier for MP and UP
- smp_mb()Memory barrier for MP only
- smp_rmb()Read memory barrier for MP only
- smp_wmb()Write memory barrier for MP only


## Spin locks

As a general rule, kernel preemption is disabled in every critical region protected by
spin locks. In the case of a uniprocessor system, the locks themselves are useless, and
the spin lock primitives just disable or enable the kernel preemption.Please notice
that kernel preemption is still enabled during the busy wait phase, thus a process
waiting for a spin lock to be released could be replaced by a higher priority process.

### Spin locks implementation in ARM64

```C
static inline void spin_lock(spinlock_t *lock)
{
    preempt_disable();
    while (1) {
        uint32_t tmp = 1;
        asm volatile("ldaxr %w0, [%1]\n" //This instruction loads the value of the memory location pointed to by %1 into register %w0, using a Load-Exclusive (LDX) operation.
                     "cbnz %w0, 1f\n" //This instruction checks whether the value loaded from the memory location is zero or not. If it is not zero, the code jumps to the label 1f.
                     "stxr %w0, %w2, [%1]\n" // This instruction attempts to store the value %w2 (which is 0 in this case) into the memory location pointed to by %1, using a Store-Exclusive (STX) operation. If the operation is successful, the value 0 is returned in %w0. If the operation fails (e.g., because another processor has modified the memory location since the LDX operation), a non-zero value is returned in %w0.
                     "cbnz %w0, 0b\n" //This instruction checks whether the value returned by the STX operation is zero or not. If it is not zero, the code jumps back to the label 0b and retries the LDX/STX operations.
                     "dmb sy\n" //This instruction is a Data Memory Barrier (DMB) instruction, which ensures that all memory accesses before the barrier are completed before any memory accesses after the barrier are started. This ensures that the effects of the previous LDX/STX operations are visible to other processors before the spin lock is released.
                     : "=&r" (tmp) //The colon-separated lists at the end of the assembly block are the input and output operands for the inline assembly. %w0, %1, and %w2 are placeholders for register names, and =&r and r are constraints that specify the types of the operands. In this case, %w0 is an output operand that is constrained to a general-purpose register (r), and & indicates that the operand is write-only. %1 and %w2 are input operands that are also constrained to general-purpose registers.
                     : "r" (&lock->lock), "r" (0)
                     : "memory");
        if (tmp == 0)
            break;
    }
    preempt_enable();
}

```
### Read/Write Spin Locks

Read/write spin locks have been introduced to increase the amount of concurrency
inside the kernel. They allow several kernel control paths to simultaneously read the
same data structure, as long as no kernel control path modifies it. If a kernel control
path wishes to write to the structure, it must acquire the write version of the read/write
lock, which grants exclusive access to the resource. Of course, allowing concurrent
reads on data structures improves system performance.

## Seqlocks

When using read/write spin locks, requests issued by kernel control paths to per-
form a read_lock or a write_lock operation have the same priority: readers must wait
until the writer has finished and, similarly, a writer must wait until all readers have
finished.

Seqlocks introduced in Linux 2.6 are similar to read/write spin locks, except that they
give a much higher priority to writers:in fact a writer is allowed to proceed even
when readers are active. The good part of this strategy is that a writer never waits
(unless another writer is active); the bad part is that a reader may sometimes be
forced to read the same data several times until it gets a valid copy.

## Read-Copy Update(RCU)
it is used in the networking layer and in the Virtual Filesystem.

Read-copy update (RCU) is yet another synchronization technique designed to protect
data structures that are mostly accessed for reading by several CPUs. RCU allows many
readers and many writers to proceed concurrently (an improvement over seqlocks,
which allow only one writer to proceed). Moreover, RCU is lock-free, that is, it uses no
lock or counter shared by all CPUs; this is a great advantage over read/write spin locks
and seqlocks, which have a high overhead due to cache line-snooping and invalidation.

How does RCU obtain the surprising result of synchronizing several CPUs without
shared data structures? The key idea consists of limiting the scope of RCU as follows:

1. Only data structures that are dynamically allocated and referenced by means of
pointers can be protected by RCU.
2. No kernel control path can sleep inside a critical region protected by RCU.


When a kernel control path wants to read an RCU-protected data structure, it exe-
cutes the rcu_read_lock() macro, which is equivalent to preempt_disable(). Next,
the reader dereferences the pointer to the data structure and starts reading it. As
stated above, the reader cannot sleep until it finishes reading the data structure; the
end of the critical region is marked by the rcu_read_unlock() macro, which is equiva-
lent to preempt_enable().

```C
#include <linux/module.h>
#include <linux/rcupdate.h>

struct my_data {
    int value;
};

struct rcu_head my_data_rcu;

static int __init rcu_example_init(void)
{
    struct my_data *data = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->value = 42;
    rcu_assign_pointer(my_data_rcu, data);

    rcu_read_lock();
    struct my_data *rcu_data = rcu_dereference(my_data_rcu);
    if (rcu_data) {
        printk(KERN_INFO "RCU Example: Value is %d\n", rcu_data->value);
    }
    rcu_read_unlock();

    return 0;
}
```

Because the reader does very little to prevent race conditions, we could expect that
the writer has to work a bit more. In fact, when a writer wants to update the data
structure, it dereferences the pointer and makes a copy of the whole data structure.
Next, the writer modifies the copy. Once finished, the writer changes the pointer to
the data structure so as to make it point to the updated copy. Because changing the
value of the pointer is an atomic operation, each reader or writer sees either the old
copy or the new one: no corruption in the data structure may occur. However, a
memory barrier is required to ensure that the updated pointer is seen by the other
CPUs only after the data structure has been modified. Such a memory barrier is
implicitly introduced if a spin lock is coupled with RCU to forbid the concurrent
execution of writers.

The real problem with the RCU technique, however, is that the old copy of the data
structure cannot be freed right away when the writer updates the pointer. In fact, the
readers that were accessing the data structure when the writer started its update
could still be reading the old copy. The old copy can be freed only after all (poten-
tial) readers on the CPUs have executed the rcu_read_unlock() macro. The kernel
requires every potential reader to execute that macro before:

 - The CPU performs a process switch (see restriction 2 earlier).
 - The CPU starts executing in User Mode.
 - The CPU executes the idle loop

 ## Semaphores and mutex

 Actually, Linux offers two kinds of semaphores:

- Kernel semaphores, which are used by kernel control paths
- System V IPC semaphores, which are used by User Mode processes

A kernel semaphore is similar to a spin lock, in that it doesn’t allow a kernel control
path to proceed unless the lock is open. However, whenever a kernel control path
tries to acquire a busy resource protected by a kernel semaphore, the corresponding
process is suspended. It becomes runnable again when the resource is released.
Therefore, kernel semaphores can be acquired only by functions that are allowed to
sleep; interrupt handlers and deferrable functions cannot use them.

In general, the semaphore is an older implementation, so it's advised that you use the mutex lock in place of it.
It (i.e. mutex) behaves similar to a semaphore with a count of one, but it has a simpler interface, more efficient performance, and additional constraints on its use.

A semaphore is a more generalized form of a mutex; a mutex lock can be acquired (and subsequently released or unlocked) exactly once, while a semaphore can be acquired (and subsequently released) multiple times.

A mutex is used to protect a critical section from simultaneous access, while a semaphore should be used as a mechanism to signal another waiting task that a certain milestone has been reached (typically, a producer task posts a signal via the semaphore object, which a consumer task is waiting to receive, in order to continue with further work).

A mutex has the notion of ownership of the lock and only the owner context can perform the unlock; there is no ownership for a binary semaphore.

Additionally, there are many constraints that apply to mutexes but not to semaphores. Things like process cannot exit while holding a mutex. Moreover, if CONFIG_DEBUG_MUTEXES kernel option is enabled, then all the constraints that apply on mutexes are ensured by debugging checks.

So, unless there is a good reason not to use mutex, that should be first choice.

## Complettions 

The function corresponding to up() is called complete().

The function corresponding to down() is called wait_for_completion().

The real difference between completions and semaphores is how the spin lock
included in the wait queue is used. In completions, the spin lock is used to ensure
that complete() and wait_for_completion() cannot execute concurrently. In semaphores, the spin lock is used to avoid letting concurrent down()’s functions mess up the semaphore data structure.

## Local Interrupt Disabling

Interrupt disabling is one of the key mechanisms used to ensure that a sequence of
kernel statements is treated as a critical section. It allows a kernel control path to
continue executing even when hardware devices issue IRQ signals, thus providing an
effective way to protect data structures that are also accessed by interrupt handlers.
By itself, however, local interrupt disabling does not protect against concurrent
accesses to data structures by interrupt handlers running on other CPUs, so in multi-
processor systems, local interrupt disabling is often coupled with spin locks.

The local_irq_disable( ) macro isables interrupts on the local CPU. The local_irq_enable() enables them.

Saving and restoring the eflags content is achieved by means of the local_irq_save
and local_irq_restore macros, respectively.

## Disabling and Enabling Deferrable Functions

A trivial way to forbid deferrable functions execution on a CPU is to disable inter-
rupts on that CPU. Because no interrupt handler can be activated, softirq actions
cannot be started asynchronously.

However, the kernel sometimes needs to disable
deferrable functions without disabling interrupts. Local deferrable functions can be
enabled or disabled on the local CPU by acting on the softirq counter stored in the
preempt_count field of the current’s thread_info descriptor.

Recall that the do_softirq() function never executes the softirqs if the softirq
counter is positive. Moreover, tasklets are implemented on top of softirqs, so setting
this counter to a positive value disables the execution of all deferrable functions on a
given CPU, not just softirqs.

The local_bh_disable macro adds one to the softirq counter of the local CPU, while
the local_bh_enable() function subtracts one from it. The kernel can thus use sev-
eral nested invocations of local_bh_disable; deferrable functions will be enabled
again only by the local_bh_enable macro matching the first local_bh_disable invoca-
tion.


## Choosing Among Spin locks, Semaphores and Interrupt disabling
Protection required by data structures accessed by kernel control paths:
|Kernel control paths accessing the data structure|               UP protection       |   MP further protection   |
|:-----:  |             :-----:              |   :----: |
|Exceptions|Semaphore and Mutex and RCU |None|
|Interrupts|Local interrupt disabling |Spin lock|
|Deferrable functions|None|None or spin lock|
|Exceptions + Interrupts|Local interrupt disabling|Spin lock|
|Exceptions + Deferrable functions|Local softirq disabling|Spin lock|
|Interrupts + Deferrable functions|Local interrupt disabling|Spin lock|
|Exceptions + Interrupts + Deferrable functions|Local interrupt disabling|Spin lock|

### Protecting a data structure accessed by exceptions

When a data structure is accessed only by exception handlers, race conditions are
usually easy to understand and prevent. The most common exceptions that give rise
to synchronization problems are the system call service routines n which the CPU operates in
Kernel Mode to offer a service to a User Mode program. Thus, a data structure accessed only by an exception usually represents a resource that can be assigned to
one or more processes.
Race conditions are avoided through semaphores, because these primitives allow the
process to sleep until the resource becomes available. Notice that semaphores work
equally well both in uniprocessor and multiprocessor systems.

### Protecting a data structure accessed by interrupts

Suppose that a data structure is accessed by only the “top half” of an interrupt han-
dler. We learned in the section “Interrupt Handling” in Chapter 4 that each inter-
rupt handler is serialized with respect to itself—that is, it cannot execute more than
once concurrently. Thus, accessing the data structure does not require synchroniza-
tion primitives.That means that interrrupt handler doesn't need to be reentrance.

Things are different, however, if the data structure is accessed by several interrupt
handlers. A handler may interrupt another handler, and different interrupt handlers
may run concurrently in multiprocessor systems. Without synchronization, the
shared data structure might easily become corrupted.

In uniprocessor systems, race conditions must be avoided by disabling interrupts in
all critical regions of the interrupt handler. Nothing less will do because no other
synchronization primitives accomplish the job. A semaphore can block the process,
so it cannot be used in an interrupt handler. A spin lock, on the other hand, can
freeze the system: if the handler accessing the data structure is interrupted, it cannot
release the lock; therefore, the new interrupt handler keeps waiting on the tight loop
of the spin lock.

Multiprocessor systems, as usual, are even more demanding. Race conditions cannot
be avoided by simply disabling local interrupts. In fact, even if interrupts are dis-
abled on a CPU, interrupt handlers can still be executed on the other CPUs. The
most convenient method to prevent the race conditions is to disable local interrupts
(so that other interrupt handlers running on the same CPU won’t interfere) and to
acquire a spin lock or a read/write spin lock that protects the data structure. Notice
that these additional spin locks cannot freeze the system because even if an interrupt
handler finds the lock closed, eventually the interrupt handler on the other CPU that
owns the lock will release it.

### Protecting a data structure accessed by deferrable functions 

First of all, no race condition may exist in uniprocessor systems. This is because exe-
cution of deferrable functions is always serialized on a CPU—that is, a deferrable
function cannot be interrupted by another deferrable function. Therefore, no syn-
chronization primitive is ever required.

Conversely, in multiprocessor systems, race conditions do exist because several
deferrable functions may run concurrently.

- softirqs   Spin lock
- one tasklet None
- many tasklet spin lock

A data structure accessed by a softirq must always be protected, usually by means of
a spin lock, because the same softirq may run concurrently on two or more CPUs.
Conversely, a data structure accessed by just one kind of tasklet need not be pro-
tected, because tasklets of the same kind cannot run concurrently. However, if the
data structure is accessed by several kinds of tasklets, then it must be protected.

### Protecting a data structure accessed by exceptions and interrupts

Let’s consider now a data structure that is accessed both by exceptions (for instance,
system call service routines) and interrupt handlers.

On uniprocessor systems, race condition prevention is quite simple, because inter-
rupt handlers are not reentrant and cannot be interrupted by exceptions. As long as
the kernel accesses the data structure with local interrupts disabled, the kernel can-
not be interrupted when accessing the data structure. However, if the data structure
is accessed by just one kind of interrupt handler, the interrupt handler can freely
access the data structure without disabling local interrupts.

On multiprocessor systems, we have to take care of concurrent executions of excep-
tions and interrupts on other CPUs. Local interrupt disabling must be coupled with a
spin lock, which forces the concurrent kernel control paths to wait until the handler
accessing the data structure finishes its work.

Sometimes it might be preferable to replace the spin lock with a semaphore. Because
interrupt handlers cannot be suspended, they must acquire the semaphore using a
tight loop and the down_trylock() function; for them, the semaphore acts essentially
as a spin lock. System call service routines, on the other hand, may suspend the call-
ing processes when the semaphore is busy. For most system calls, this is the expected
behavior. In this case, semaphores are preferable to spin locks, because they lead to a
higher degree of concurrency of the system.

### Protecting a data structure accessed by exceptions and deferrable functions

A data structure accessed both by exception handlers and deferrable functions can be
treated like a data structure accessed by exception and interrupt handlers. In fact,
deferrable functions are essentially activated by interrupt occurrences, and no excep-
tion can be raised while a deferrable function is running. Coupling local interrupt
disabling with a spin lock is therefore sufficient.
Actually, this is much more than sufficient: the exception handler can simply disable
deferrable functions instead of local interrupts by using the local_bh_disable()
macro.Disabling only the deferrable func-
tions is preferable to disabling interrupts, because interrupts continue to be serviced
by the CPU. Execution of deferrable functions on each CPU is serialized, so no race
condition exists.

### Protecting a data structure accessed by interrupts and deferrable functions

This case is similar to that of a data structure accessed by interrupt and exception
handlers. An interrupt might be raised while a deferrable function is running, but no
deferrable function can stop an interrupt handler. Therefore, race conditions must be
avoided by disabling local interrupts during the deferrable function. However, an
interrupt handler can freely touch the data structure accessed by the deferrable func-
tion without disabling interrupts, provided that no other interrupt handler accesses
that data structure.
Again, in multiprocessor systems, a spin lock is always required to forbid concurrent
accesses to the data structure on several CPUs.

### Protecting a data structure accessed by exceptions, interrupts, and deferrable functions

Similarly to previous cases, disabling local interrupts and acquiring a spin lock is
almost always necessary to avoid race conditions. Notice that there is no need to
explicitly disable deferrable functions, because they are essentially activated when
terminating the execution of interrupt handlers; disabling local interrupts is there-
fore sufficient.