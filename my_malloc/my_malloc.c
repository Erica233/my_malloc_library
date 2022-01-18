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

  if (head == NULL) {
    make_empty_list();
  }

  // find the first fit available block
  //new_meta = find_ff(size);
  metadata_t * temp = head->next;
  while (temp->size != 0) {
    if (size <= temp->size) {
        break;
    }
    temp = temp->next;
  }
  //found available block
  if (temp->size != 0) {
      //split() or directly remove;
      if (temp->size > size + METADATA_SIZE) {
          //split();
          metadata_t *new_meta = (metadata_t *) ((char *) temp + size + METADATA_SIZE);
          new_meta->available = 1;
          new_meta->size = temp->size - size - METADATA_SIZE;
          new_meta->prev = NULL;
          new_meta->next = NULL;

          //add left_part, remove temp
          temp->prev->next = new_meta;
          new_meta->prev = temp->prev;
          new_meta->next = temp->next;
          temp->next->prev = new_meta;

          free_size -= (METADATA_SIZE + size);
      } else {
          //allocate directly (remove from list)
          temp->prev->next = temp->next;
          temp->next->prev = temp->prev;

          free_size -= (METADATA_SIZE + temp->size);
      }
  //not found available block
  } else {
      // if there is no available block, then call sbrk() to create
      // free_list is empty, or blocks in free_list are all smaller than required
      temp = sbrk(size + METADATA_SIZE);
      /*
      new_meta->available = 0;
      new_meta->size = size;
      new_meta->prev = NULL;
      new_meta->next = NULL;
      */
      heap_size += size + METADATA_SIZE;
  }

  return temp + 1;
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
