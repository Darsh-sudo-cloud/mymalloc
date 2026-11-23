#include <stddef.h>
#include "mymalloc.h"

#define HEAP_SIZE  (64 * 1024)
#define ALIGN      8
#define ALIGN_UP(n) (((n) + ALIGN - 1) & ~(size_t)(ALIGN - 1))

typedef struct block_header {
    size_t size;     /* payload size in bytes (not counting the header) */
    int    is_free;  /* 1 = free, 0 = in use */
} block_header;

#define HEADER_SIZE ALIGN_UP(sizeof(block_header))

static unsigned char heap[HEAP_SIZE];
static int heap_initialized = 0;

static void init_heap(void)
{
    block_header *first = (block_header *)heap;
    first->size    = HEAP_SIZE - HEADER_SIZE;
    first->is_free = 1;
    heap_initialized = 1;
}

/* Merge each free block with any free blocks directly after it. */
static void coalesce(void)
{
    unsigned char *cur = heap;
    unsigned char *end = heap + HEAP_SIZE;

    while (cur < end) {
        block_header *b = (block_header *)cur;
        unsigned char *next = cur + HEADER_SIZE + b->size;

        if (next >= end)
            break;

        block_header *nb = (block_header *)next;

        if (b->is_free && nb->is_free) {
            b->size += HEADER_SIZE + nb->size;  /* absorb the next block */
        } else {
            cur = next;
        }
    }
}

void *my_malloc(size_t size)
{
    unsigned char *cur, *end;

    if (!heap_initialized)
        init_heap();
    if (size == 0)
        return NULL;

    size = ALIGN_UP(size);
    cur = heap;
    end = heap + HEAP_SIZE;

    while (cur < end) {
        block_header *b = (block_header *)cur;

        if (b->is_free && b->size >= size) {
            /* Split if the leftover is big enough for a header plus some payload. */
            if (b->size >= size + HEADER_SIZE + ALIGN) {
                block_header *next = (block_header *)(cur + HEADER_SIZE + size);
                next->size    = b->size - size - HEADER_SIZE;
                next->is_free = 1;
                b->size = size;
            }
            b->is_free = 0;
            return cur + HEADER_SIZE;
        }

        cur += HEADER_SIZE + b->size;
    }

    return NULL;  /* out of memory */
}

void my_free(void *ptr)
{
    block_header *b;

    if (ptr == NULL)
        return;

    b = (block_header *)((unsigned char *)ptr - HEADER_SIZE);
    b->is_free = 1;
    coalesce();
}