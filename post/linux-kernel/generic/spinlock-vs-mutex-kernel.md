# Spinlock versus mutexes
Used for concurrency in the kernel, spinlocks and mutexes both have their own objectives:
Mutexes protect the process's critical resources, whereas spinlocks protect the
IRQ handler's critical sections



Mutexes put contenders to sleep until the lock is acquired, whereas spinlocks
infinitely spin in a loop (consuming CPU) until the lock is acquired
Because of the previous point, you can't hold spinlocks for a long time, since
waiters will waste CPU time waiting for the lock, whereas a mutex can be held as
long as the resource needs to be protected, since contenders are put to sleep in a
wait queue.


When dealing with spinlocks, please keep in mind that preemption is
disabled only for threads holding spinlocks, not for spinning waiters.
