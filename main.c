#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

int main(void)
{
    char *a = my_malloc(100);
    char *b = my_malloc(100);
    char *c = my_malloc(50);

    strcpy(a, "first block");
    strcpy(b, "second block");
    strcpy(c, "third block");

    printf("a=%p  b=%p  c=%p\n", (void *)a, (void *)b, (void *)c);
    printf("%s | %s | %s\n\n", a, b, c);

    /* Free a and b. Neither alone is big enough for 200 bytes,
       but merged they are. */
    my_free(a);
    my_free(b);

    char *d = my_malloc(200);
    printf("d=%p (should match a, since a and b merged)\n", (void *)d);
    printf("c still holds: %s\n", c);

    return 0;
}