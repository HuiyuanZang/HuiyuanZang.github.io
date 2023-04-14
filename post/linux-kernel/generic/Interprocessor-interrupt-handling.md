# Interprocessor Interrupt Handling

Interprocessor interrupts allow a CPU to send interrupt signals to any other CPU in
the system. As explained in the section “The Advanced Programmable Interrupt
Controller (APIC)” earlier in this chapter, an interprocessor interrupt (IPI) is deliv-
ered not through an IRQ line, but directly as a message on the bus that connects the
local APIC of all CPUs (either a dedicated bus in older motherboards, or the system
bus in the Pentium 4–based motherboards).
On multiprocessor systems, Linux makes use of three kinds of interprocessor interrupts 

- CALL_FUNCTION_VECTOR (vector 0xfb)

Sent to all CPUs but the sender, forcing those CPUs to run a function passed by
the sender. The corresponding interrupt handler is named call_function_
interrupt( ). The function (whose address is passed in the call_data global vari-
able) may, for instance, force all other CPUs to stop, or may force them to set
the contents of the Memory Type Range Registers (MTRRs).* Usually this inter-
rupt is sent to all CPUs except the CPU executing the calling function by means
of the smp_call_function() facility function.

- RESCHEDULE_VECTOR (vector 0xfc)

When a CPU receives this type of interrupt, the corresponding handler—named
reschedule_interrupt() —limits itself to acknowledging the interrupt. Resched-
uling is done automatically when returning from the interrupt 

- INVALIDATE_TLB_VECTOR (vector 0xfd)

Sent to all CPUs but the sender, forcing them to invalidate their Translation
Lookaside Buffers. The corresponding handler, named invalidate_interrupt( ),
flushes some TLB entries of the processor as described in the section 