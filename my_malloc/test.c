#include <stdio.h>
#include "my_malloc.c"

int main() {
    printf("METADATA_SIZE: %zu\n", METADATA_SIZE); //32 bytes
    printf("%p ", sbrk(0));
    metadata_t * curr = sbrk(METADATA_SIZE);
    printf("%p\n", sbrk(0));
    printf("addr %p\n", (void *)curr);
    printf("\n\n----------test result----------\n\n\n");

    printf("%zu\n", sizeof(char)); //1
    printf("%p\n", sbrk(0));
    sbrk(sizeof(char));
    printf("%p\n\n", sbrk(0));

    printf("%zu\n", sizeof(int)); //4
    printf("%p\n", sbrk(0));
    sbrk(sizeof(int));
    printf("%p\n\n", sbrk(0));

    printf("%zu\n", sizeof(int *)); //8
    printf("%p\n", sbrk(0));
    sbrk(sizeof(int *));
    printf("%p\n\n", sbrk(0));

    printf("%zu\n", sizeof(metadata_t)); //
    printf("%p\n", sbrk(0));
    sbrk(sizeof(metadata_t));
    printf("%p\n\n", sbrk(0));


    return 0;
}
