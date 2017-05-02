#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

extern mem_ptr Heap;


mem_ptr mm_realloc(mem_ptr m, int size) {
  /* Input: pointer to a mem_rec, new size of the block needed
     Output: 
       If the input pointer is null, call error_msg(2) and return

       Return a pointer to a mem_rec of the appropriate size (new_size).
       This block should be chosen as follows:
          if the new size is less than the current size of the block, 
            use the current block after moving the excess back to the appropriate segregated free
            list. 

          if the new block size is larger than the current size, 
            first see if there is enough space after the current block
              to expand (possibly using the neighboring free block). 
              Any excess partial free block that may result should be put to the appropriate segregated list.
           
          If this will not work either, you will need to free the current block by calling mm_free(),  
              and find a location for the requested larger block using mm_alloc(). 
 
 
       If there is nowhere to place a block of the given size, 
          call error_msg(1) and return a NULL pointer.

       Remember that the free blocks in each segregated list must be address-ordered at all times.      

  */
   if(!m){
      error_msg(1);
      return NULL;
   }

   if(m -> size > size){
      // Split m, free the new node, and return m
   }

   if(m -> size < size){
      // If there is a block neighboring m's address then merge, split off excess, free excess

      // Else then free m and then malloc a new block
   }

   int new_size = ALIGN(size);
   return NULL;
}



