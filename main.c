#include <stdio.h>
#include <unistd.h>
#include <string.h>

// macros
#define  Block_Size (sizeof(struct M_Block))

// type definitons
typedef struct M_Block* Block_Ptr;

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
struct Heap heap = {NULL, NULL};

// function prototypes
Block_Ptr findFirstFit(Block_Ptr head, size_t size);
void splitBlocksIntoTwo(Block_Ptr oldBlock, size_t size);
Block_Ptr coalesceAdjacentFreeBlock(Block_Ptr m_block);
void* m_alloc(size_t size);
void zeroFill(Block_Ptr m_block);
void createMoreMemory(size_t size);

// ######################
// function definitions
// ######################

void zeroFill(Block_Ptr m_block){
    char* charPtr = (char*) (m_block +  1);  // get data address
    for (int i = 0; i < (m_block->size) ; i++){
        charPtr[i] = 0;
    }
}

Block_Ptr findFirstFit(Block_Ptr head, size_t size){
    if (head == NULL)  // head is not initialized
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


void splitBlocksIntoTwo(Block_Ptr oldBlock, size_t size){
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
}

void createMoreMemory(size_t size){
}

void* m_alloc(size_t size){
    if (size == 0){
        return NULL;
    }

    if (heap.head != NULL){  // if the first block is initialized

        // check if an already-free block exists that fits the requirement
        Block_Ptr fit_block = findFirstFit(heap.head, size);

        if (fit_block != NULL) {  // if there is a block that fits

            if (fit_block->size > size + Block_Size) { // and it is big enough to be split into two
                // split into two
                splitBlocksIntoTwo(fit_block, size);
            }
            // set the fit block's isFree to 0
            fit_block->isFree = 0;

            // return the pointer to the "data" part of the fit block (after metadata)
            return (void*)(fit_block + 1);  
        }

        // if not fit block was found, extend brk (heap end)
        else {  
            // increment brk by size + metadata and
            // give its return value (a pointer), i.e., prev loc of brk, to new_block
            Block_Ptr new_block = sbrk(Block_Size + size);

            // if the increment was not successful, return NULL
            if (new_block == (void*) -1) return NULL;

            new_block->size = size;  // give the new block the size of size
            new_block->next = NULL;  // new_block's next is NULL bcus it's the last block
            new_block->prev = heap.end;  // new_block's prev is old last block
            new_block->isFree = 0;  // set the new_block's free to 0 (false)
            heap.end->next = new_block;  // new_block is the next for prev last block
            heap.end = new_block;  // new_block is the new last block of heap
            
            // fill zero allocated
            zeroFill(new_block);

            return new_block + 1;  // return the address of the new_block after the metadata
        }
    }
    else {  // if no blocks in heap, i.e. first call of m_alloc 

        // same thing
        Block_Ptr new_block = sbrk(Block_Size + size);

        if (new_block == (void*) -1) return NULL;
        new_block->size = size;
        new_block->next = NULL;
        new_block->prev = NULL;
        new_block->isFree = 0;

        // new_block is both the head and the end
        heap.head = new_block;
        heap.end = new_block;

        // fill zero allocated
        zeroFill(new_block);

        return new_block + 1;
    }
    return NULL;
}

void m_free(void* ptr){
    if (ptr == NULL)
        return;

    Block_Ptr m_block = (Block_Ptr) ptr;
    m_block = m_block - 1;  // get block address
    m_block->isFree = 1;
    
    m_block = coalesceAdjacentFreeBlock(m_block);

    if (m_block->next == NULL) {
        if (m_block->prev == NULL){
            heap.head = NULL;
            heap.end = NULL;
        }
        else {
            m_block->prev->next = NULL;
        }
        brk(m_block);
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

    old_m_block = (Block_Ptr)ptr - 1;
    old_size = old_m_block->size;

    if (size <= old_size){
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

int main()
{
    printf("Implementing malloc from scratch.\n");
    return 0;
}
