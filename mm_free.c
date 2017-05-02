#include <stdlib.h>
#include "memory.h"


int placeInSegList(mem_ptr m);
void coalesce(int location);

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
   if(m){
      //printf("free(%d)\n",m->size);    
      int size = 0;
      temp->valid = 0;
	
      if((temp->previous != NULL)&&(temp->next != NULL)){
   	   temp->previous->next = temp->next;
	   temp->next->previous = temp->previous;
      }
      else if((temp->previous != NULL)&&(temp->next == NULL)){
  	   temp->previous->next = NULL;
      }
      else if((temp->previous == NULL)&&(temp->next != NULL)){
	   temp->next->previous = NULL;
	   Heap = temp->next;
   	   temp->next = NULL;
      }
      else if((temp->previous == NULL)&&(temp->next == NULL)){
           Heap = NULL;
      }	
      temp -> next = NULL;
      temp -> previous = NULL;
      int location = placeInSegList(temp);
      if(location != -1)
         coalesce(location);
   } else{
      error_msg(2);
   }	
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
						list -> previous = temp;
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

void coalesce(int location){
	
//temporary pointer
	mem_ptr erase = NULL;
	
	int size = 0;
	int minSize = segLists[location] -> minSize;
	int maxSize = segLists[location] -> maxSize;
        mem_ptr tmp = NULL;
	mem_ptr p = segLists[location]->next;
  
        //loop over seg list
	while(p->next){

           size = p->address + p->size;	   
           if(size == p->next->address){
              p -> size += p->next->size;
	      erase = p -> next;
              // Erase the merged node
	      if(erase){
		p -> next = erase -> next;
	        if(erase->next){
		   erase -> next -> previous = p;	    
                   tmp = erase->next;
                }  
              }

	      // Remove p from this list
	      p -> previous -> next = p -> next;
              if(p -> next){
                 p -> next -> previous = p -> previous;
              }
              p -> next = NULL;
              p -> previous = NULL;

              // Place in new list
	      if(p->size > maxSize){
		int loc = placeInSegList(p);
                coalesce(loc);
              }

	      // Remove erase from this list
	      if(erase){
		free(erase);
              } else {
                 break;
              }
              p = tmp;
           }
	   if(!tmp)
		break;
	   if(!p)
		break;
           if(tmp)
	      p = tmp;
           else 
              p = p -> next;
	}
}



