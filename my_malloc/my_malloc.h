#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

typedef struct block block_t;
struct block {
  int avail;
  size_t size;
  block_t *next;
};

//First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);

//Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);

#endif
