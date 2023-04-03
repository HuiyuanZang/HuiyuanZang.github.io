# What happends when sending SIGKILL to a Zombie Process in Linux?


Each process is represented as a task_struct inside the kernel (the definition is in the sched.h header file and begins here). That struct holds information about the process; for instance the pid. The important information is in line struct signal_struct where the associated signal is stored. This is set only if a signal is sent to the process.


A dead process or a zombie process still has a task_struct. The struct remains, until the parent process (natural or by adoption) has called wait() after receiving SIGCHLD to reap its child process. When a signal is sent, the signal_struct is set. It doesn't matter if the signal is a catchable one or not, in this case.

Signals are evaluated every time when the process runs. Or to be exact, before the process would run. The process is then in the TASK_RUNNING state. The kernel runs the schedule() routine which determines the next running process according to its scheduling algorithm. Assuming this process is the next running process, the value of the signal_struct is evaluated, whether there is a waiting signal to be handled or not. If a signal handler is manually defined (via signal() or sigaction()), the registered function is executed, if not the signal's default action is executed. The default action depends on the signal being sent.


For instance, the SIGSTOP signal's default handler will change the current process's state to TASK_STOPPED and then run schedule() to select a new process to run. Notice, SIGSTOP is not catchable (like SIGKILL), therefore there is no possibility to register a manual signal handler. In case of an uncatchable signal, the default action will always be executed.


A defunct or dead process will never be determined by the scheduler to be in the TASK_RUNNING state again. Thus the kernel will never run the signal handler (default or defined) for the corresponding signal, whichever signal is was. Therefore the exit_signal will never be set again. The signal is "delivered" to the process by setting the signal_struct in task_struct of the process, but nothing else will happen, because the process will never run again. There is no code to run, all that remains of the process is that process struct.

However, if the parent process reaps its children by wait(), the exit code it receives is the one when the process "initially" died. It doesn't matter if there is a signal waiting to be handled.