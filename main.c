#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define  Block_Size (sizeof(struct M_Block))

void* head = NULL;

typedef struct M_Block* Block_Ptr;

struct M_Block {
    size_t size;
    int isFree;
    Block_Ptr next;
    Block_Ptr prev;
};



Block_Ptr findFirstFit(Block_Ptr head, size_t size){
    if (head == NULL)  // head is not initialized
        return NULL;

    Block_Ptr current = head;

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
    
    Block_Ptr newBlock = (Block_Ptr) ((char*)oldBlock + size);

    newBlock->next = oldBlock->next;
    oldBlock->next = newBlock;
    newBlock->prev = oldBlock;
    newBlock->isFree = 1;
    newBlock->size = oldBlock->size - (size + Block_Size);
    oldBlock->size = size;
}


void* m_alloc(size_t size){
    if (size == 0){
        return NULL;
    }

    if (head != NULL){  // if the first block is initialized
        Block_Ptr fit_block = findFirstFit(head, size);
        
        if (fit_block != NULL) {
            if (fit_block->size > size + Block_Size) {
                // split into two
                splitBlocksIntoTwo(fit_block, size);
            }
        }
        return NULL;
    }

    return NULL;
}


int main()
{
    printf("Implementing malloc from scratch.\n");
    return 0;
}
