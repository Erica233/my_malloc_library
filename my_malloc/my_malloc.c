#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "my_malloc.h"

#define METADATA_SIZE sizeof(metadata_t)
metadata_t *head = NULL;
metadata_t *tail = NULL;
size_t heap_size = 0;
size_t free_size = 0;


void make_empty_list() {
    head = sbrk(METADATA_SIZE);
    if (head == (void *) -1) {
        ////printf("sbrk failed\n");
        return;
    }
    head->available = 0;
    head->size = 0;

    tail = sbrk(METADATA_SIZE);
    if (tail == (void *) -1) {
        ////printf("sbrk failed\n");
        return;
    };
    tail->available = 0;
    tail->size = 0;

    head->next = tail;
    head->prev = NULL;
    tail->prev = head;
    tail->next = NULL;

    heap_size += METADATA_SIZE * 2;
    free_size += METADATA_SIZE * 2;
}


void print_free_list() {
    //printf("------print_free_list: \n");
    metadata_t *curr = head;
    int i = 0;
    while (curr != NULL) {
        printf("%dth free block: addr = %lu avail = %d size = %zu\n", i, (unsigned long)curr, curr->available, curr->size);
        //printf("diff = %lu\n", (unsigned long)curr->next - (unsigned long)curr);

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
        ////printf("diff = %lu\n", (unsigned long)curr->next - (unsigned long)curr);

        curr = curr->prev;
        i++;
    }
    //printf("head: addr = %lu avail = %d size = %lu\n", (unsigned long) head, head->available, head->size);
    //printf("tail: addr = %lu avail = %d size = %lu\n", (unsigned long) tail, tail->available, tail->size);
    //printf("end of program break: %lu\n\n", (unsigned long)sbrk(0));
}

metadata_t * find_ff(size_t size) {
    // find the first fit available block
    //new_meta = find_ff(size);
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (size <= temp->size) {
            //printf("====find the first fit available block\n");
            break;
        }
        temp = temp->next;
    }
    return temp;
}

metadata_t * find_bf(size_t size) {
    //printf("====try to find the best fit available block\n");
    int j = 0;
    metadata_t * best_free = NULL;
    unsigned long long smallest_size = ULLONG_MAX;
    //printf("initial smallest_size = %llu\n", smallest_size);
    metadata_t *temp = head->next;
    //printf("go to while loop: \n");
    while (temp->size != 0) {
        //printf("~~~~~~~%dth loop: \n", j);
        //printf("temp addr = %lu size = %lu\n", (unsigned long)temp, temp->size);
        if (size <= temp->size) {
            //printf("a block:  large enough\n");
            //printf("current smallest_size = %llu\n", smallest_size);
            if (temp->size < smallest_size) {
                //printf("smaller size - need to update best_free:\n");
                smallest_size = temp->size;
                best_free = temp;
                //printf("after update: size = %llu addr = %lu\n", smallest_size, (unsigned long)best_free);
                if (size == smallest_size) {
                    break;
                }
            }
        }
        j++;
        temp = temp->next;
    }
    if (best_free == NULL) {
        best_free = head;
    }
    return best_free;
}

//malloc: alloc_policy stands for the options of memeory allocation policies
// alloc_policy == 0: first fit malloc
// alloc_policy == 1: best fit malloc
void *my_malloc(size_t size, int alloc_policy) {
    //printf("~~~~~~~~~~~~in my_malloc: ~~~~~~~~~~~~\n");
    //printf("before malloc - current program break: %lu\n", (unsigned long )sbrk(0));
    //printf("input size: %zu\n", size);
    if (head == NULL) {
        //printf("---it's the very first block in heap: init free_list \n");
        make_empty_list();
        //print_free_list();
        //print_from_back();
    }

    metadata_t * usable;
    //first fit malloc
    if (alloc_policy == 0) {
        usable = find_ff(size);
        // find the first fit available block
        //new_meta = find_ff(size);
    }
    //best fit malloc
    if (alloc_policy == 1) {
        usable = find_bf(size);
        // find the best fit available block
        //new_meta = find_ff(size);
    }

    //found available block
    if (usable->size != 0) {
        //printf("====found: \n");
        //split() or directly remove; (allocate former part)
        if (usable->size > size + METADATA_SIZE) {
            //printf("====split: \n");
            //split();
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

            free_size = free_size - METADATA_SIZE - size;
        } else {
            //printf("====not split (allocate directly (remove from list)): \n");
            //allocate directly (remove from list)
            usable->available = 0;
            usable->prev->next = usable->next;
            usable->next->prev = usable->prev;

            free_size = free_size - METADATA_SIZE - usable->size;
        }
        usable->next = NULL;
        usable->prev = NULL;
        //print_free_list();
        //print_from_back();
        //printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        //printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(temp + 1), temp->available, temp->size);
        return usable + 1;
        //not found available block
    } else {
        //printf("====not found: \n");
        // if there is no available block, then call sbrk() to create
        // free_list is empty, or blocks in free_list are all smaller than required
        metadata_t * new_meta = sbrk(size + METADATA_SIZE);
        if (new_meta == (void *) -1) {
            ////printf("sbrk failed\n");
            return NULL;
        }
        new_meta->available = 0;
        new_meta->size = size;
        new_meta->prev = NULL;
        new_meta->next = NULL;

        heap_size = heap_size + METADATA_SIZE + size;

        //print_free_list();
        ////print_from_back();
        //printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        //printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(new_meta + 1), new_meta->available, new_meta->size);
        return new_meta + 1;
    }
}

//First Fit malloc
void *ff_malloc(size_t size) {
    //printf("~~~~~~~~~~~~in ff_malloc: ~~~~~~~~~~~~\n");
    //printf("before malloc - current program break: %lu\n", (unsigned long )sbrk(0));
    //printf("input size: %zu\n", size);
    if (size <= 0) {
        return NULL;
    }
    //metadata_t *new_meta;

    my_malloc(size, 0);
    /*
    if (head == NULL) {
        //printf("---it's the very first block in heap: init free_list \n");
        make_empty_list();
        //print_free_list();
        //print_from_back();
    }

    // find the first fit available block
    //new_meta = find_ff(size);
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (size <= temp->size) {
            //printf("====find the first fit available block\n");
            break;
        }
        temp = temp->next;
    }
    //found available block
    if (temp->size != 0) {
        //printf("====found: \n");
        //split() or directly remove; (allocate former part)
        if (temp->size > size + METADATA_SIZE) {
            //printf("====split: \n");
            //split();
            //generate new metadata (add to free list) for the left part
            metadata_t * new_meta = (metadata_t *) ((char *) temp + METADATA_SIZE + size);
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
            //printf("====not split (allocate directly (remove from list)): \n");
            //allocate directly (remove from list)
            temp->available = 0;
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;

            free_size = free_size - METADATA_SIZE - temp->size;
        }
        temp->next = NULL;
        temp->prev = NULL;
        //print_free_list();
        //print_from_back();
        //printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        //printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(temp + 1), temp->available, temp->size);
        return temp + 1;
        //not found available block
    } else {
        //printf("====not found: \n");
        // if there is no available block, then call sbrk() to create
        // free_list is empty, or blocks in free_list are all smaller than required
        metadata_t * new_meta = sbrk(size + METADATA_SIZE);
        if (new_meta == (void *) -1) {
            ////printf("sbrk failed\n");
            return NULL;
        }
        new_meta->available = 0;
        new_meta->size = size;
        new_meta->prev = NULL;
        new_meta->next = NULL;

        heap_size = heap_size + METADATA_SIZE + size;

        //print_free_list();
        ////print_from_back();
        //printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        //printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(new_meta + 1), new_meta->available, new_meta->size);
        return new_meta + 1;
    }
    */
}


void coalesce(metadata_t * new_free, metadata_t * temp) {
    // coalesce the adjacent free block (compare to the prev and the next)
    //case 1: coalesce with both prev and next (by removing next)
    if (temp->size != 0 && temp->prev->size != 0 &&
        (char *) temp->prev + METADATA_SIZE + temp->prev->size == (char *) new_free &&
        (char *) new_free + METADATA_SIZE + new_free->size == (char *) temp) {
        //printf("case 1: merge prev and next:\n");
        //update prev and remove next
        temp->prev->size += (METADATA_SIZE * 2 + new_free->size + temp->size);
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        temp->next = NULL;
        temp->prev = NULL;
    }
        //case 2: coalesce with only prev
    else if (temp->prev->size != 0 && (char *) temp->prev + METADATA_SIZE + temp->prev->size == (char *) new_free) {
        //printf("case 2: merge prev:\n");
        temp->prev->size += (METADATA_SIZE + new_free->size);
    }
        //case 3: coalesce with only next (by replacing the temp with new_free)
    else if (temp->size != 0 && (char *) new_free + METADATA_SIZE + new_free->size == (char *) temp) {
        //printf("case 3: merge next:\n");
        new_free->size += (METADATA_SIZE + temp->size);
        new_free->next = temp->next;
        temp->next->prev = new_free;
        temp->prev->next = new_free;
        new_free->prev = temp->prev;
    }
        //case 4: no need to coalesce
    else {
        //printf("case 4: no merge - add to list:\n");
        //add before the temp
        new_free->next = temp;
        temp->prev->next = new_free;
        new_free->prev = temp->prev;
        temp->prev = new_free;
    }
}

//my free
void my_free(void *ptr) {
    //printf("............in ff_free: ............\n");
    if (ptr == NULL) {
        return;
    }
    metadata_t *new_free = (metadata_t *) ptr - 1;
    //printf("need to free ptr (*new_free) at %lu\n", (unsigned long )new_free);
    new_free->available = 1;
    free_size += (METADATA_SIZE + new_free->size);
    new_free->prev = NULL;
    new_free->next = NULL;

    // find the location of new_free to add to free_list
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (new_free < temp) {
            //printf("find the loc to add new_free: temp at %lu\n", (unsigned long)temp);
            break;
        }
        temp = temp->next;
    }

    coalesce(new_free, temp);
    //print_free_list();
    //print_from_back();
    //printf("\n");
}


//Best Fit malloc
void *bf_malloc(size_t size) {
    //printf("~~~~~~~~~~~~in bf_malloc: ~~~~~~~~~~~~\n");
    //printf("before malloc - current program break: %lu\n", (unsigned long )sbrk(0));
    //printf("input size: %zu\n", size);
    if (size <= 0) {
        return NULL;
    }
    //metadata_t *new_meta;

    my_malloc(size, 1);
    /*
    if (head == NULL) {
        //printf("---it's the very first block in heap: init free_list \n");
        make_empty_list();
        //print_free_list();
        //print_from_back();
    }

    // find the best fit available block
    //new_meta = find_ff(size);
    //printf("====try to find the best fit available block\n");
    int j = 0;
    metadata_t * best_free = NULL;
    unsigned long long smallest_size = ULLONG_MAX;
    //printf("initial smallest_size = %llu\n", smallest_size);
    metadata_t *temp = head->next;
    //printf("go to while loop: \n");
    while (temp->size != 0) {
        //printf("~~~~~~~%dth loop: \n", j);
        //printf("temp addr = %lu size = %lu\n", (unsigned long)temp, temp->size);
        if (size <= temp->size) {
            //printf("a block:  large enough\n");
            //printf("current smallest_size = %llu\n", smallest_size);
            if (temp->size < smallest_size) {
                //printf("smaller size - need to update best_free:\n");
                smallest_size = temp->size;
                best_free = temp;
                //printf("after update: size = %llu addr = %lu\n", smallest_size, (unsigned long)best_free);
                if (size == smallest_size) {
                    break;
                }
            }
        }
        j++;
        temp = temp->next;
    }
     */

    /*
    printf("====try to find the best fit available block\n");
    int j = 0;
    metadata_t * best_free = head;
    if (head->next->size != 0) {

    }
    //unsigned long long smallest_size = ULLONG_MAX;
    //printf("initial smallest_size = %llu\n", smallest_size);
    metadata_t *temp = head->next;
    printf("go to while loop: \n");
    while (temp->size != 0) {
        printf("~~~~~~~%dth loop: \n", j);
        printf("temp addr = %lu size = %lu\n", (unsigned long)temp, temp->size);
        if (size <= temp->size) {
            printf("a block:  large enough\n");
            printf("current smallest_size = %llu\n", smallest_size);
            if (temp->size < smallest_size) {
                printf("smaller size - need to update best_free:\n");
                smallest_size = temp->size;
                best_free = temp;
                printf("after update: size = %llu addr = %lu\n", smallest_size, (unsigned long)best_free);
            }
        }
        j++;
        temp = temp->next;
    }
    */

    /*
    //printf("after while: \nbest_free addr = %lu size = %llu\n", (unsigned long )best_free, smallest_size);

    //found available block
    if (best_free != NULL) {
        //printf("====found: \n");
        //split() or directly remove; (allocate former part)
        if (best_free->size > size + METADATA_SIZE) {
            //printf("====split: \n");
            //split();
            //generate new metadata (add to free list) for the left part
            metadata_t * new_meta = (metadata_t *) ((char *) best_free + METADATA_SIZE + size);
            new_meta->available = 1;
            new_meta->size = best_free->size - size - METADATA_SIZE;
            new_meta->prev = NULL;
            new_meta->next = NULL;

            //replace the best_free with new_meta
            best_free->available = 0;
            best_free->size = size;
            best_free->prev->next = new_meta;
            new_meta->prev = best_free->prev;
            new_meta->next = best_free->next;
            best_free->next->prev = new_meta;

            free_size = free_size - METADATA_SIZE - size;
        } else {
            //printf("====not split (allocate directly (remove from list)): \n");
            //allocate directly (remove from list)
            best_free->available = 0;
            best_free->prev->next = best_free->next;
            best_free->next->prev = best_free->prev;

            free_size = free_size - METADATA_SIZE - best_free->size;
        }
        best_free->next = NULL;
        best_free->prev = NULL;
        //print_free_list();
        //print_from_back();
        //printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        //printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(best_free + 1), best_free->available, best_free->size);
        return best_free + 1;
        //not found available block
    } else {
        //printf("====not found: \n");
        // if there is no available block, then call sbrk() to create
        // free_list is empty, or blocks in free_list are all smaller than required
        metadata_t * new_meta = sbrk(size + METADATA_SIZE);
        if (new_meta == (void *) -1) {
            ////printf("sbrk failed\n");
            return NULL;
        }
        new_meta->available = 0;
        new_meta->size = size;
        new_meta->prev = NULL;
        new_meta->next = NULL;

        heap_size = heap_size + METADATA_SIZE + size;

        //print_free_list();
        ////print_from_back();
        //printf("after malloc - current program break: %lu\n", (unsigned long)sbrk(0));
        //printf("return malloc()'s addr: %lu avail = %d size = %zu\n\n\n", (unsigned long)(new_meta + 1), new_meta->available, new_meta->size);
        return new_meta + 1;
    }
    */

}

//First Fit free
void ff_free(void *ptr) {
    my_free(ptr);
}

//Best Fit free
void bf_free(void *ptr) {
    my_free(ptr);
}


unsigned long get_data_segment_size() {
    return heap_size;
}

unsigned long get_data_segment_free_space_size() {
    return free_size;
}
