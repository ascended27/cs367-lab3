#include <stdlib.h>
#include "memory.h"
#include "aux.h"

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
      //int location = placeInSegList(temp);
      coalesce(temp);
   } else{
      error_msg(2);
   }	
}

