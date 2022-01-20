#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "my_malloc.h"

#define METADATA_SIZE sizeof(metadata_t)
metadata_t *head = NULL;
metadata_t *tail = NULL;
size_t heap_size = 0;

void make_empty_list() {
    head = expand_heap(0);
    tail = expand_heap(0);
    head->next = tail;
    tail->prev = head;

    heap_size += METADATA_SIZE * 2;
}


void print_free_list() {
    //printf("------print_free_list: \n");
    metadata_t *curr = head;
    int i = 0;
    while (curr != NULL) {
        printf("%dth free block: addr = %lu avail = %d size = %zu\n", i, (unsigned long)curr, curr->available, curr->size);

        curr = curr->next;
        i++;
    }
    printf("head: addr = %lu avail = %d size = %lu\n", (unsigned long) head, head->available, head->size);
    printf("tail: addr = %lu avail = %d size = %lu\n", (unsigned long) tail, tail->available, tail->size);
    printf("end of program break: %lu\n\n", (unsigned long)sbrk(0));
}

void print_from_back() {
    //printf("------print_from_back: \n");
    metadata_t *curr = tail;
    int i = 0;
    while (curr != NULL) {
        //printf("%dth free block: addr = %lu avail = %d size = %zu\n", i, (unsigned long)curr, curr->available, curr->size);

        curr = curr->prev;
        i++;
    }
    //printf("head: addr = %lu avail = %d size = %lu\n", (unsigned long) head, head->available, head->size);
    //printf("tail: addr = %lu avail = %d size = %lu\n", (unsigned long) tail, tail->available, tail->size);
    //printf("end of program break: %lu\n\n", (unsigned long)sbrk(0));
}

metadata_t * find_ff(size_t size) {
    // find the first fit available block
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (size <= temp->size) {
            printf("====find the first fit available block\n");
            break;
        }
        temp = temp->next;
    }
    return temp;
}

metadata_t * find_bf(size_t size) {
    printf("====try to find the best fit available block\n");
    int j = 0;
    metadata_t * best_free = NULL;
    unsigned long long smallest_size = ULLONG_MAX;
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        printf("~~~~~~~%dth loop: \n", j);
        printf("temp addr = %lu size = %lu\n", (unsigned long)temp, temp->size);
        if (size <= temp->size) {
            printf("large enough\ncurrent smallest_size = %llu\n", smallest_size);
            if (temp->size < smallest_size) {
                smallest_size = temp->size;
                best_free = temp;
                printf("smaller size - need to update best_free:\n");
                printf("after update: size = %llu addr = %lu\n", smallest_size, (unsigned long)best_free);
                if (size == smallest_size) {
                    printf("break a tie\n");
                    break;
                }
            }
        }
        j++;
        temp = temp->next;
    }
    if (best_free == NULL) {
        best_free = tail;
    }
    return best_free;
}

//First Fit malloc
void *ff_malloc(size_t size) {
    printf("~~~~~~~~~~~~in ff_malloc: ~~~~~~~~~~~~\n");
    if (size <= 0) {
        return NULL;
    }
    metadata_t * new = my_malloc(size, 0);
    return new + 1;
}

//Best Fit malloc
void *bf_malloc(size_t size) {
    printf("~~~~~~~~~~~~in bf_malloc: ~~~~~~~~~~~~\n");
    if (size <= 0) {
        return NULL;
    }
    metadata_t * new = my_malloc(size, 1);
    return new + 1;
}

//malloc: alloc_policy stands for the options of memory allocation policies
// alloc_policy == 0: first fit malloc
// alloc_policy == 1: best fit malloc
void *my_malloc(size_t size, int alloc_policy) {
    printf("~~~~~~~~~~~~in my_malloc: ~~~~~~~~~~~~\n");
    printf("before malloc - current program break: %lu\n", (unsigned long )sbrk(0));
    printf("input size: %zu\n", size);
    if (head == NULL) {
        printf("---it's the very first block in heap: init free_list \n");
        make_empty_list();
        print_free_list();
    }

    // find the best fit available block
    metadata_t * usable;
    //first fit malloc
    if (alloc_policy == 0) {
        usable = find_ff(size);
    }
    //best fit malloc
    if (alloc_policy == 1) {
        usable = find_bf(size);
    }
    printf("after while: \nbest_free addr = %lu size = %llu\n", (unsigned long )usable, usable->size);

    //found available block
    if (usable->size != 0) {
        printf("====found: \n");
        //split or directly remove; (allocate former part)
        if (usable->size > size + METADATA_SIZE) {
            split(usable, size);
        } else {
            printf("====not split (allocate directly (remove from list)): \n");
            //allocate directly (remove from list)
            usable->available = 0;
            usable->prev->next = usable->next;
            usable->next->prev = usable->prev;
        }
        usable->next = NULL;
        usable->prev = NULL;
        print_free_list();
        printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(usable + 1), usable->available, usable->size);
        return usable;
    } else {
        printf("====not found: \n");
        //not found available block
        metadata_t * new_meta = expand_heap(size);
        heap_size = heap_size + METADATA_SIZE + size;
        print_free_list();
        printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(new_meta + 1), new_meta->available, new_meta->size);
        return new_meta;
    }
}

metadata_t * expand_heap(size_t size) {
    metadata_t * new_meta = sbrk(size + METADATA_SIZE);
    if (new_meta == (void *) -1) {
        //printf("sbrk failed\n");
        return NULL;
    }
    new_meta->available = 0;
    new_meta->size = size;
    new_meta->prev = NULL;
    new_meta->next = NULL;
    return new_meta;
}

void split(metadata_t * usable, size_t size) {
    printf("====split: \n");
    //generate new metadata (add to free list) for the left part
    metadata_t * new_meta = (metadata_t *) ((char *) usable + METADATA_SIZE + size);
    new_meta->available = 1;
    new_meta->size = usable->size - size - METADATA_SIZE;
    new_meta->prev = NULL;
    new_meta->next = NULL;

    //replace the usable with new_meta
    usable->available = 0;
    usable->size = size;
    usable->prev->next = new_meta;
    new_meta->prev = usable->prev;
    new_meta->next = usable->next;
    usable->next->prev = new_meta;
}

void coalesce(metadata_t * new_free, metadata_t * temp) {
    // coalesce the adjacent free block (compare to the prev and the next)
    //case 1: coalesce with both prev and next (by removing next)
    if (temp->size != 0 && temp->prev->size != 0 &&
        (char *) temp->prev + METADATA_SIZE + temp->prev->size == (char *) new_free &&
        (char *) new_free + METADATA_SIZE + new_free->size == (char *) temp) {
        //update prev and remove next
        temp->prev->size += (METADATA_SIZE * 2 + new_free->size + temp->size);
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        temp->next = NULL;
        temp->prev = NULL;
    }
        //case 2: coalesce with only prev
    else if (temp->prev->size != 0 && (char *) temp->prev + METADATA_SIZE + temp->prev->size == (char *) new_free) {
        temp->prev->size += (METADATA_SIZE + new_free->size);
    }
        //case 3: coalesce with only next (by replacing the temp with new_free)
    else if (temp->size != 0 && (char *) new_free + METADATA_SIZE + new_free->size == (char *) temp) {
        new_free->size += (METADATA_SIZE + temp->size);
        new_free->next = temp->next;
        temp->next->prev = new_free;
        temp->prev->next = new_free;
        new_free->prev = temp->prev;
    }
        //case 4: no need to coalesce
    else {
        //add before the temp
        new_free->next = temp;
        temp->prev->next = new_free;
        new_free->prev = temp->prev;
        temp->prev = new_free;
    }
}

//First Fit free
void ff_free(void *ptr) {
    my_free(ptr);
}

//Best Fit free
void bf_free(void *ptr) {
    my_free(ptr);
}

//my free
void my_free(void *ptr) {
    printf("............in ff_free: ............\n");
    if (ptr == NULL) {
        return;
    }
    metadata_t *new_free = (metadata_t *) ptr - 1;
    printf("need to free ptr (*new_free) at %lu\n", (unsigned long )new_free);
    new_free->available = 1;
    new_free->prev = NULL;
    new_free->next = NULL;

    // find the location of new_free to add to free_list
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (new_free < temp) {
            printf("find the loc to add new_free: temp at %lu\n", (unsigned long)temp);
            break;
        }
        temp = temp->next;
    }

    coalesce(new_free, temp);
    print_free_list();
    printf("\n");
}

unsigned long get_data_segment_size() {
    return heap_size;
}

unsigned long get_data_segment_free_space_size() {
    unsigned long total = METADATA_SIZE * 2;
    metadata_t * temp = head->next;
    while (temp->size != 0) {
        total += (METADATA_SIZE + temp->size);
        temp = temp->next;
    }
    return total;
}