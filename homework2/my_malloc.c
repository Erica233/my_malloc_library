#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <pthread.h>
#include "my_malloc.h"

#define METADATA_SIZE sizeof(metadata_t)
metadata_t *head_lock = NULL;
metadata_t *tail_lock = NULL;
size_t heap_size = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
__thread metadata_t *head_nolock = NULL;
__thread metadata_t *tail_nolock = NULL;

//Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size) {
    if (size <= 0) {
        return NULL;
    }
    pthread_mutex_lock(&lock);
    metadata_t * new = my_malloc(size, 1, &head_lock, &tail_lock, 0);
    pthread_mutex_unlock(&lock);
    return new + 1;
}

void ts_free_lock(void *ptr) {
    pthread_mutex_lock(&lock);
    my_free(ptr, &head_lock, &tail_lock);
    pthread_mutex_unlock(&lock);
}
//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size) {
    if (size <= 0) {
        return NULL;
    }
    metadata_t * new = my_malloc(size, 1, &head_nolock, &tail_nolock, 1);
    return new + 1;
}
void ts_free_nolock(void *ptr) {
    my_free(ptr, &head_nolock, &tail_nolock);
}

// initialize the free list (both head and tail are dummies)
void make_empty_list(metadata_t ** head, metadata_t ** tail, int tls) {
    *head = expand_heap(0, tls);
    *tail = expand_heap(0, tls);
    (*head)->next = tail;
    (*tail)->prev = head;

    heap_size += METADATA_SIZE * 2;
}

/*
// find the first fit block in the free list
metadata_t * find_ff(size_t size, metadata_t ** head) {
    metadata_t *temp = head->next;
    while (temp->size != 0) {
        if (size <= temp->size) {
            break;
        }
        temp = temp->next;
    }
    return temp;
}
 */

// find the best fit blcok in the free list
metadata_t * find_bf(size_t size, metadata_t ** head, metadata_t ** tail) {
    int j = 0;
    metadata_t * best_free = NULL;
    unsigned long long smallest_size = ULLONG_MAX;
    metadata_t *temp = (*head)->next;
    while (temp->size != 0) {
        if (size <= temp->size) {
            if (temp->size < smallest_size) {
                smallest_size = temp->size;
                best_free = temp;
                // if it has exact same size to required size, break the tie
                if (size == smallest_size) {
                    break;
                }
            }
        }
        j++;
        temp = temp->next;
    }
    if (best_free == NULL) {
        best_free = *tail;
    }
    return best_free;
}

/*
// First Fit malloc
void *ff_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }
    metadata_t * new = my_malloc(size, 0, &head, &tail);
    return new + 1;
}

// Best Fit malloc
void *bf_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }
    metadata_t * new = my_malloc(size, 1, &head, &tail);
    return new + 1;
}
 */

// malloc memory according to different memory allocation policies:
// alloc_policy == 0: first fit malloc
// alloc_policy == 1: best fit malloc
void *my_malloc(size_t size, int alloc_policy, metadata_t ** head, metadata_t ** tail, int tls) {
    if (*head == NULL) {
        make_empty_list(*head, *tail, tls);
    }

    // find the best fit available block
    metadata_t * usable;
    //first fit malloc
    if (alloc_policy == 0) {
        //usable = find_ff(size, head);
    }
    //best fit malloc
    if (alloc_policy == 1) {
        usable = find_bf(size, *head, *tail);
    }

    //found available block
    if (usable->size != 0) {
        if (usable->size > size + METADATA_SIZE) {
            split(usable, size);
        } else {
            //allocate directly (remove from list)
            usable->available = 0;
            usable->prev->next = usable->next;
            usable->next->prev = usable->prev;
        }
        usable->next = NULL;
        usable->prev = NULL;
        return usable;
    } else {
        // if not found available block, expand the heap
        metadata_t * new_meta = expand_heap(size, tls);
        heap_size = heap_size + METADATA_SIZE + size;
        return new_meta;
    }
}

// expand the heap area
metadata_t * expand_heap(size_t size, int tls) {
    metadata_t * new_meta;
    if (tls == 1) {
        pthread_mutex_lock(&lock);
        new_meta = sbrk(size + METADATA_SIZE);
        pthread_mutex_unlock(&lock);
    } else {
        new_meta = sbrk(size + METADATA_SIZE);
    }
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

// split the given block into two blocks, the first part is malloced,
// the left part becomes a new block add into the free list
void split(metadata_t * usable, size_t size) {
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

// coalesce the adjacent free block (compare to the prev and the next)
void coalesce(metadata_t * new_free, metadata_t * temp) {
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

/*
// First Fit free
void ff_free(void *ptr) {
    my_free(ptr);
}

// Best Fit free
void bf_free(void *ptr) {
    my_free(ptr);
}
 */

void my_free(void *ptr, metadata_t ** head, metadata_t ** tail) {
    if (ptr == NULL) {
        return;
    }
    metadata_t *new_free = (metadata_t *) ptr - 1;
    new_free->available = 1;
    new_free->prev = NULL;
    new_free->next = NULL;

    // find the location of new_free to add to free_list
    metadata_t *temp = (*head)->next;
    while (temp->size != 0) {
        if (new_free < temp) {
            break;
        }
        temp = temp->next;
    }

    // check if the list needs coalesce
    coalesce(new_free, temp);
}