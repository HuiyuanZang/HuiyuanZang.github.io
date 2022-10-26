# Is printk() safe to use in kernel thread or interrupt handler, in other word, atomic context


printk() never blocks and is safe enough to be called even from atomic
contexts. It tries to lock the console and print the message. If locking fails,
the output will be written into a buffer and the function will return, never
blocking. The current console holder will then be notified about new
messages and will print them before releasing the console.
