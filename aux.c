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

//TODO: Coalesce all seg lists not just this seglist
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

