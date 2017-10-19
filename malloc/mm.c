#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "./mm.h"
#include "./memlib.h"
#include "./mminline.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

block_t *prologue;
block_t *epilogue;

static inline size_t align(size_t size) {
    return (((size) + (WORD_SIZE - 1)) & ~(WORD_SIZE - 1));
}

int mm_check_heap(void);
void *coalesce(void *ptr);
void mm_free(void *ptr);
int mm_check_heap(void);
void *split(void *ptr, size_t nsize);
int coalesce_helper(void *ptr);




/*
 *                             _       _ _
 *     _ __ ___  _ __ ___     (_)_ __ (_) |_
 *    | '_ ` _ \| '_ ` _ \    | | '_ \| | __|
 *    | | | | | | | | | | |   | | | | | | |_
 *    |_| |_| |_|_| |_| |_|___|_|_| |_|_|\__|
 *                       |_____|
 *
 * initializes the dynamic storage allocator (allocate initial heap space)
 * arguments: none
 * returns: 0, if successful
 *         -1, if an error occurs
 */
int mm_init(void) {
  flist_first = NULL;  // Set the first to NULL

  // Create the initial empty heap
  prologue = mem_sbrk(MINBLOCKSIZE);
  if (prologue == (void *) -1) {
        return -1;  // Terminated because failed heap extend
  }
  block_set_size_and_allocated(prologue, MINBLOCKSIZE, 1);

  // Create free space heap and block
  block_t *free_spot = mem_sbrk(MINBLOCKSIZE);
  if (free_spot == (void *) -1) {
        return -1;  // Terminated because failed heap extend
  }
  block_set_size_and_allocated(free_spot, MINBLOCKSIZE, 0);
  insert_free_block(free_spot);

  // Create the end of the empty heap
  epilogue = mem_sbrk(MINBLOCKSIZE);
  if (epilogue == (void *) -1) {
        return -1;  // Terminate because failed heap extend
  }

  block_set_size_and_allocated(epilogue, MINBLOCKSIZE, 1);

  return 0;  // Everything worked out
}


/*
 * Checks to see if the block is in the free list
 *
 * Parameters:
 *  - ptr: the pointer to the payload of malloc
 *
 * Returns:
 *  - Whether the block was free or not
 */
int coalesce_helper(void *ptr) {
  block_t *block = flist_first;

  if (block == ptr) {
    return 1;  // Block is in free list
  }

  block = block_next_free(block);

  while (flist_first != block) {
    if (block == ptr) {
      return 1;  // Block is in free list
    }

    // Check next free block
    block = block_next_free(block);
  }

  return 0;  // Block not found in free list
}


/*
 * Coalsesce free blocks to form one large block
 *
 * Parameters:
 *  - ptr: the pointer to the payload of malloc
 *
 * Returns:
 *  - Return the pointer after coallescing
 */
void *coalesce(void *ptr) {
  // Check coalescing only free blocks
  if (!coalesce_helper(ptr)) {
    return NULL;
  }
  block_t *prev = block_prev(ptr);
  block_t *next = block_next(ptr);

  size_t csize = block_size(ptr);


  // Case where the previous block is free and next is allocated
  if (!block_allocated(prev) && block_allocated(next)) {
    pull_free_block(ptr);

    block_set_size_and_allocated(prev, csize + block_size(prev), 0);

    ptr = prev; 
    return ptr;
  } 

  // Case where the previous block is allocated and the next is free
  if (block_allocated(prev) &&  !block_allocated(next)) {
    pull_free_block(next);

    block_set_size_and_allocated(ptr, csize+block_size(next), 0);

    return ptr;
  }
    
  // Case where the previous block is free and the next is free
  if (!block_allocated(prev) && !block_allocated(next)) {
    pull_free_block(ptr);
    pull_free_block(next);

    block_set_size_and_allocated(prev, 
      csize + block_size(prev) + block_size(next), 0);

    ptr = prev;
    return ptr;
  }

  return ptr;  // Case where previous and next block are allocated
}


/*
 * Helps malloc with extending the heap
 *
 * Parameters:
 *  - nsize: the new size to malloc
 *
 * Returns:
 *  - the payload pointer of the new space
 */
void *extend_heap(size_t nsize) {
  // Add more space to the
  if ((void *) -1 == mem_sbrk(nsize)) {
    return NULL;
  }

  block_t *ptr;
    
  // Set overwrite the old epilogue
  block_set_size_and_allocated(epilogue, nsize, 1);

  
  // Set up for the new epilogue
  block_t *new_epilogue = block_next(epilogue);
  block_set_size_and_allocated(new_epilogue, MINBLOCKSIZE, 1);
    
  ptr = epilogue;
  // ptr = coalesce(epilogue->payload);
  ptr = split(ptr, nsize);
  epilogue = new_epilogue;

  return ptr->payload;
}


/*
 * Finds the difference of the smallest fraction of /2
 *
 * Parameters:
 *  - nsize: size we are looking to malloc
 *  - csize: current malloc size
 *  - diff: the smallest difference so far
 *
 * Returns:
 *  - the smallest difference
 */
size_t smallest(size_t nsize, size_t csize, size_t diff) {
  if (diff/2 > nsize) {
    return smallest(nsize, csize, diff/2);
  }

  return diff;
}


/*     _ __ ___  _ __ ___      _ __ ___   __ _| | | ___   ___
 *    | '_ ` _ \| '_ ` _ \    | '_ ` _ \ / _` | | |/ _ \ / __|
 *    | | | | | | | | | | |   | | | | | | (_| | | | (_) | (__
 *    |_| |_| |_|_| |_| |_|___|_| |_| |_|\__,_|_|_|\___/ \___|
 *                       |_____|
 *
 * allocates a block of memory and returns a pointer to that block's payload
 * arguments: size: the desired payload size for the block
 * returns: a pointer to the newly-allocated block's payload (whose size
 *          is a multiple of ALIGNMENT), or NULL if an error occurred
 */
void *mm_malloc(size_t size) {
  // Check if it a reasonable size
  if (size <= 0) {
    return NULL;  // Return if bad size
  }

  // Get the new size with aligning and padding
  size_t nsize = MAX(align(size) + TAGS_SIZE, MINBLOCKSIZE);

  // Case of empty free list
  if (flist_first == NULL) {
    return extend_heap(nsize);
  }

  size_t min = UINT_MAX;
  block_t *best = NULL;  // Best malloc spot found
  block_t *current = flist_first;  // Keeps track of location in free list
  size_t csize;  // Size of current block
  size_t diff;  
  int d;


  do {
    csize = block_size(current);
    d = csize - nsize;
    if (d >= 0) {
      diff = (size_t) d;

      // find the smallest diff with split
      diff = smallest(csize, nsize, diff);

      // if the differnce is better than current best
      if (diff < min) {
        min = diff;
        best = current;
      }
    }

    // Get next free block
    current = block_next_free(current);
  } while (current != flist_first);

  // Couldn't find a good free list block
  if (best == NULL) {
    return extend_heap(nsize);
  }

  // Found a good free list block
  pull_free_block(best);
  block_set_size_and_allocated(best, block_size(best), 1);
  best = split(best, nsize);

  return best->payload;
}

/*                              __
 *     _ __ ___  _ __ ___      / _|_ __ ___  ___
 *    | '_ ` _ \| '_ ` _ \    | |_| '__/ _ \/ _ \
 *    | | | | | | | | | | |   |  _| | |  __/  __/
 *    |_| |_| |_|_| |_| |_|___|_| |_|  \___|\___|
 *                       |_____|
 *
 * frees a block of memory, enabling it to be reused later
 * arguments: ptr: pointer to the allocated block's payload to free
 * returns: nothing
 */
void mm_free(void *ptr) {
  // Check for NULL pointer
  if (!ptr) {return;}

  // Get the block to the payload
  block_t *block = payload_to_block(ptr);

  // Set the allocation to free of block
  block_set_size_and_allocated(block, block_size(block), 0);
  insert_free_block(block);  // Set the block free

  coalesce(block);  // Coalesce the free blocks
}


/*
 * Splits the malloc if it is two times greater then needed size
 *
 * Parameters:
 *  - ptr: a pointer to the malloc
 *  - nsize: needed size for malloc
 *
 * Returns:
 *  - ponter to new malloc after split
 */
void *split(void *ptr, size_t nsize) {
  size_t diff = block_size(ptr) - nsize;

  // Check to see if it is worth mallocing
  if (diff >= 2*nsize && diff >= MINBLOCKSIZE) {
    block_set_size_and_allocated(ptr, nsize, 1);
    block_set_size_and_allocated(block_next(ptr), diff, 0);
    insert_free_block(block_next(ptr));
    /*
     * The idea of coalescing here is appealing but we are already splitting
     * a large block off so maybe we shouldn't.
     * coalesce(block_next(ptr));
     */
  }

  return ptr;
}



/*                                               _ _
 *     _ __ ___  _ __ ___      _ __ ___     __ _| | | ___   ___
 *    | '_ ` _ \| '_ ` _ \    | '__/ _ \   / _` | | |/ _ \ / __|
 *    | | | | | | | | | | |   | | |  __/  | (_| | | | (_) | (__s
 *    |_| |_| |_|_| |_| |_|___|_|  \___|___\__,_|_|_|\___/ \___|
 *                       |_____|      |_____|
 *
 * reallocates a memory block to update it with a new given size
 * arguments: ptr: a pointer to the memory block
 *            size: the desired new block size
 * returns: a pointer to the new memory block
 */
void *mm_realloc(void *ptr, size_t size) {
  // If size is 0 or less, then just free the block
  if (size <= 0) {
    mm_free(ptr);
    return NULL;
  }

  // If the pointer doesn't point to a block, then malloc space for the pointer
  if (ptr == NULL) {
    return mm_malloc(size);
  }

  // Block of payload
  block_t *block = payload_to_block(ptr);

  // Get the new size with aligning and padding
  size_t nsize = MAX(align(size) + TAGS_SIZE, MINBLOCKSIZE);
  size_t csize = block_size(block);  // Current size

  // Case of perfect match
  if (nsize == csize) {
    return ptr;  // There is match with our current
  }
  
  // Case of current size being greater than desired size
  if (nsize < csize) {
    // split(ptr, nsize);
    return ptr;
  }

  // Case that current block has space next to it that it can move into
  if (csize + block_next_size(ptr) >= nsize) {
    block_t *next = block_next(ptr);
    pull_free_block(next);
    block_set_size_and_allocated(ptr, csize+block_size(next), 1);
    split(ptr, nsize);
    return ptr;
  }
  
  // Case of current size being less than desired size
  void *nptr = mm_malloc(size);

  if (!nptr) {
    return NULL;  // Failed to malloc
  }

  memmove(nptr, ptr, csize);  // Copy current block into new block
  mm_free(ptr);
  
  return nptr;
}


/*
 * checks the state of the heap for internal consistency
 * arguments: none
 * returns: 0, if successful
 *          nonzero, if the heap is not consistent
 */
int mm_check_heap(void) {
  block_t *current = block_next(prologue);

    
  // prints out the appropriate messages across the heap:
  while (current != epilogue) {
    if (block_allocated(current)) {
      printf("Allocated block at location %p, size %d, Next: %p of size: %d\n",
        (void *) current, (int) block_size(current),
         (void *) block_next(current), (int) block_size(block_next(current)));
    } else {
        printf("Free block at location %p, size %d, Next: %p of size: %d\n",
          (void *) current, (int) block_size(current), 
          (void *) block_next(current), (int) block_size(block_next(current)));
      }

    current = block_next(current);
  }
    
  // error checks if all entries in free list is 
  // actually free (allocated flag set to 0)
  block_t *ncurr = flist_first;
  if (block_allocated(ncurr)) {
    printf("bad free list\n");
    return -1;
  }

  ncurr = block_next_free(ncurr);
  while (ncurr != flist_first) {
    if (block_allocated(ncurr)) {
      printf("bad free list\n");
      return -1;
    }

    ncurr = block_next_free(ncurr);
  }

  return 0;
}
