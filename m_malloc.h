#ifndef M_ALLOC_H
#define M_ALLOC_H

#include <stddef.h>

// public API
void* m_alloc(size_t size);
void* m_realloc(void* ptr, size_t size);
void m_free(void* ptr);
void* m_calloc (size_t num, size_t size);

#endif
