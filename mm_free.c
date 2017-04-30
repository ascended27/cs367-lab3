#include <stdlib.h>

#include "memory.h"

extern mem_ptr Heap;
extern int numLists;
extern mem_ptr* segLists;

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

	//temporary pointer
	mem_ptr* temp = m;

	//checking if there is a single list
	if(numLists == 1){
	
		int size = 0;

		temp->valid = 0;

		//if the previous node is free
		if(temp->previous->valid == 0){
			
			//total size of the block of memory
			size = temp->previous->address + temp->previous->size;

			//if the total block size is equal to the address of temp
			if(size == temp->address){

				temp->previous->next = temp->next;
				temp->previous->size = temp->size + temp->previous->size;
				temp->next->previous = temp->previous;
				temp = temp->previous;
				free(m);
				m = NULL;
			}
		}

		//if the next node is free
		if(temp->next->valid == 0){
		
			//total size of the memory block
			size = temp->address + temp->size;

			//if the total block size is equal to the address of the next node
			if(size == temp->next->address){
			
				temp->size = temp->size + temp->next->size;
				temp->next->next->previous = temp;
				temp->next = temp->next->next;
				free(m);
				m = NULL;
			}
		}
	
	}
	else{
	
	}

}




