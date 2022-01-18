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
  
  heap_size += METADATA_SIZE * 2;
  free_size += METADATA_SIZE * 2;
}

/*
metadata_t *find_ff() {
}
*/

//First Fit malloc
void *ff_malloc(size_t size) {
  printf("------in ff_malloc--------\n");

  //metadata_t *new_meta;
  if (head == NULL) {
    make_empty_list();
  }
  
  // find the first fit available block
  //new_meta = find_ff(size);
  metadata_t * temp = head->next;
  while (temp->size != 0) {
    if (size <= temp->size) {
      //split();
      if (temp->size > size + METADATA_SIZE) {
	//split();
	metadata_t * left_part = (metadata_t *)((char *)temp + size + METADATA_SIZE); 
	left_part->available = 1;
	left_part->size = temp->size - size - METADATA_SIZE;
	left_part->prev = NULL;
	left_part->next = NULL;

	//add left_part, 
      } else {
	//allocate directly (remove from list)
      }
    }
    temp = temp->next;
  } 
  
  // if there is no available block, then call sbrk() to create
  // free_list is empty, or blocks in free_list are all smaller than required
  new_meta = sbrk(size + METADATA_SIZE);
  /*  new_meta->available = 0;
  new_meta->size = size;
  new_meta->prev = NULL;
  new_meta->next = NULL;
  */
  heap_size += size + METADATA_SIZE;

  return new_meta + 1;
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
