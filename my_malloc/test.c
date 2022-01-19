#include <stdio.h>
#include "my_malloc.c"

int main() {
    printf("METADATA_SIZE: %zu\n", METADATA_SIZE); //32 bytes
    printf("%p ", sbrk(0));
    metadata_t * curr = sbrk(METADATA_SIZE);
    printf("%p\n", sbrk(0));
    printf("addr %p\n", (void *)curr);
    printf("\n\n----------test result----------\n\n\n");


    return 0;
}
