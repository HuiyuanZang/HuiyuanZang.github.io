# Scheduling Policy


## Conflicting Objectives

- fast process response time, 
- good throughput for backgroundjobs, 
- avoidance of process starvation, 
- reconciliation of the needs of low- and high-priority processes,

The set of rules used to determine when and how to select a new process to run is called scheduling policy.

## Classification 

When speaking about scheduling, processes are traditionally classified as **I/O-bound**
or **CPU-bound**.

An alternative classification distinguishes three classes of processes:
- Interactive processes

  These interact constantly with their users, and therefore spend a lot of time wait-
ing for keypresses and mouse operations.

- Batch processes

  These do not need user interaction, and hence they often run in the back-
ground. Because such processes do not need to be very responsive, they are often
penalized by the scheduler. Typical batch programs are programming language
compilers, database search engines, and scientific computations.

- Real-time processes

  These have very stringent scheduling requirements. Such processes should never
be blocked by lower-priority processes and should have a short guaranteed
response time with a minimum variance. Typical real-time programs are video
and sound applications, robot controllers, and programs that collect data from
physical sensors.

It is often believed that a long quantum duration degrades the response time of inter-
active applications. This is usually false.Interactive processes have a relatively high priority,
so they quickly preempt the batch processes, no matter how long the quantum duration is.

In some cases, however, a very long quantum duration degrades the responsiveness of
the system. For instance, suppose two users concurrently enter two commands at the
respective shell prompts; one command starts a CPU-bound process, while the other
launches an interactive application. Both shells fork a new process and delegate the
execution of the user’s command to it; moreover, suppose such new processes have
the same initial priority (Linux does not know in advance if a program to be executed
is batch or interactive). Now if the scheduler selects the CPU-bound process to run first, the other process could wait for a whole time quantum before starting its execu-
tion. Therefore, if the quantum duration is long, the system could appear to be unresponsive to the user that launched the interactive application.

The choice of the average quantum duration is always a compromise. The rule of
thumb adopted by Linux is choose a duration as long as possible, while keeping
good system response time.


## system calls related to scheduling

- nice( )Change the static priority of a conventional process
- getpriority( )Get the maximum static priority of a group of conventional processes
- setpriority( )Set the static priority of a group of conventional processes
- sched_getscheduler( )Get the scheduling policy of a process
- sched_setscheduler( )Set the scheduling policy and the real-time priority of a process
- sched_getparam( )Get the real-time priority of a process
- sched_setparam( )Set the real-time priority of a process
- sched_yield( )Relinquish the processor voluntarily without blocking
- sched_get_priority_min( )Get the minimum real-time priority value for a policy
- sched_get_priority_max( )Get the maximum real-time priority value for a policy
- sched_rr_get_interval( )Get the time quantum value for the Round Robin policy
- sched_setaffinity()Set the CPU affinity mask of a process
- sched_getaffinity()Get the CPU affinity mask of a process