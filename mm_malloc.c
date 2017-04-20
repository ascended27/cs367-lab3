#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

extern mem_ptr Heap;


mem_ptr
mm_malloc(int size) {
  /* Input: size of the block needed 
     Output: Return a pointer to a mem_rec of the appropriate size (new_size).  
     
     The function should identify the smallest nominal size segregated list that is
     potentially large enough to satisfy the new request. Within that
     segregated list, the block will be identified using the first-fit
     heuristic.  

     If that segregated list does not have any free block
     that can accommodate the request, the segregated list of the next
     largest size should be checked, and so on.  

     If none of the
     segregated lists has place for the block of the given size, call
     error_msg(1) and return a NULL pointer
  */
  int new_size = ALIGN(size);
  return NULL;
}



