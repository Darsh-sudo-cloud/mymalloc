#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

#define SLOTS   50
#define ROUNDS  10000
#define MAX_REQ 512

typedef struct {
    unsigned char *ptr;
    size_t         size;
    unsigned char  tag;
} slot;

static int check_slot(const slot *s)
{
    for (size_t i = 0; i < s->size; i++)
        if (s->ptr[i] != s->tag)
            return 0;
    return 1;
}

int main(void)
{
    slot slots[SLOTS];
    int failures = 0;

    memset(slots, 0, sizeof(slots));
    srand(12345);  /* fixed seed so failures are reproducible */

    for (int round = 0; round < ROUNDS; round++) {
        int i = rand() % SLOTS;

        if (slots[i].ptr == NULL) {
            size_t size = 1 + rand() % MAX_REQ;
            unsigned char *p = my_malloc(size);

            if (p == NULL)
                continue;  /* heap full right now, that's fine */

            slots[i].ptr  = p;
            slots[i].size = size;
            slots[i].tag  = (unsigned char)(i + 1);
            memset(p, slots[i].tag, size);
        } else {
            if (!check_slot(&slots[i])) {
                printf("CORRUPTION in slot %d at round %d\n", i, round);
                failures++;
            }
            my_free(slots[i].ptr);
            slots[i].ptr = NULL;
        }
    }

    /* Free whatever is still allocated. */
    for (int i = 0; i < SLOTS; i++) {
        if (slots[i].ptr != NULL) {
            if (!check_slot(&slots[i])) {
                printf("CORRUPTION in slot %d during cleanup\n", i);
                failures++;
            }
            my_free(slots[i].ptr);
            slots[i].ptr = NULL;
        }
    }

    /* After everything is freed, one big allocation should succeed. */
    unsigned char *big = my_malloc(60 * 1024);
    if (big == NULL) {
        printf("FAIL: heap is fragmented, coalescing is not working\n");
        failures++;
    } else {
        printf("OK: 60 KB allocation succeeded after all frees\n");
        my_free(big);
    }

    printf("Total failures: %d\n", failures);
    return failures != 0;
}