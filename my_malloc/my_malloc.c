#include <unistd.h>
#include <stdio.h>
#include "my_malloc.h"

typedef struct metadata metadata_t;
struct metadata {
    int available;
    size_t size;
    metadata_t *next;
    metadata_t *prev;
};
#define METADATA_SIZE sizeof(metadata_t)
metadata_t *head = NULL;
metadata_t *tail = NULL;
size_t heap_size = 0;
size_t free_size = 0;

void print_free_list();

void *ff_malloc(size_t size);

void ff_free(void *ptr);

unsigned long get_data_segment_size();

unsigned long get_data_segment_free_space_size();

void make_empty_list() {
    head = sbrk(METADATA_SIZE);
    head->available = 0;
    head->size = 0;
    tail = sbrk(METADATA_SIZE);
    tail->available = 0;
    tail->size = 0;

    head->next = tail;
    head->prev = NULL;
    tail->prev = head;
    tail->next = NULL;

    heap_size += METADATA_SIZE * 2;
    free_size += METADATA_SIZE * 2;
}

/*
metadata_t *find_ff() {
}
*/

void print_free_list() {
    printf("\n------print_free_list: \n");
    metadata_t *curr = head;
    int i = 0;
    while (curr != 0) {
        printf("%dth free block: addr = %lu avail = %d size = %zu ", i, (unsigned long)curr, curr->available, curr->size);
        printf("diff = %lu\n", (unsigned long)curr->next - (unsigned long)curr);

        curr = curr->next;
        i++;
    }
    printf("head: addr = %p avail = %d size = %lu\n", (unsigned long) head, head->available, head->size);
    printf("tail: addr = %p avail = %d size = %lu\n", (unsigned long) tail, tail->available, tail->size);
    printf("end of program break: %lu\n\n", (unsigned long)sbrk(0));
}

//First Fit malloc
void *ff_malloc(size_t size) {
    printf("~~~~~~~~~~~~in ff_malloc: ~~~~~~~~~~~~\n");
    printf("before malloc - current program break: %p\n", sbrk(0));
    printf("input size: %zu\n", size);
    metadata_t *new_meta;

    if (head == NULL) {
        printf("---it's the very first block in heap: init free_list \n");
        make_empty_list();
        print_free_list();
    }

    // find the first fit available block
    //new_meta = find_ff(size);
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (size <= temp->size) {
            printf("====find the first fit available block\n");
            break;
        }
        temp = temp->next;
    }
    //found available block
    if (temp->size != 0) {
        printf("====found: \n");
        //split() or directly remove;
        if (temp->size > size + METADATA_SIZE) {
            printf("====split: \n");
            //split();
            //generate new metadata for the left part
            new_meta = (metadata_t *) ((char *) temp + size + METADATA_SIZE);
            new_meta->available = 1;
            new_meta->size = temp->size - size - METADATA_SIZE;
            new_meta->prev = NULL;
            new_meta->next = NULL;

            //replace the temp with new_meta
            temp->available = 0;
            temp->size = size;
            temp->prev->next = new_meta;
            new_meta->prev = temp->prev;
            new_meta->next = temp->next;
            temp->next->prev = new_meta;

            free_size = free_size - METADATA_SIZE - size;
        } else {
            printf("====not split (allocate directly (remove from list)): \n");
            //allocate directly (remove from list)
            temp->available = 0;
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;

            free_size = free_size - METADATA_SIZE - temp->size;
        }
        print_free_list();
        printf("after malloc - current program break: %lu\n\n\n", (unsigned long)sbrk(0));
        printf("return malloc()'s addr: %lu\n", (unsigned long)(temp + 1));
        return temp + 1;
        //not found available block
    } else {
        printf("====not found: \n");
        // if there is no available block, then call sbrk() to create
        // free_list is empty, or blocks in free_list are all smaller than required
        new_meta = sbrk(size + METADATA_SIZE);
        new_meta->available = 0;
        new_meta->size = size;
        new_meta->prev = NULL;
        new_meta->next = NULL;

        heap_size = heap_size + METADATA_SIZE + size;

        print_free_list();
        printf("after malloc - current program break: %lu\n\n\n", (unsigned long)sbrk(0));
        printf("return malloc()'s addr: %lu\n", (unsigned long)(new_meta + 1));
        return new_meta + 1;
    }
}

//First Fit free
void ff_free(void *ptr) {
    printf("\n............in ff_free: ............\n");
    metadata_t *new_free = (metadata_t *) ptr - 1;
    printf("need to free ptr (*new_free) at %lu\n", (unsigned long )new_free);
    new_free->available = 1;
    free_size += (METADATA_SIZE + new_free->size);

    // find the location of new_free to add to free_list
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (new_free < temp) {
            printf("find the loc to add new_free: temp at %lu\n", (unsigned long)temp);
            break;
        }
        temp = temp->next;
    }

    // coalesce the adjacent free block (compare to the prev and the next)
    //case 1: coalesce with both prev and next (by removing next)
    if (temp->size != 0 && temp->prev->size != 0 &&
        (char *) temp->prev + METADATA_SIZE + temp->size == (char *) new_free &&
        (char *) new_free + METADATA_SIZE + new_free->size == (char *) temp) {
        printf("case 1: merge prev and next:\n");
        //update prev and remove next
        temp->prev->size += (METADATA_SIZE * 2 + new_free->size + temp->size);
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
    }
        //case 2: coalesce with only prev
    else if (temp->prev->size != 0 && (char *) temp->prev + METADATA_SIZE + temp->size == (char *) new_free) {
        printf("case 2: merge prev:\n");
        temp->prev->size += (METADATA_SIZE + new_free->size);
    }
        //case 3: coalesce with only next (by replacing the temp with new_free)
    else if (temp->size != 0 && (char *) new_free + METADATA_SIZE + new_free->size == (char *) temp) {
        printf("case 3: merge next:\n");
        new_free->size += (METADATA_SIZE + temp->size);
        new_free->next = temp->next;
        temp->next->prev = new_free;
        temp->prev->next = new_free;
        new_free->prev = temp->prev;
    }
        //case 4: no need to coalesce
    else {
        printf("case 4: no merge - add to list:\n");
        //add before the temp
        new_free->next = temp;
        temp->prev->next = new_free;
        new_free->prev = temp->prev;
        temp->prev = new_free;
    }
    print_free_list();
}

/*
//Best Fit malloc
void *bf_malloc(size_t size) {

}

//Best Fit free
void bf_free(void *ptr) {
}*/

unsigned long get_data_segment_size() {
    return heap_size;
}

unsigned long get_data_segment_free_space_size() {
    return free_size;
}
