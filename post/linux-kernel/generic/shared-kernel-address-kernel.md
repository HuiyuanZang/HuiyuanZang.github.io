# why does the kernel share its address space with every process


There is a reason why the kernel shares its address space with every process: because every
single process at a given moment uses system calls, which will involve the kernel. Mapping
the kernel's virtual memory address into each process's virtual address space allows us to
avoid the cost of switching out the memory address space on each entry to (and exit from)
the kernel. This is the reason why the kernel address space is permanently mapped on top
of each process in order to speed up kernel access through system calls.

- Memory **page**, virtual page, or simply page are terms you use to refer to a fixed-
length contiguous block of virtual memory. The same name, page , is used as a
kernel data structure to represent a memory page.

- On the other hand, a **frame** (or page frame) refers to a fixed-length contiguous
block of physical memory on top of which the operating system maps a memory
page. Each page frame is given a number, called a page frame number (PFN).
Given a page, you can easily get its PFN and vice versa, using the page_to_pfn
and pfn_to_page macros, which will be discussed in detail in the next sections.

- A **page table** is the kernel and architecture data structure used to store the
mapping between virtual addresses and physical addresses. The key pair
page/frame describes a single entry in the page table. This represents a mapping.



the size page = the size frame


Another reason is that Kernel should manage all resouce shared with each process.
