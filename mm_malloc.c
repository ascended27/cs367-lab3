#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

extern mem_ptr Heap;
extern int numLists;
extern mem_ptr* segLists;


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

  // Allocate a new mem_ptr for our node. Probably could do this after we find a space to insert. 
  // Because we don't need to always have a new node. If we find a node of the same size we can just
  // make it's valid flag true.
  mem_ptr toAdd = malloc(sizeof(mem_ptr));

  toAdd -> size = new_size;
  toAdd -> valid = 1;
  toAdd -> minSize = -1;
  toAdd -> maxSize = -1;
  toAdd -> next = toAdd -> previous = NULL;
  toAdd -> address = -1;

  // The heap pointer points to the first segregated 
  // list if there is only one segregated list.
  if(numLists == 1){
	  mem_ptr p = Heap;
	  while( (p -> next) != NULL){
	     // If p is not valid (aka free) and has enough size then use that node
	     if( (p -> size) >= new_size && (p -> valid) == 0){
	        // p's size is exactly the size we need so we can set it to valid and be done.
		if(p -> size == new_size){
                   p -> valid = 1;
		   return p;
		} else{
		   // Otherwise we need to cut the node to the right size.
		   p -> size -= new_size;
		   toAdd -> previous = p -> previous;
		   toAdd -> next = p;
		   toAdd -> address = p -> address;
		   p -> address += new_size;
		}
	     } else {
	        // Otherwise p does not fit our size requirement or is already allocated
		// so move to the next mem_ptr.
		p = p -> next;
	     }

	  }

	  // If we got to the end of the heap then our we insert a new node at the end if we have free space.
	  if(p -> valid == 0){
	     p -> size -= new_size;
	     toAdd -> address = p -> address;
	     p -> address += new_size;
	     toAdd -> next = p;
	     toAdd -> previous = p -> previous;
	     return toAdd;
	  } else{
	  	return NULL;
	  }
  }

  // Otherwise we need to search through segmented lists
  else if(numLists > 1){
     //TODO: Handle This case
  }

  return NULL;
}



