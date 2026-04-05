#include <stdio.h>
#include <string.h>
#include "m_malloc.h"

void print_line() {
    printf("--------------------------------------------------\n");
}

// Helper to print array contents
void print_int_array(int *arr, size_t size, const char *name) {
    printf("%s: [", name);
    for (size_t i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i != size - 1) printf(", ");
    }
    printf("]\n");
}

int main() {
    printf("CUSTOM MALLOC TEST SUITE\n");
    print_line();

    printf("1. Testing m_alloc()\n");
    int *a = (int*) m_alloc(5 * sizeof(int));
    printf("Allocated 'a' (5 ints) at %p\n", (void*)a);
    for (int i = 0; i < 5; i++) a[i] = i+1;
    print_int_array(a, 5, "a");
    print_line();

    printf("2. Testing m_calloc()\n");
    int *b = (int*) m_calloc(4, sizeof(int));
    printf("Allocated 'b' (4 ints) at %p\n", (void*)b);
    print_int_array(b, 4, "b");  // should all be 0
    print_line();

    printf("3. Testing multiple allocations\n");
    int *c = (int*) m_alloc(3 * sizeof(int));
    int *d = (int*) m_alloc(2 * sizeof(int));
    printf("Allocated 'c' (3 ints) at %p\n", (void*)c);
    printf("Allocated 'd' (2 ints) at %p\n", (void*)d);
    print_line();

    printf("4. Writing to allocated memory\n");
    for (int i = 0; i < 3; i++) c[i] = i + 100;
    for (int i = 0; i < 2; i++) d[i] = i + 200;
    print_int_array(c, 3, "c");
    print_int_array(d, 2, "d");
    print_line();

    printf("Heap after allocations:\n");
    m_print_heap();

    printf("5. Freeing 'b' and 'c'\n");
    m_free(b);
    printf("Freed 'b'\n");
    m_free(c);
    printf("Freed 'c'\n");
    print_line();

    m_free(b);
    printf("Heap after freeing 'b':\n");
    m_print_heap();

    printf("6. Testing m_realloc (grow 'a')\n");
    a = (int*) m_realloc(a, 10 * sizeof(int));
    printf("'a' reallocated to 10 ints at %p\n", (void*)a);
    for (int i = 5; i < 10; i++) a[i] = i + 10;
    print_int_array(a, 10, "a");
    print_line();

    printf("7. Testing m_realloc (shrink 'd')\n");
    d = (int*) m_realloc(d, sizeof(int));
    printf("'d' shrunk to 1 int at %p\n", (void*)d);
    print_int_array(d, 1, "d");
    print_line();
    
    printf("Heap after allocations:\n");
    m_print_heap();

    printf("8. Freeing remaining blocks\n");
    m_free(a);
    printf("Freed 'a'\n");
    m_free(d);
    printf("Freed 'd'\n");
    print_line();
    

    printf("Final Heap:\n");
    m_print_heap();

    printf("All tests completed successfully!\n");
    return 0;
}
