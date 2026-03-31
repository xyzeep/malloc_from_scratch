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
















