#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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

void *ff_malloc(size_t size);
void ff_free(void *ptr);

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
}

//First Fit malloc
void *ff_malloc(size_t size) {
  printf("------in ff_malloc--------\n");
  metadata_t *new_meta;
  if (head == NULL) {
    make_empty_list();
  }
  // find the first fit available block
  // if there is no available block, then call sbrk() to create
  // first_block is null, or free_list is empty, or blocks in free_list are all smaller than required
  //if () {
  new_meta = sbrk(size + METADATA_SIZE);
    //}
  return new_meta + METADATA_SIZE;
}

//First Fit free
void ff_free(void *ptr) {
  head = NULL;
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
