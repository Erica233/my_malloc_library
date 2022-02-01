#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

typedef struct metadata metadata_t;
struct metadata {
    int available;
    size_t size;
    metadata_t *next;
    metadata_t *prev;
};

void make_empty_list(metadata_t ** head, metadata_t ** tail, int tls);
metadata_t * expand_heap(size_t size, int tls);
//metadata_t * find_ff(size_t size);
metadata_t * find_bf(size_t size, metadata_t ** head, metadata_t ** tail);
void split(metadata_t * usable, size_t size);
void *my_malloc(size_t size, int alloc_policy,  metadata_t ** head, metadata_t ** tail, int tls);
//void *ff_malloc(size_t size);
//void *bf_malloc(size_t size);

void coalesce(metadata_t * new_free, metadata_t * temp);
void my_free(void *ptr, metadata_t ** head, metadata_t ** tail);
//void ff_free(void *ptr);
//void bf_free(void *ptr);

//unsigned long get_data_segment_size();
//unsigned long get_data_segment_free_space_size();

//Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);
//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);

#endif
