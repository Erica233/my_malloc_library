#include <stdio.h>
#include "my_malloc.c"

int main() {
    printf("METADATA_SIZE: %zu\n", METADATA_SIZE); //32 bytes
    void * p1 = sbrk(0);
    printf("%p ", p1);
    metadata_t * curr = sbrk(METADATA_SIZE);
    printf("%p\n", curr);
    printf("diff %lu\n", (unsigned long)curr-(unsigned long)p1);
    printf("\n\n----------test result----------\n\n\n");

    printf("%zu\n", sizeof(metadata_t)); //
    printf("%p\n", sbrk(0));
    sbrk(sizeof(metadata_t));
    printf("%p\n\n", sbrk(0));


    return 0;
}
