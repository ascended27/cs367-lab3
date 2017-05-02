#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "aux.h"

extern mem_ptr Heap;
extern int numLists;
extern int probes;
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
  //printf("malloc(%d)\n",size);  
  int new_size = ALIGN(size);

  // Allocate a new mem_ptr for our node. Probably could do this after we find a space to insert. 
  // Because we don't need to always have a new node. If we find a node of the same size we can just
  // make it's valid flag true.
  mem_ptr toAdd = malloc(sizeof(mem_rec));

  toAdd -> size = new_size;
  toAdd -> valid = 1;
  toAdd -> minSize = -1;
  toAdd -> maxSize = -1;
  toAdd -> next = toAdd -> previous = NULL;
  toAdd -> address = 0;

  // The heap pointer points to the first segregated 
  // list if there is only one segregated list.
  if(numLists == 1){
	mem_ptr p = Heap;
	
	while(p->next){
		if(p->size < new_size || p -> valid == 1 ){
			p = p->next;
			probes++;
		}
		else
			break;
	}
	
	// The Heap is empty
	if(p == Heap){
	   //Split off a chunk that we need
	   Heap->size -= new_size;
	   Heap->address += new_size;
	   toAdd -> next = Heap;
	   Heap -> previous = toAdd;
	   Heap = toAdd;
	   return Heap;
	}  
	
	// p's size is exactly the size we need so we can set it to valid and be done.
	if(p -> size == new_size){
		p -> valid = 1;
		return p;
	} else if(p->size >= new_size){
	   // Otherwise we need to cut the node to the right size.
	   p -> size -= new_size;
	   toAdd -> previous = p -> previous;
	   toAdd -> next = p;
	   p -> previous -> next = toAdd;
	   p -> previous = toAdd;
	   toAdd -> address = p -> address;
	   p -> address += new_size;
	   return toAdd;
	} 
	return NULL;
  }

  // Otherwise we need to search through segmented lists
  else if(numLists > 1){
	 int i;
	 mem_ptr p; 
	 
	 // Traverse the segregated list array to find the segregated list with the proper range
	 for(i = 0; i < numLists; i++){
		p = getFirstFit(segLists[i],new_size);
		if(p){
			break;
		}
	}
	 
	if(!p){
		// Out of memory
		error_msg(1);
		return NULL;
	} 
	 
	 if(p->size > new_size){
		 //printf("Malloc(%d)\n", new_size);
		 //printf("splitting    -> %5d: size = %-5d \n", p->address, p->size);
		 split(p,new_size);
	}
	 
	// Remove from the segregated list
	p -> previous -> next = p -> next;
	if(p->next)
		 p -> next -> previous = p -> previous;
	 
	// Add to the Heap
	if(Heap){
		 mem_ptr heapPosition = getPosition(p);
		 if(heapPosition == p){
		    p -> previous = heapPosition -> previous;
                    p -> next = heapPosition;
                    if(heapPosition -> previous)
                       heapPosition -> previous -> next = p;
                 }
	}else{
		//printf("heap is empty\n\n");
		p -> next = NULL;
		p -> previous = NULL;
		Heap = p;
	}
	
	return p;
  }
 
  error_msg(1);
  return NULL;
}
