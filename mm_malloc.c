#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

void split(mem_ptr node, int new_size);
mem_ptr getFirstFit(mem_ptr list, int new_size);
mem_ptr getPosition(mem_ptr node);

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
		error_msg(2);
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
		 if(heapPosition){
			//printf("inserting -> %5d: size = %-5d \n\n", heapPosition->address, heapPosition->size);
			 p -> next = heapPosition;
			 p -> previous = heapPosition -> previous;
			 if(heapPosition -> previous)
				heapPosition -> previous -> next = p;
			 heapPosition -> previous = p;
		 } else if(heapPosition == p){
			//printf("insert at end\n\n");
			heapPosition = Heap;
			while(heapPosition->next){
				heapPosition = heapPosition -> next;
			}
			heapPosition -> next = p;
			p -> previous = heapPosition;
		 }
	}else{
		//printf("heap is empty\n\n");
		Heap = p;
	}
	 
	return p;
  }
 
  error_msg(1);
  return NULL;
}

void split(mem_ptr node, int new_size){
	// Make a new node
	mem_ptr splitNode = malloc(sizeof(mem_rec));
	
	// Set split node's size
	splitNode -> size = node -> size - new_size;
	
	// Set the current node's size
	node -> size = new_size;
	
	// Set split node's address
	splitNode -> address = node-> address + new_size;
	
	// Set the pointers
	splitNode -> previous = node;
	splitNode -> next = node -> next;
	node -> next = splitNode;
	
	// Move splitNode if necessary
	mem_ptr p = splitNode;
	while(p->previous){
		probes++;
		p = p->previous;
	}
	
	// If splitNode's size is smaller than its seg list then 
	// move it down to the correct list.
	//printf("p->minSize: %d\nsplitNode -> size: %d\n",p->minSize,splitNode -> size);
	if(p -> minSize > splitNode -> size){
		//printf("Need Moving\n");
		int i;
		for(i = 0; i < numLists; i++){
			p = segLists[i];
			
			// If splitNode's size fits in p's size range then 
			// insert into this seg list
			//printf("p->minSize: %d maxSize: %d\nsplitNode -> size: %d\n",p->minSize,p->minSize,splitNode -> size);
			if(p -> minSize < splitNode -> size && p -> maxSize > splitNode -> size){
				p = p->next;
				// Seg List is not empty
				if(p){
					// Remove from the previous seg list
					splitNode -> previous -> next  = splitNode -> next;
					if(splitNode->next)
						splitNode -> next -> previous = splitNode -> previous;
					// Loop over the seg list and find a position to insert;
					while(p->next){
						if(p -> address > splitNode -> address){
							break;
						} else{
							p = p->next;
							probes++;
						}
					}
					// Insert into the new seg list
					splitNode -> next = p;
					splitNode -> previous = p->previous;
					if(p->previous)
						p -> previous -> next = splitNode;
					p->previous = splitNode;
				} else{ // Seg list is empty
					// Remove from the previous seg list
					splitNode -> previous -> next  = splitNode -> next;
					if(splitNode->next)
						splitNode -> next -> previous = splitNode -> previous;
					segLists[i] -> next = splitNode;
					splitNode -> previous = segLists[i];
				}
			}	
		}
	}
	
	//printf("splitNode    -> %5d: size = %-5d \n", splitNode->address, splitNode->size);
	//printf("node         -> %5d: size = %-5d \n", node->address, node->size);
}

mem_ptr getFirstFit(mem_ptr list, int new_size){
	//printf("getFirstFit -> %5d: size = %-5d \n", list->address, list->size);
	mem_ptr p = list -> next;
	//printf("p           -> %5d: size = %-5d \n\n", p->address, p->size);
	if(!p)
		return NULL;
	probes++;
	mem_ptr toReturn = NULL;
	while(p){
		if( p -> size >= new_size){
			toReturn = p;
			break;
		}
		else{
			p = p->next;
			probes++;
		}
	}
	return toReturn;
}

mem_ptr getPosition(mem_ptr node){
	//printf("getPosition  -> %5d: size = %-5d \n", node->address, node->size);
	mem_ptr p = Heap;
	if(!p){
		printf("Empty Heap\n");
		return node;
	}
	while(p -> next){
		if(p->address > node->address){
			//printf("found -> %5d: size = %-5d \n", p->address, p->size);
			return p;
		}
		else
			p = p->next;
	}
	//printf("Insert at end\n\n");
	return NULL;
}

