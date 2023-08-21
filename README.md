# IPCviaSharedMemory
"Explore the use of shared memory for IPC. This repo offers tutorials and examples on effectively utilizing shared memory between processes. Perfect for beginners and experts alike. Contributions welcome!"

# Memory Mapping and Its Processes

Memory mapping is a cornerstone of modern computer systems, ensuring efficient memory utilization and process isolation. This document delves deeper into the details.

## Table of Contents
- [Introduction](#introduction)
- [Why Memory Mapping?](#why-memory-mapping)
- [Memory Mapping Process](#memory-mapping-process)
- [Virtual vs Physical Address](#virtual-vs-physical-address)
- [Page Tables & Translation](#page-tables--translation)
- [Memory Management Unit (MMU)](#memory-management-unit-mmu)



## Introduction
Memory mapping is the method through which virtual addresses are mapped to physical addresses. This abstraction layer lets programs treat memory in a manner that might differ from the actual RAM organization.

## Why Memory Mapping?
- **Process Isolation:** Each process gets its virtual address space, safeguarding it from unintentional interference by others.
- **Memory Efficiency:** Allows for mechanisms like paging, where inactive portions of a program's memory can be stored on disk instead of RAM.
- **Easier Management:** Simplifies memory allocations and reallocations.

## Memory Mapping Process
1. **Virtual Address Generation:** When a process needs to access memory, it generates a virtual address.
2. **Translation:** The system translates this virtual address to a physical one.
3. **Physical Memory Access:** The CPU accesses the actual memory location using the translated physical address.

## Virtual vs Physical Address
- **Virtual Address:** Generated by CPU while a program runs. It references a location in the virtual memory space.
- **Physical Address:** The real address in the main memory hardware (RAM). It's where the data is actually stored.

## Page Tables & Translation
The OS and MMU use a data structure called the "page table" for translation:
- Each entry in the page table maps a virtual page to a physical frame in memory.
- Large programs might have multiple page tables or hierarchical structures.
- Translation Lookaside Buffers (TLBs) cache recent translations for faster lookup.

## Memory Management Unit (MMU)
MMU is a dedicated hardware component handling the translation:
1. On receiving a virtual address, the MMU checks if the translation is cached in the TLB.
2. If not cached, the MMU refers to the page table.
3. In case of a "page fault" (when the page isn't in RAM), the OS fetches it from the disk into the RAM, updating the page table.

---
# Internals of `mmap`, `munmap`, and `msync`

Memory mapping is a complex dance between a process, the OS kernel, and the hardware. This document aims to shed light on the internal operations of `mmap`, `munmap`, and `msync`.

## Table of Contents
- [How `mmap` Works](#how-mmap-works)
- [How `munmap` Works](#how-munmap-works)
- [How `msync` Works](#how-msync-works)

## How `mmap` Works
When a process invokes `mmap`, several internal steps are initiated:
1. **Request Validation:** The kernel first checks the validity of the request, including file permissions, provided flags, and alignment constraints.
2. **Address Space Reservation:** The kernel reserves a range in the process's virtual address space without allocating physical memory.
3. **Page Table Entry:** The OS modifies the process's page table to reflect this mapping. However, no actual data loading from the file occurs now.
4. **Lazy Loading:** When the process accesses this memory region, a page fault is triggered since the data isn't in RAM yet. The OS then loads the required data from the file into physical memory and updates the page table to link the virtual page to the physical one.

## How `munmap` Works
The process of `munmap` is about deallocating and cleaning:
1. **Address Range Validation:** The kernel ensures the range provided is valid and was indeed mapped earlier.
2. **Page Table Update:** The associated entries in the page table are cleared. This disassociates the virtual addresses from any physical memory.
3. **Physical Memory Reclamation:** While the virtual mapping is removed immediately, the actual physical memory might be deallocated lazily, depending on system requirements and memory pressure.

## How `msync` Works
`msync` is about ensuring data consistency between the process's virtual memory and the underlying file or memory:
1. **Range Validation:** The kernel checks if the specified range is a valid memory-mapped region.
2. **Dirty Page Check:** The OS identifies "dirty" pages - pages in memory that were modified but not written back to the file.
3. **Write-back:** Dirty pages are written back to the underlying file or device. If the `MS_SYNC` flag is used, the call blocks until this operation completes. Otherwise, with `MS_ASYNC`, the operation might complete in the background.

---

**Conclusion:** While these system calls provide a simplified interface to programmers, the underlying machinery is intricate, ensuring optimal performance and data integrity. The beauty of these mechanisms is in how they abstract away complex memory and file management tasks, allowing developers to focus on application logic.

------

# Memory Mapping in UNIX-like Operating Systems

```SQL
+----------------------------------------------------------------------------------------------------------------------------------+
|                                                                   System Memory                                                                              |
|                                                                                                                                                              |
|  +----------------+       +--------+        +---------------------+        +--------+        +----------------+                                              |
|  |  Process A     |       |        |        |                     |        |        |        |  Process B     |                                              |
|  |                |       |        |        |                     |        |        |        |                |                                              |
|  /  Virtual Page X / ----> |  MMU   | ---->  | Physical Memory Page| <----  |  MMU   | <---- | Virtual Page Y |                                              |
|  |                |       |        |        |                     |        |        |        |                |                                              |
|  |                |       |        |        |    (Shared Memory)  |        |        |        |                |                                              |
|  +----------------+       +--------+        +---------------------+        +--------+        +----------------+                                              |
|                                                                                                                                                              |
+----------------------------------------------------------------------------------------------------------------------------------+
```



Memory mapping is a mechanism that allows processes in UNIX-like systems to access data in a file as if it were in memory. This document dives deep into the internal workings of the `mmap`, `munmap`, and `msync` system calls.

## Table of Contents
- [mmap](#mmap)
- [munmap](#munmap)
- [msync](#msync)

## mmap

`mmap` is used to map a file or other objects into the memory space of a process.

1. **Request Processing:** The kernel begins by validating parameters like file permissions and the requested length.
2. **Virtual Address Space Reservation:** A range in the process's virtual address space is reserved without allocating physical memory.
3. **Page Table Entry Creation:** Entries in the process's page table are created but marked as not present.
4. **Lazy Loading:** Actual data loading happens on-demand. Accessing the memory region triggers a page fault, loading the required data from the file into RAM.
5. **Access Control:** The kernel manages memory access based on protection flags provided during the `mmap` call.

## munmap

`munmap` is used to unmap previously mapped regions.

1. **Request Validation:** The kernel ensures that the specified range corresponds to a mapped region.
2. **Page Table Entry Removal:** The relevant entries in the page table are cleared, disconnecting virtual addresses from their physical counterparts.
3. **Physical Memory Handling:** Physical pages might not be deallocated immediately but might be marked for reuse.
4. **Address Space Release:** The reserved range in the virtual address space is released.

## msync

`msync` synchronizes the memory-mapped region with its backing storage.

1. **Range Verification:** The kernel verifies the provided range.
2. **Dirty Page Identification:** "Dirty" pages, or modified pages, are identified within the range.
3. **Write-back Operation:** Dirty pages are written back to the underlying file or device. The behavior might vary based on flags like `MS_SYNC` and `MS_ASYNC`.
4. **Cache Coherency:** Modified cache lines associated with the memory-mapped region are flushed to ensure consistency.

---

## Conclusion

Memory mapping simplifies file and device I/O by enabling direct access through memory operations. `mmap`, `munmap`, and `msync` play vital roles in establishing, managing, and synchronizing these mappings. A deep understanding of these mechanisms provides insights into system-level programming in UNIX-like environments.

------

# Memory Mapping in UNIX: mmap, munmap, msync

This document provides an overview, argument explanations, and examples for the system calls `mmap`, `munmap`, and `msync`.

## Table of Contents
- [mmap](#mmap)
- [munmap](#munmap)
- [msync](#msync)

## mmap

### Synopsis
```c
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```

### Arguments
- `addr`: Suggested starting address for the mapping. Typically set to `NULL`, allowing the kernel to decide.
- `length`: Length of the mapping.
- `prot`: Memory protection of the mapping. Common values include `PROT_READ`, `PROT_WRITE`, and `PROT_EXEC`.
- `flags`: Determines how the mapping is applied. Common flags are `MAP_SHARED` and `MAP_PRIVATE`.
- `fd`: File descriptor of the file to be mapped.
- `offset`: Offset in the file where the mapping starts.

### Example
```c
int fd = open("example.txt", O_RDONLY);
char *mapped = mmap(NULL, file_length, PROT_READ, MAP_SHARED, fd, 0);
```

## munmap

### Synopsis
```c
int munmap(void *addr, size_t length);
```

### Arguments
- `addr`: Address of the mapping to unmap.
- `length`: Length of the mapping.

### Example
```c
int result = munmap(mapped, file_length);
```

## msync

### Synopsis
```c
int msync(void *addr, size_t length, int flags);
```

### Arguments
- `addr`: Address of the mapped region to synchronize.
- `length`: Length of the mapped region.
- `flags`: Controls the behavior of the synchronization. Common flags include `MS_SYNC` (synchronous write) and `MS_ASYNC` (asynchronous write).

### Example
```c
int sync_result = msync(mapped, file_length, MS_SYNC);
```

---

# Shared Memory File Mapping

```SQL
+--------------------------------------------------------------------------------------------------------+
|                                  Shared Memory (Inter-Process Communication)                           |
|                                                                                                        |
|  +-----------------+                             +-----------------------+                            +-----------------+
|  |   Process A     |                             |      Shared Memory     |                           |    Process B    |
|  |                 |      +----------->          |                       |          <-----------+     |                 |
|  |                 |      | Reads/Writes         +-----------------------+         Reads/Writes |     |                 |
|  |                 |      |                                                                    |      |                 |
|  +-----------------+      |                                                                    |      +-----------------+
|                            |                                                                    |                         
|                            +--------------------------------------------------------------------+                         
|                                                                                                        |
+--------------------------------------------------------------------------------------------------------+

```


This repository contains two programs, `file_read` and `file_write`, that demonstrate the concept of memory mapping files into a process's virtual address space using `mmap`.

## Overview

- `file_read`: Reads data from a given file using memory mapping and prints the details of a student.
- `file_write`: Writes a student's data to a given file using memory mapping.

## Pseudo-code

### `file_read`

```vbnet
PROGRAM file_read

STRUCTURE student_t:
    INTEGER roll_no
    INTEGER marks
    STRING city[128]

START

    IF number of command line arguments is less than 2:
        PRINT "File path not mentioned"
        EXIT

    STRING filepath = command line argument[1]

    OPEN filepath in read-only mode and get file descriptor fd
    IF fd is invalid:
        PRINT "File could not open"
        EXIT

    GET file size using fstat on fd into statbuf

    MAP file using mmap:
        address = NULL
        length = size from statbuf
        protection = PROT_READ
        flags = MAP_PRIVATE
        file descriptor = fd
        offset = 0

    IF mapping failed:
        PRINT "Mapping Failed"
        EXIT

    CLOSE file descriptor fd

    COPY memory from mapped area to student_t structure

    PRINT student details

    UNMAP mapped memory

END
```

### `file_write`

```vbnet
PROGRAM file_write

STRUCTURE student_t:
    INTEGER roll_no
    INTEGER marks
    STRING name[128]
    STRING city[128]

START

    IF number of command line arguments is less than 2:
        PRINT "File path not mentioned"
        EXIT

    STRING filepath = command line argument[1]

    OPEN filepath in read-write mode and get file descriptor fd
    IF fd is invalid:
        PRINT "File could not open"
        EXIT

    GET file size using fstat on fd into statbuf

    MAP file using mmap:
        address = NULL
        length = size from statbuf
        protection = PROT_READ | PROT_WRITE
        flags = MAP_SHARED
        file descriptor = fd
        offset = 0

    IF mapping failed:
        PRINT "Mapping Failed"
        EXIT

    CLOSE file descriptor fd

    INITIALIZE student_t structure with data:
        roll_no = 123
        marks = 90
        name = "Abhishek"
        city = "Bangalore"

    COPY student_t data to mapped area

    SYNCHRONIZE data using msync

    UNMAP mapped memory

END
```

## Conclusion

These programs provide a hands-on demonstration of how `mmap` can be used to map files into a process's virtual address space, enabling efficient file reading and writing operations.

----------

# Shared Memory as IPC: When to Use It (and When Not To)?

Inter-Process Communication (IPC) is crucial for facilitating information exchange between different processes in an operating system. One popular IPC mechanism is shared memory. But when should you choose shared memory over other IPC mechanisms, and when should you avoid it? Let's dive in!

## Advantages of Shared Memory

1. **Speed**: Shared memory is typically faster than other IPC methods, since processes directly access the same memory region, avoiding the overhead of data copying.
2. **Direct Communication**: Processes can communicate without intermediary steps or system calls once the shared memory region is established.
3. **Flexibility**: Multiple processes can access shared memory simultaneously, allowing for dynamic and concurrent communication.

## When Should You Use Shared Memory as IPC?

1. **High Throughput Requirements**: If your processes need to exchange large volumes of data quickly, shared memory might be the answer. No copying data between process spaces makes it swift.
2. **Fine-grained Control**: Shared memory gives you explicit control over when and how data is accessed, which can be crucial in real-time or latency-sensitive applications.
3. **State Sharing**: When different processes need continuous and immediate access to shared state (e.g., a configuration or a shared counter), shared memory can be very effective.
4. **Complex Data Structures**: If you're passing complex data structures (like trees or linked lists) between processes, serializing and deserializing them for other IPC methods can be cumbersome. With shared memory, you can directly share these structures.

## When Should You Avoid Shared Memory as IPC?

1. **Distributed Systems**: In systems where processes run on different machines, shared memory isn't an option. Instead, methods like message passing or sockets are more appropriate.
2. **Isolation Needs**: If you want strict isolation between processes, using shared memory might introduce vulnerabilities, as one process might accidentally (or maliciously) corrupt shared data.
3. **Complexity Concerns**: Implementing shared memory correctly, especially with needed synchronization primitives, can add complexity to your application. If your IPC needs are straightforward, other simpler methods might suffice.
4. **Portability Issues**: Shared memory mechanisms can sometimes be OS-specific. If you aim for portability across multiple operating systems, you might face challenges with shared memory implementations.

## Things to Watch Out For:

- **Synchronization**: Multiple processes accessing shared memory concurrently can lead to data inconsistencies or race conditions. You'll often need to use synchronization primitives like semaphores or mutexes.
- **Security Concerns**: Since processes share the same memory space, one misbehaving process can potentially corrupt the memory contents, affecting all processes sharing that memory.

## Conclusion

While shared memory offers speed and direct communication advantages, it's essential to weigh its benefits against potential challenges. Assess the requirements and constraints of your project to determine whether shared memory is a fitting IPC choice. 


Happy coding! 🚀👩‍💻


