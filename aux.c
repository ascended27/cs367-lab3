#include <stdlib.h>
#include "memory.h"
#include "aux.h"

extern int probes;
extern int numLists;
extern mem_ptr Heap;
extern mem_ptr* segLists;

// FROM MM_FREE.C
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

void coalesce(mem_ptr p){

   int done = 0;
   int i;
   while(!done){
   for(i = 0; i < numLists; i++){

      mem_ptr cur = segLists[i]->next;

      while(cur){
    
         int addr = p->address + p->size;
         int curAddr = cur->address + cur->size;

         if(curAddr == p -> address ||  addr == cur -> address){

            p -> size += cur -> size;
            if(p->address > cur->address)
               p->address = cur->address;
            if(cur -> previous)
               cur -> previous -> next = p;
            if(cur -> next)
               cur -> next -> previous = p;
            else
               segLists[i] -> next = NULL;
            free(cur);
            
            //coalesce(p);
            done = 0;
           
         }            
            cur = cur -> next;
         }
         if(p->previous)
             p -> previous -> next = p -> next;
         if(p-> next)
             p -> next -> previous = p -> previous;
         done = 1;
      }

   }
   placeInSegList(p);   
   

}

// FROM MM_MALLOC.C
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
                return NULL;
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
        p -> next = node;
        node -> previous = p;
        node -> next = NULL;
        return -1;
}

