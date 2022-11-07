# DMA coherence issue

## what is DMA coherence issue

Let's imagine a CPU equipped with a cache and an external memory that can be accessed
directly by devices using DMA. When the CPU accesses location X in the memory, the
current value will be stored in the cache. Subsequent operations on X will update the
cached copy of X, but not the external memory version of X, assuming a write-back cache.
If the cache is not flushed to the memory before the next time a device tries to access X, the
device will receive a stale value of X. Similarly, if the cached copy of X is not invalidated
when a device writes a new value to the memory, then the CPU will operate on a stale
value of X.

There are actually two ways to address this issue:

- A hardware-based solution. Such systems are coherent systems.
- A software-based solution, where the OS is responsible for ensuring cache coherency. One calls such systems non-coherent systems.


Any suitable DMA transfer requires suitable memory mapping. A DMA mapping consists
of allocating a DMA buffer and generating a bus address for it. Devices actually use bus
addresses. Bus addresses are each instance of the dma_addr_t type.
One distinguishes two types of mapping: **coherent DMA** mappings and **streaming DMA mappings**. 
One can use the former over several transfers, which automatically addresses
cache coherency issues. Therefore, it is too expensive. Streaming mapping has a lot of
constraints and does not automatically address coherency issues, although, there is a
solution for that, which consists of several function calls between each transfer. Coherent
mapping usually exists for the life of the driver, whereas a streaming mapping is usually
unmapped once the DMA transfer completes. One should use streaming mapping when one can and coherent mapping when one must.


## Coherent Mapping 


## Streaming Mapping