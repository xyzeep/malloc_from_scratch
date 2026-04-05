#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "m_malloc.h"

// macros
#define  Block_Size (sizeof(struct M_Block))
#define ALIGNMENT 16
#define MIN_BLOCK_SIZE 16
#define ALIGN16(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// type definitons
typedef struct M_Block* Block_Ptr; // opaque

// struct definitions
struct M_Block {
    size_t size;  // size of only the payload/data
    int isFree;
    Block_Ptr next;
    Block_Ptr prev;
};


struct Heap {
    Block_Ptr head;
    Block_Ptr end;
};


// global variables
static struct Heap heap = {NULL, NULL};

// function prototypes
static void splitBlocksIntoTwo(Block_Ptr oldBlock, size_t size);
static void zeroFill(Block_Ptr m_block);
static Block_Ptr findFirstFit(size_t size);
static Block_Ptr coalesceAdjacentFreeBlock(Block_Ptr m_block);
static Block_Ptr createMoreMemory(size_t size);

// ######################
// function definitions
// ######################

void m_print_heap(void) {
    Block_Ptr current = heap.head;
    printf("HEAP STATUS:\n");
    while (current) {
        printf("Block at %p | size: %zu | %s\n",
               (void*)current,
               current->size,
               current->isFree ? "FREE" : "USED");
        current = current->next;
    }
    printf("End of heap\n");
}

static void zeroFill(Block_Ptr m_block){
    char* charPtr = (char*) (m_block +  1);  // get data address
    for (size_t i = 0; i < (m_block->size) ; i++){
        charPtr[i] = 0;
    }
}

static Block_Ptr findFirstFit(size_t size){
    if (heap.head == NULL)  // head is not initialized
        return NULL;

    Block_Ptr current = heap.head;

    while (current) {  // traverse the linked list

        // if the current block is free and has required space
        if (current->isFree && current->size >= size) {  
            break;
        }
        current = current->next;  // else move to next block
    }

    if (current == NULL) {
        // failed to find a fit
        return NULL;
    }

    return current;
}


static void splitBlocksIntoTwo(Block_Ptr oldBlock, size_t size){
    if (oldBlock == NULL)
        return;

    Block_Ptr newBlock = (Block_Ptr) ((char*)oldBlock + Block_Size + size);

    newBlock->next = oldBlock->next;
    oldBlock->next = newBlock;
    newBlock->prev = oldBlock;
    newBlock->isFree = 1;
    if (newBlock->next)
        newBlock->next->prev = newBlock;
    newBlock->size = oldBlock->size - (size + Block_Size);
    oldBlock->size = size;
    if (heap.end == oldBlock)
        heap.end = newBlock;

    zeroFill(newBlock);
}

static Block_Ptr createMoreMemory(size_t size){

    // increment brk by size + metadata and
    // give its return value (a pointer), i.e., prev loc of brk, to new_block
    Block_Ptr new_block = sbrk(Block_Size + size);

    // if the increment was not successful, return NULL
    if (new_block == (void*) -1) return NULL;

    new_block->size = size;  // give the new block the size of size
    new_block->next = NULL;  // new_block's next is NULL bcus it's the last block
    new_block->prev = heap.end;  // new_block's prev is old last block
    new_block->isFree = 0;  // set the new_block's free to 0 (false)


    if (heap.end)
        heap.end->next = new_block;  // new_block is the next for prev last block
    heap.end = new_block;  // new_block is the new last block of heap

    if (heap.head == NULL)
        heap.head = new_block;

    return new_block;
}


void* m_calloc (size_t num, size_t size){

    if (num == 0 || size == 0) return NULL;
    if (num > (SIZE_MAX / size)) return NULL;

    size_t total_size = num * size;
    void* ptr = m_alloc(total_size);

    if (ptr == NULL)
        return NULL;
    zeroFill((Block_Ptr)ptr - 1);
    return ptr;
}

void* m_alloc(size_t size){
    if (size == 0){
        return NULL;
    }

    size = ALIGN16(size);

    if (size > (SIZE_MAX - Block_Size))
        return NULL;

    if (heap.head != NULL){  // if the first block is initialized

        // check if an already-free block exists that fits the requirement
        Block_Ptr fit_block = findFirstFit(size);

        if (fit_block != NULL) {  // if there is a block that fits

            if (fit_block->size > (size + Block_Size + MIN_BLOCK_SIZE))  // and it is big enough to be split into two
                splitBlocksIntoTwo(fit_block, size);  // slpit into two

            // set the fit block's isFree to 0
            fit_block->isFree = 0;

            // return the pointer to the "data" part of the fit block (after metadata)
            return (void*)(fit_block + 1);  
        }
    }
    Block_Ptr new_block = createMoreMemory(size);
    if (new_block == NULL)
        return NULL;

    return (void*) (new_block + 1);
}

void m_free(void* ptr){
    if (ptr == NULL)
        return;

    Block_Ptr m_block = (Block_Ptr) ptr;
    m_block = m_block - 1;  // get block address
    m_block->isFree = 1;
    m_block = coalesceAdjacentFreeBlock(m_block);
    zeroFill(m_block);


    if (m_block->next == NULL) {
        Block_Ptr prev = m_block->prev;

        if (brk(m_block) != 0)
            return;

        if (prev == NULL){
            heap.head = NULL;
            heap.end = NULL;
        }
        else {
            prev->next = NULL;
            heap.end = prev;
        }

    }
}



void* m_realloc(void* ptr, size_t size) {

    Block_Ptr old_m_block;
    void* new_m_block;
    size_t old_size;

    if (ptr == NULL) {
        return m_alloc(size);
    }
    if (size == 0) {
        m_free(ptr);
        return NULL;
    }

    if (heap.head == NULL)
        return NULL;

    size = ALIGN16(size);

    old_m_block = (Block_Ptr)ptr - 1;
    old_size = old_m_block->size;

    if (size <= old_size){
        if (old_m_block->size > (size + Block_Size + MIN_BLOCK_SIZE)){
            splitBlocksIntoTwo(old_m_block, size);
        }
        return ptr;
    }

    if ((new_m_block = m_alloc(size)) == NULL)
        return NULL;

    memcpy(new_m_block, ptr, old_size); 
    m_free(ptr);

    return new_m_block;
}

Block_Ptr coalesceAdjacentFreeBlock(Block_Ptr m_block){

    if (m_block->next && m_block->next->isFree) {
        Block_Ptr next_m_block = m_block->next;

        m_block->size += next_m_block->size + Block_Size;
        m_block->next = next_m_block->next;
        if (next_m_block->next)
            next_m_block->next->prev = m_block;
    }

    if (m_block->prev && m_block->prev->isFree) {
        Block_Ptr prev_m_block = m_block->prev;

        prev_m_block->size += m_block->size + Block_Size;
        prev_m_block->next = m_block->next;
        if (m_block->next)
            m_block->next->prev = prev_m_block;
        m_block = prev_m_block;
    }

    return m_block;
}

// ###########################################################

