# 1. What is `malloc`?

What is `malloc`? If you haven't even heard the name, you should probably stop reading this and get familiar with the C programming language and come back to this... tutorial?

For programmers like you and I, **malloc** is a function from the standard C library that lets the programmer **allocate memory chunks**. Instead of reserving memory at compile time, it lets you request a block of memory of a certain size at runtime. It returns a pointer to the start of the allocated memory block. This memory block is uninitialized, i.e., it may contain garbage values. It is the programmer's responsibility to take care of the memory, like using it or freeing it after user, after it has been allocated.

The signature of the function looks something like this:

```c
void* malloc(size_t size);
```

where `size` is the requested size.

I am going to implement a basic version of the standard **malloc** and its related functions.

<br>

# 2. A Process’s Memory

Each process running inside a computer has its own virtual address space, which is dynamically mapped to a physical address by the operating system. This space/memory is divided into several parts:

1. Code/ Program Text
2. Data
3. Heap
4. Stack

<img src="images/img01.png" alt="alt text" width="250">

All we have to know is that a process has space for its code, a region for constant and global variables, a stack for local and temporary data, and an unorganized space for program’s data called the heap. We are mostly interested in the heap segment right now. Before implementing our own **malloc**, we need to understand the heap and how memory is managed within it.

## 2.1 Heap

The heap is a continuous (in terms of virtual address) space of memory. It has three bounds:

1. Start of the heap
2. The break (top of the heap) which marks the end of the mapped region.
3. The maximum limit (rlimit) of the heap, that the break cannot go past.

<img src="images/img02.png" alt="alt text" width="500">

In order to code a **malloc**, we need to

1. know where the heap begins
2. know the position of the break
3. be able to move the break as needed (using `brk` & `sbrk`) to grow or shrink the mapped region

## 2.2 `brk()` and `sbrk()`

`brk()` is used to move the break to the given address. It returns 0 on success and -1 otherwise on failure.

```c
int brk(void *addr);
```

`sbrk()` increments the break pointer by a specified number of bytes. It returns the previous program break on success and an `(void*) -1` (error pointer) on failure.

```c
void *sbrk(intptr_t increment);
```

<img src="images/img03.png" alt="alt text" width="700">

<br>

# 3. Implementation

We need a structure to represent our heap and memory chunks. While there are many ways to structure a memory allocator, I will use a linked list for its simplicity. Let's have a look at how a linked list is used to represent memory chunks.

Let's define a memory block with the following signature

```c
struct M_Block {
    size_t size;
    int isFree;
    Block_Ptr next;
    Block_Ptr prev;
};
```

We include the following members for each memory block:

1. **size**: the size of the memory block (excluding the metadata)
2. **isFree**: 0 for reserved and 1 for free
3. **next** and **prev**: pointers to the next and previous memory block, if any

Let's also create a Heap to keep track for the head and tail of the heap.

```c
struct Heap {
    Block_Ptr head;
    Block_Ptr end;
};
```

<img src="images/img04.png" alt="alt text" width="400">

<br>

Finally, let's see how allocation, deallocation, and reallocation should work on top of this structure.

## 3.1 Allocation (`m_alloc`)

```c
void* m_alloc(size_t size);
```

The `m_alloc` function allocates a block of memory from the heap. The function returns a pointer to the usable memory (the data section), after the metadata region.

- The requested size is first aligned to 16 bytes to ensure proper memory alignment.
- If the heap already has some memory blocks, the allocator searches for a suitable free block using the [first-fit algorithm](https://www.memorymanagement.org/mmref/alloc.html#mmref-alloc-first-fit).
  - If a suitable free block is found, it's used to fulfill the request, and maybe split into two if it's large enough, making the other half a free block.
  - If no suitable block is available, the heap is extended using `sbrk`, and a new block of requested size is created at the end of the heap.

- Returns **NULL** if the allocater cannot allocated the memeory.
- Returns **NULL** if the requested size is 0 or very huge.

## 3.2 Deallocation (`m_free`)

```c
void m_free(void* ptr);
```

The `m_free` function marks a previously allocated block as free.

- If the adjacent blocks are also free, they are merged together using coalescing (`coalesceAdjacentFreeBlock` function) to reduce fragmentation issues.
- The memory region is also filled with 0s to get rid of junk values.
- If the freed block happens to be at the end of the heap, the allocator shrinks the heap by moving the break backward using `brk`.

- If the passed pointer is **NULL**, does nothing.

## 3.3 Reallocation (`m_realloc`)

```c
void* m_realloc(void* ptr, size_t size);
```

The `m_realloc` function resizes an existing memory block. It returns a pointer to the resized block, which may or may not be the same as the original pointer (ptr).

- If the new size is smaller than the old size, and if the left over space is big enough, the original block is split into two to avoid wasting space. The latter block is marked as free for later use.
- If the new size is larger, a new block is allocated using `m_alloc()`, the existing data is copied over using `memcpy`, and the old block is freed.

- Returns **NULL** if it fails to allocate the new size and does NOT modify the existing block.
- Calling `m_realloc(ptr, 0)` is the same as freeing the block and returning **NULL**.
- `m_realloc(NULL, size)` is equivalent to calling `m_alloc(size)`.
- `m_realloc(NULL, 0)` is equivalent to calling `mm_alloc(0)`, which should just return **NULL**.

<hr>
<br>

You can have a look at **src/m_malloc.c** for other helper functions for a complete overview of the implementation. Do not hesitate to clone the repo and play around with it yourself. I know that this is not a full implementation that handles all real-life edge cases, and it’s not optimized. I made this to learn how memory allocaters work fundamentally. If you find any issues in the code, feel free to open an issue or create a pull request.

Thank you for reading. Have a good one, reader!

