#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
    void* bptr = sbrk(0);
    
    char* msg = "Hello, world!";
    size_t msg_size = strlen(msg) * sizeof(char);

    bptr = sbrk(msg_size);
    printf("Current break pointer: %p\n", bptr);
    strcpy(bptr, msg);
    printf("Current break pointer: %p\n", bptr);

    printf("Written text in old break pointer: '%s/' and in location %p\n", (char*)bptr, bptr);

    return 0;
}
