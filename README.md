## Outline

1. What is `malloc`?
2. A Process's Memory
    - Heap
    - sbrk and brk
3. Implementation
    - alloc
    - free
    - realloc
4. Testing
5. Conclusion

---

We could use a linked list to implment this.

```C
typedef HeapChunk* chunk_ptr;

typedef struct {
    void* start;
    void* break_pointer;
    void* limit;
} Heap;

// and a heap block

typedef struct {
    size_t size;
    bool isFree;
    chunk_ptr prev;
    chunk_ptr next;
} HeapChunk;
```

## sbrk() and brk()

1. sbrk(): is used to increment the break pointer by a specified increment in bytes. It returns the previous address of the break pointer. It can also be used to get the current address of the break pointer by simply doing `sbrk(0)`.

2. brk(): sets the break pointer of program's heap to the specified address.



# Implementing

## void* alloc(size_t size);

1. Use first fit algorithm to search for sufficient free of memory in the heap.
2. If found: return the pointer to it and mark it as not free.
3. If found block of memory big enough for another block to exist right; split the block into two.
4. If not found: use sbrk to extend the heap with the given size.
5. If allocating memory fails -> return NULL
6. If given size is 0 bytes -> return NULL
7. Zero fill the newly allocated space in memory.


## void free(void* ptr);
1. Free the memory block of given address.
2. If the given ptr is NULL -> do nothing.
3. Solve the issue of fragmentation using **coalesce**.
    - If any of the adjacent memory blocks of just freed memory are also free, join them into a single block of memory.

## void* realloc(void* ptr, size_t size);
1. Free the block referended by the ptr.
2. Use `alloc` to allocated new block of memory of given size.
3. use `memcpy` to copy the data of the old block to the new block.
4. zero fill extended area.

**Edge Cases:**
1. Cannot allocate new block of memory -> do NOT modify the old block.
2. realloc(ptr, 0) = free(ptr) -> return NULL
3. realloc(NULL, n) = alloc(n)
4. realloc(NULL, 0) = alloc(0) -> return NULL
5. argument size < orginal size -> ?
