#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "aux.h"

extern mem_ptr Heap;
extern mem_ptr* segLists;
extern int numLists;

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

   int new_size = ALIGN(size);
   
   if(!m){
      error_msg(1);
      return NULL;
   }

   if(m -> size > new_size){
      // Split m
      split(m,new_size);
      // Free the split off node
      mm_free(m->next);
      // Return m
      return m;
   }

   if(m -> size < new_size){
      // If there is a block neighboring m's address then merge, split off excess, free excess
      // Traverse the seg lists looking for a neighboring block
      int i;
      for(i = 0; i < numLists; i++){
          mem_ptr cur = segLists[i]->next;
          while(cur){
             int prevAddr = cur -> address + cur -> size;
             int nextAddr = m -> address + m -> size;

             // Merge m into previous block
             if(prevAddr == m -> address){
                // Merge 
                cur -> size += m -> size;
                mm_free(m);
                // If the merged block is to big then split
                if(cur->size > new_size){
                    // Split
                    split(cur,i);
                    // Free the split off node
                    mm_free(cur->next);
                    // Return cur
                    return cur;
                }
             } else if(m -> next){
                // Merge m -> next into m
                if(nextAddr = m -> next -> address){
                   // Merge
                   m -> size += m -> next -> size;
                   mm_free(m->next);
                   // If the merged block is to big then split
                   if(m -> size > new_size){
                      // Split
                      split(m,i);
                      // Free the split off node
                      mm_free(m->next);
                      // Return the realloc'd node
                      return m;
                   }
                } 
             }
             cur = cur -> next;
          }
      }

      // Else then free m and then malloc a new block
      mm_free(m);
      return mm_malloc(new_size);
   }

   return NULL;
}



