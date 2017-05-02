#include <stdlib.h>
#include "memory.h"


int placeInSegList(mem_ptr m);
mem_ptr coalesce(mem_ptr m, int location);

extern mem_ptr Heap;
extern int numLists;
extern mem_ptr* segLists;



/* 	Input: pointer to a mem_rec 
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
void mm_free(mem_ptr m) {


	//temporary pointer
	mem_ptr temp = m;
	mem_ptr erase = NULL;

	//checking if there is a single list
	//if(numLists == 1){
	//	coalesce(m);
	//}
	//else{
		
		int size = 0;
		temp->valid = 0;
		
		if((temp->previous != NULL)&&(temp->next != NULL)){
			temp->previous->next = temp->next;
			temp->next->previous = temp->previous;
		}
		else if((temp->previous != NULL)&&(temp->next == NULL)){
				if(temp->previous)
					temp->previous->next = NULL;
		}
		else if((temp->previous == NULL)&&(temp->next != NULL)){
				if(temp->previous)
					temp->next->previous = NULL;
				Heap = NULL;
		}
		else if((temp->previous == NULL)&&(temp->next == NULL)){
				Heap = NULL;
		}	
		
		int location = placeInSegList(temp);
		temp = coalesce(temp,location);
		
		if(segLists[location]->maxSize < temp->size){
			location = placeInSegList(temp);
		}
	
	//}

}

int placeInSegList(mem_ptr temp){
	
	int x = 0;
	int i = 0;
	int found = -1;
	int size = temp->size;
	
	for(i=0; i < numLists; i++){
		int minSize = segLists[i]->minSize;
		int maxSize = segLists[i]->maxSize;
		
		if(( (minSize < size) && (size <maxSize) ) ||  ( (minSize < size) && (maxSize == 0) )){
			
			found = i;
			
			mem_ptr list = segLists[i]->next;
			if(list != temp){
				if(list == NULL){
					segLists[i]->next = temp;
					temp->previous = segLists[i];
				}
				else{
					while(list->next){
						
						if(temp->address < list->address){
							break;
						}else{
							list = list->next;
						}
					}
					
					if(temp->address < list->address){
						temp->previous = list->previous;
						temp->next = list;
						if(list->previous)
							list->previous->next = temp;
					}
					else if(list->next == NULL){
						temp->next = NULL;
						list->next = temp;
						temp->previous = list;
					}
				} 
			}
		}
		
	}
	
	return found;
}

mem_ptr coalesce(mem_ptr m, int location){
	
//temporary pointer
	mem_ptr temp = m;
	mem_ptr erase = NULL;
	
	int size = 0;

	temp->valid = 0;


	if(temp->previous != NULL){
		// If the previous node is the dummy node then don't merge into that
		if(temp->previous != segLists[location]){
			//if the previous node is free
			if(temp->previous->valid == 0){

				//total size of the block of memory
				size = temp->previous->address + temp->previous->size;

				//if the total block size is equal to the address of temp
				if(size == temp->address){

					erase = m;
					temp->previous->next = temp->next;
					temp->previous->size = temp->size + temp->previous->size;
						
					if(temp->next){
						temp->next->previous = temp->previous;
						temp = temp->previous;
					}

						
					if(erase != Heap){
						free(erase);
						erase = NULL;
					}
						
				}
			}
		} 
			
	}
		
	if(temp->next != NULL)
		//if the next node is free
		if(temp->next->valid == 0){
		//total size of the memory block
			size = temp->address + temp->size;

			//if the total block size is equal to the address of the next node				
			if(size == temp->next->address){
				erase = temp->next;
				temp->size = temp->size + temp->next->size;
					
				if(temp->next->next){
					temp->next->next->previous = temp;
					temp->next = temp->next->next;
				}
				else{
					temp->next = NULL;
				}

				if(erase != Heap){
					free(erase);
					erase = NULL;
				}
			}
		}
			
	return temp;
}



