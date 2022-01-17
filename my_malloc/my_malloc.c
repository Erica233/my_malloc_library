#include <unistd.h>
#include <stdio.h>
#include "my_malloc.h"

typedef struct block_info block_info_t;
struct block_info {
  int available;
  size_t size;
  block_info_t *next;
};
#define BLOCK_SIZE sizeof(block_info_t)
block_info_t *head = NULL;
block_info_t *tail = NULL;

void *ff_malloc(size_t size);
void ff_free(void *ptr);


int main() {
    printf("hello");
    return 0;
}

//First Fit malloc
void *ff_malloc(size_t size) {
  // if there is no available block, then call sbrk() to create
  // first_block is null, or free_list is empty, or blocks in free_list are all smaller than required
  if (head == NULL) {
    sbrk(size + );
  }
  
}

//First Fit free
void ff_free(void *ptr) {
  
}

/*
//Best Fit malloc
void *bf_malloc(size_t size) {

}

//Best Fit free
void bf_free(void *ptr) {
}


unsigned long get_data_segment_size() {

}

unsigned long get_data_segment_free_space_size() {

}
*/
