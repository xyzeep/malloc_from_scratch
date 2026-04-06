# What is `malloc`?

What is `malloc`? If you haven't even heard the name, you should probably stop reading this and get familiar with the C programming language and come back to this... tutorial?

For programmers like you and I, `malloc` is a function from the standard C library that lets the programmer **allocate memory chunks**. Instead of reserving memory at compile time, it lets you request a block of memory of certain size at runtime. It returns a pointer to the start of the allocated memory block. This memory block is uninitialized, i.e., it may contain garbage value. It is the programmer's responsibity to take care of the memory, like using it or freeing after user, after it has been allocated.

The signature of the function looks something like this:

```c
void* malloc(size_t size);
```

Where `size` is the requested size.

I am going to implement a basic version of the standard malloc and its related functions.

# A Process’s Memory

Each process running inside a computer has its own virtual address space, which is dynamically mapped to physical address by the operating system. This space/memory is divided into several parts:

1. Code/ Program Text
2. Data
3. Heap
4. Stack

<img src="images/img01.png" alt="alt text" width="200">

All we have to know is that a process has space for its code, a region for constant and global variables, a stack for local and temporary data, and an unorganized space for program’s data called the heap. We are mostly interested on the heap segment right now. Before implementing our own `malloc`, we need to understand the heap and how memory is managed within it.

# Heap

The heap is a continuous (in terms of virtual address) space of memory. It has three bounds:

1. Start of the heap
2. The break (top of the heap) which marks the end of the mapped region.
3. The maximum limit (rlimit) of the heap, that the break cannot go past.

<img src="images/img02.png" alt="alt text" width="600">

In order to code a `malloc`, we need to

1. know where the heap begins
2. know the position of the break
3. be able to move the break as needed (using `brk` & `sbrk`) to grow or shrink the mapped region

## `brk()` and `sbrk()`

`brk()` is used to move the break to the given address. It returns 0 on success and -1 otherwise on failure.

```c
int brk(void *addr);
```

`sbrk()` increments the break pointer by a specified number of bytes. It returns the previous program break on success and an `(void*) -1` (error pointer) on failure.

```c
void *sbrk(intptr_t increment);
```

<img src="images/img03.png" alt="alt text" width="800">

# Implementation

