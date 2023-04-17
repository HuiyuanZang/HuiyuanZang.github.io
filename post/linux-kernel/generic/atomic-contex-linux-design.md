# Atomic context and kernel API design

An API should refrain from making promises that it cannot keep. A recent episode involving the kernel's in_atomic() macro demonstrates how things can go wrong when a function does not really do what it appears to do. It is also a good excuse to look at an under-documented (but fundamental) aspect of kernel code design.

Kernel code generally runs in one of two fundamental contexts. Process context reigns when the kernel is running directly on behalf of a (usually) user-space process; the code which implements system calls is one example. When the kernel is running in process context, it is allowed to go to sleep if necessary. But when the kernel is running in atomic context, things like sleeping are not allowed. Code which handles hardware and software interrupts is one obvious example of atomic context.

There is more to it than that, though: any kernel function moves into atomic context the moment it acquires a spinlock. Given the way spinlocks are implemented, going to sleep while holding one would be a fatal error; if some other kernel function tried to acquire the same lock, the system would almost certainly deadlock forever.

"Deadlocking forever" tends not to appear on users' wishlists for the kernel, so the kernel developers go out of their way to avoid that situation. To that end, code which is running in atomic context carefully follows a number of rules, including (1) no access to user space, and, crucially, (2) no sleeping. Problems can result, though, when a particular kernel function does not know which context it might be invoked in. The classic example is kmalloc() and friends, which take an explicit argument (GFP_KERNEL or GFP_ATOMIC) specifying whether sleeping is possible or not.

The wish to write code which can work optimally in either context is common, though. Some developers, while trying to write such code, may well stumble across the following definitions from <linux/hardirq.h>:
```C
    /*
     * Are we doing bottom half or hardware interrupt processing?
     * Are we in a softirq context? Interrupt context?
     */
    #define in_irq()	   (hardirq_count())
    #define in_softirq()   (softirq_count())
    #define in_interrupt() (irq_count())

    #define in_atomic()	   ((preempt_count() & ~PREEMPT_ACTIVE) != 0)
```
It would seem that in_atomic() would fit the bill for any developer trying to decide whether a given bit of code needs to act in an atomic manner at any specific time. A quick grep through the kernel sources shows that, in fact, in_atomic() has been used in quite a few different places for just that purpose. There is only one problem: those uses are almost certainly all wrong.

The in_atomic() macro works by checking whether preemption is disabled, which seems like the right thing to do. Handlers for events like hardware interrupts will disable preemption, but so will the acquisition of a spinlock. So this test appears to catch all of the cases where sleeping would be a bad idea. Certainly a number of people who have looked at this macro have come to that conclusion.

But if preemption has not been configured into the kernel in the first place, the kernel does not raise the "preemption count" when spinlocks are acquired. So, in this situation (which is common - many distributors still do not enable preemption in their kernels), in_atomic() has no way to know if the calling code holds any spinlocks or not. So it will return zero (indicating process context) even when spinlocks are held. And that could lead to kernel code thinking that it is running in process context (and acting accordingly) when, in fact, it is not.

Given this problem, one might well wonder why the function exists in the first place, why people are using it, and what developers can really do to get a handle on whether they can sleep or not. Andrew Morton answered the first question in a relatively cryptic way:

in_atomic() is for core kernel use only. Because in special circumstances (ie: kmap_atomic()) we run inc_preempt_count() even on non-preemptible kernels to tell the per-arch fault handler that it was invoked by copy_*_user() inside kmap_atomic(), and it must fail.

In other words, in_atomic() works in a specific low-level situation, but it was never meant to be used in a wider context. Its placement in hardirq.h next to macros which can be used elsewhere was, thus, almost certainly a mistake. As Alan Stern pointed out, the fact that Linux Device Drivers recommends the use of in_atomic() will not have helped the situation. Your editor recommends that the authors of that book be immediately sacked.

Once these mistakes are cleared up, there is still the question of just how kernel code should decide whether it is running in an atomic context or not. The real answer is that it just can't do that. Quoting Andrew Morton again:

The consistent pattern we use in the kernel is that callers keep track of whether they are running in a schedulable context and, if necessary, they will inform callees about that. Callees don't work it out for themselves.
This pattern is consistent through the kernel - once again, the GFP_ flags example stands out in this regard. But it's also clear that this practice has not been documented to the point that kernel developers understand that things should be done this way. Consider this recent posting from Rusty Russell, who understands these issues better than most:

This flag indicates what the allocator should do when no memory is immediately available: should it wait (sleep) while memory is freed or swapped out (GFP_KERNEL), or should it return NULL immediately (GFP_ATOMIC). And this flag is entirely redundant: kmalloc() itself can figure out whether it is able to sleep or not.

In fact, kmalloc() cannot figure out on its own whether sleeping is allowable or not. It has to be told by the caller. This rule is unlikely to change, so expect a series of in_atomic() removal patches starting with 2.6.26. Once that work is done, the in_atomic() macro can be moved to a safer place where it will not create further confusion.