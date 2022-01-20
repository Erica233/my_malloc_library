#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

typedef struct metadata metadata_t;
struct metadata {
    int available;
    size_t size;
    metadata_t *next;
    metadata_t *prev;
};


//First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);

//Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

#endif
