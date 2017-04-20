#include <stdlib.h>

#include "memory.h"

extern mem_ptr Heap;


void mm_free(mem_ptr m) {
  /* Input: pointer to a mem_rec 
     Output:  None

        You must coalesce this block with adjacent blocks if
        appropriate. The new free block (if possible, coalesced with
        the neighbors) must be added to the appropriate segregated
        list. The appropriate segregated list is defined as the
        smallest nominal size list that is still large enough to hold
        the new free block.

	Before returning from mm_free, make sure that all free blocks, in all
	segregated lists, are address-ordered (lower addresses first).

	If the input pointer is null, call error_msg(2) and return.  
  */
}




