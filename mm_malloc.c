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
		if(p->size < new_size || p -> valid == 1 )
			p = p->next;
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
	} else{
		return NULL;
	}
  }

  // Otherwise we need to search through segmented lists
  else if(numLists > 1){
     mem_ptr p = NULL;
     mem_ptr curNode = NULL;
     // Loop over segLists and check for the size list we need
     int i;
     
     // Loop over the seg lists 
     for(i = 0; i < numLists; i++){
        p = segLists[i];
		// Seg list is of the right range of sizes
		if( ((p -> minSize <= new_size) && (p -> maxSize >= new_size)) || ((p -> minSize <= new_size) && (p -> maxSize == 0)) ){
			curNode = p -> next;
			while(curNode -> next){
		
				// A node has been found that can be used.
				if(curNode -> size <= new_size){
					break;
				}	   
				else{ // A node has not been found
					curNode = curNode -> next;
				}	
			}
			if(curNode -> size == new_size){
						
				   // Traverse the heap and insert in the right address position
				   mem_ptr heapPtr = Heap;
				   while(heapPtr->next){
				   
						// Insert to the left of headPtr
						if(heapPtr -> address > curNode -> address){
							// Remove from the seg list
							curNode -> previous -> next = curNode -> next;
							curNode -> next -> previous = curNode -> previous;

							// Insert into the heap
							curNode -> previous = heapPtr -> previous;
							heapPtr -> previous -> next = curNode;
							curNode -> next = heapPtr;
							return curNode;

						} else { // Move to the next node
						  heapPtr = heapPtr -> next;
					  }
				   
				   }
				   // We need to insert it at the end of the list then
				   heapPtr -> next = curNode;
				   curNode -> previous = heapPtr;
				   curNode -> next = NULL;
				   return curNode;
			} else{ // We need to split this node
					mem_ptr toAdd = malloc(sizeof(mem_rec));
						 
				 // Initialize new node
						 toAdd -> size = new_size;
						 toAdd -> valid = 1;
						 toAdd -> minSize = -1;
						 toAdd -> maxSize = -1;
				 toAdd -> address = curNode -> address;

				 // Update curNode
				 curNode -> address += new_size;
				 curNode -> size -= new_size;

				// Insert toAdd into the heap
				// Traverse the heap and insert in the right address position
				mem_ptr heapPtr = Heap;
				while(heapPtr->next != NULL){

							// Insert to the left of headPtr
							if(heapPtr -> address > toAdd -> address){
							   // Remove from the seg list
							   toAdd -> previous -> next = toAdd -> next;
							   toAdd -> next -> previous = toAdd -> previous;
							   
							   // Insert into the heap
							   toAdd -> previous = heapPtr -> previous;
							   heapPtr -> previous -> next = toAdd;
							   toAdd -> next = heapPtr;

							} else { // Move to the next node
							   heapPtr = heapPtr -> next;
							}
				
							// We need to insert it at the end of the list then
							heapPtr -> next = toAdd;
							toAdd -> previous = heapPtr;
							toAdd -> next = NULL;
						}

				// Move curNode to the correct position in the segLists array
				int j;
				for(j = 0; j < numLists; j++){
					mem_ptr tmp = segLists[j];
					// If curNode's size is between the current segLists range then insert it here
					if( ((tmp -> minSize <= new_size) && (tmp-> maxSize >= new_size)) || ((tmp -> minSize <= new_size) && (tmp -> maxSize == 0)) ){
					   mem_ptr listTmp = tmp->next;
					   while(listTmp -> next != NULL){
						   // If listTemp address is > than curNode then insert to the left of listTemp
						   if(listTmp -> address > curNode -> address){
							  // Remove from it's current seglist
							  curNode -> previous -> next = curNode -> next;
							  if(curNode -> next != NULL){
								 curNode -> next -> previous = curNode -> previous;
							  }

							  // Insert into new segList
							  curNode -> next = listTmp;
							  curNode -> previous = listTmp -> previous;
							  listTmp -> previous -> next = curNode;
							  listTmp -> previous = curNode;

						   } else {
							  listTmp = listTmp -> next;
						   }
					   }
					}
				}
				 return toAdd;
			}
		 }	
	 }
  }
        
     
  
  

  return NULL;
}



