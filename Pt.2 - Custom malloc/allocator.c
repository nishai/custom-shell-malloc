// allocator.c
#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

bool first = true;

/** Allocate a block of at least the requested size **/
void* custom_malloc(size_t size) {
    if (size == 0) {return NULL;}

    if (first){
        struct block* b;
        b = (struct block*) sbrk(MAX_SIZE);
        init_block(b, MAX_SIZE);
        head = b;
        first = false;
    }
    
    size = size + sizeof(struct block); //adds metadata size to requested size
    
    int power = 1;
    while (power < (int) size) {power = power * 2;}    //rounds up to nearest power of 2
    size = power;

    struct block* curr = head;
    while ((curr->size < size) || (!curr->free)){ //traverse to first free block with enough space
        curr = curr->next;
    }

    if (curr == NULL){
        return NULL;
    }
    else{
        while (curr->size > size){
            
            split(curr);
        }
        curr->free = false;
        return curr->data;
    }
}

void init_block(struct block* b, size_t size){  //initializes a new block to default values
    b->size = size;
    b->free = true;
    b->next = NULL;
    b->data = (void*)b + sizeof(struct block);
    b->buddy = NULL;
    for (int i = 0; i < MAX_EXP; ++i) {
        b->merge_buddy[i] = NULL;
    }
}

struct block* split(struct block* b){   
    size_t size = (size_t)((b->size)/2); 
    b->size = size;
    struct block* s = (struct block*) ((char*)b + size);
    init_block(s, size);  
    s->next = b->next;
    b->next = s;
    add_merge_buddy(b, s);
    s->buddy = b;
    b->buddy = s;
    return s;
}

void add_merge_buddy(struct block* og, struct block* new){
    int i = 0;
    while (og->merge_buddy[i] != NULL){i++;}
    og->merge_buddy[i] = og->buddy;
    i = 0;
    while (new->merge_buddy[i] != NULL){i++;}
    new->merge_buddy[i] = og->buddy;
}

/** Mark a data block as free and merge free buddy blocks **/
void custom_free(void* ptr) {
    if (ptr != NULL) {
        struct block* b = (struct block*)(ptr - sizeof(struct block));
        b->free = true;

        while ((b->buddy->size == b->size) && (b->buddy->free)){  
            if (b->buddy < b) {b = b->buddy;}

            int max = MAX_SIZE;
            if ((int)b->size != max/2){
                b->next = (b->buddy->next > b->next) ? b->buddy->next : b->next;
                b->size = b->size + b->buddy->size;
                b->buddy =  b->buddy->merge_buddy[0];
            } else {
                init_block(b, MAX_SIZE);
                break;
            }
        }
    }
}

/** Change the memory allocation of the data to have at least the requested size **/
void* custom_realloc(void* ptr, size_t size) {
    if (!ptr) {return custom_malloc(size);}   //NULL ptr - same as malloc
    struct block* b = (struct block*)(ptr - sizeof(struct block));
    
    size = size + sizeof(struct block);
    int power = 1;
    while(power < (int) size) {power = power * 2;}    
    
    if (power == b->size){  //same size: return same block
        return ptr;
    } else {     
        void* new = (void*) custom_malloc(size);
        struct block* n = (struct block*)(new - sizeof(struct block));
        memcpy(new, ptr, size);
        custom_free(ptr);
        return new;
    }
}

/*------------------------------------*\
|            DEBUG FUNCTIONS           |
\*------------------------------------*/

/** Prints the metadata of a block **/
void print_block(struct block* b) {
    if(!b) {
        printf("NULL block\n");
    }
    else {
        int i = 0;
        printf("Strt = %p\n",b);
        printf("Size = %lu\n",b->size);
        printf("Free = %s\n",(b->free)?"true":"false");
        printf("Data = %p\n",b->data);
        printf("Next = %p\n",b->next);
        printf("Buddy = %p\n",b->buddy);    
        printf("Merge Buddies = "); 
        while(b->merge_buddy[i] && i < MAX_EXP) {
            printf("%p, ",b->merge_buddy[i]);
            i++;
        }
        printf("\n\n");
    }
}

/** Prints the metadata of all blocks **/
void print_list() {
    struct block* curr = head;
    printf("--HEAP--\n");
    if(!head) printf("EMPTY\n");
    while(curr) {
        print_block(curr);
        curr = curr->next;
    }
    printf("--END--\n");
}
