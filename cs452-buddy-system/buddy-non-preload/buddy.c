/**
 * @file buddy.c
 * @author Amara Tariq
 */
 
#include "buddy.h"
int initialized = FALSE;

/* the header for an available block */
struct block_header {
	short tag; //tells whether block free/unused/reserved
	short kval; //helps where block goes plus size (2^kval)
	struct block_header *next;
	struct block_header *prev;
};

typedef struct block_header bHeader; //renaming block_header

/* All the tags for blocks */
const int RESERVED = 0;	
const int FREE = 1;
const int UNUSED = -1; /* useful for header nodes */


/* supports memory upto 2^(MAX_KVAL-1) (or 64 GB) in size */
#define  MAX_KVAL  37 //64gb
#define  MIN_KVAL  1  
#define  KVAL      29 //512MB
#define  NO_MEM    NULL

/* default memory allocation is 512MB */
const size_t DEFAULT_MAX_MEM_SIZE = 512*1024*1024;


/* A static structure stores the table of pointers to the lists in the buddy system.  */
struct pool {
	void *start; // pointer to the start of the memory pool
	int lgsize;  // log2 of size
	size_t size; // size of the pool, same as 2 ^ lgsize
	/* the table of pointers to the buddy system lists */
	struct block_header avail[MAX_KVAL];
} pool;


/**
 * Initialize the buddy system to the given size 
 * (rounded up to the next power of two)
 *
 * @return  TRUE if successful, ENOMEM otherwise.
 */
int buddy_init(size_t size){ 
    bHeader *bh; //pointer to the header
	int i = 0;

	if(!initialized){
		size = powerUpSize(size); //rounds up request to the next power of 2
		pool.lgsize = getKval(size); //calculating kval from size (size = 2^kval)

		if (pool.lgsize > MAX_KVAL ) { //if kval larger than MAX_KVAL (37)
			size = 1UL << MAX_KVAL; //resize 
			pool.lgsize = MAX_KVAL; //set to kval 
		}

		if (pool.lgsize < MIN_KVAL) { //if lower than MIN_KVAL
			size = 1UL << KVAL;	//resize
			pool.lgsize = KVAL;	//set to MIN_KVAL
		}

		pool.start = sbrk(size);

		if(pool.size < 0){ //allocation error
			return -ENOMEM; 
		}

		for(i = 0; i < pool.lgsize; i++){
			pool.avail[i].next = &pool.avail[i];
			pool.avail[i].prev = &pool.avail[i];
			pool.avail[i].tag = RESERVED;
			pool.avail[i].kval = i;
		}

		//last iteration of i
		pool.avail[i].next = pool.start;
		pool.avail[i].prev = pool.start;
		pool.avail[i].tag = FREE;
		pool.avail[i].kval = pool.lgsize;


		//header
		bh = pool.avail[i].next; 
		bh->next = &pool.avail[i];
		bh->prev = &pool.avail[i];
		bh->tag = FREE;
		bh->kval = i;

	}
	initialized = TRUE; //now program knows buddy_init has already been called and performed
	return initialized;
}



/**
 * Allocate dynamic memory. Rounds up the requested size to next power of two.
 * Returns a pointer that should be type casted as needed.
 * @param size  The amount of memory requested
 * @return Pointer to new block of memory of the specified size.
 */
void *buddy_malloc(size_t size){
	int i = 0; // for loops
	int kval = 0; 

	if(size == 0){ //if size = 0, break out and do nothing
		return NULL;
	}

	if(!initialized){	// we want to initialize before doing anything else
		buddy_init(1 << KVAL); 
	}

	bHeader *h1 = NULL;
	bHeader *h2 = NULL;

	size_t totalBlockSize = size + sizeof(bHeader); //need to include size of header in each block 
	totalBlockSize = powerUpSize(totalBlockSize);
	kval = getKval(totalBlockSize);

	if(kval > pool.lgsize){ //checking if kval within bounds
		errno = ENOMEM;
		return NULL;
		//return (void*)NO_MEM;
	}

	i = getBlockFromPool(kval);	//getting block from the list/pool (dictated by kval)
	
	if(i != UNUSED){ //(-1=tag UNUSED)if we found the block 
		h1 = pool.avail[i].next; //pointing at pool.start
		
		//setting new blocks header information
		pool.avail[i].next = h1->next;
		h1->next->prev = &pool.avail[i];
		h1->tag = RESERVED;
		h1->kval = kval;
		
		while(i != kval){
			i--; 
			
			long headerAddy = (long) h1 + (1<< i);
			h2 = (bHeader*)headerAddy; 

			h2->next = &pool.avail[i];
			h2->prev = &pool.avail[i];
			h2->tag = FREE;
			h2->kval = i;
			
			pool.avail[i].next = h2;
			pool.avail[i].prev = h2;
			pool.avail[i].tag = FREE;
		}
	}else{	//no block space found
		errno = ENOMEM;
		return NULL;
	}
	h1++; //incrementing pointer
	return (void*) h1;
}


/**
 * Allocate and clear memory to all zeroes. Wrapper function that just calles buddy_malloc.
 *
 * @param nmemb  The number of members needed
 * @param size   Size of each member
 * @return Pointer to start of the array of members
 */
void *buddy_calloc(size_t nmemb, size_t size){ 
    size_t alloc_size = nmemb * size;
    void *getP = buddy_malloc(alloc_size);

    if (getP == NULL){
        errno = ENOMEM;
        return NULL;
    }

    void *retP = memset(getP, 0, alloc_size);
    return retP;
}

/**
 * buddy_realloc() changes the size of the memory block pointed to by ptr to size bytes. 
 * The contents will be unchanged to the minimum of the old and new sizes; newly 
 * allocated memory will be uninitialized. If ptr is NULL, the call is equivalent 
 * to buddy_malloc(size); if size is equal to zero, the call is equivalent to buddy_free(ptr). 
 * Unless ptr is NULL, it must have been returned by an earlier call to buddy_malloc(), 
 * buddy_calloc() or buddy_realloc().
 *
 * @param  ptr Pointer to existing memory block
 * @param  size The new size of the memory block
 * @return The pointer to the resized block
 */
void *buddy_realloc(void *ptr, size_t size){ //if ptr = NULL, call equivalent to this
	void *ret;

	if(!initialized){
		buddy_init(size);
	}

	if (ptr == NULL){
		buddy_malloc(size);
	}
	
	if(size == 0){	 //if size = 0, call equivalent to this
		buddy_free(ptr);
	}

	//bHeader *h1 = (bHeader *)ptr -1; 

	if(sizeof(bHeader) >= size){	//if block header > size then good 
		return ptr;
	}

	ret = buddy_malloc(size);

	if(ret){
		memcpy(ret, ptr, sizeof(bHeader));
		buddy_free(ptr);
	}
	return ptr;
}

/**
 * buddy_free() frees the memory space pointed to by ptr, which must have been returned 
 * by a previous call to buddy_malloc(), buddy_calloc() or buddy_realloc(). Otherwise, 
 * or if buddy_free(ptr) has already been called before, undefined behaviour occurs. If 
 * ptr is NULL, no operation is performed. 
 * @param ptr Pointer to memory block to be freed
 */
void buddy_free(void *ptr){
	int kval;
	bHeader *h1 = NULL;
	bHeader *h2 = NULL;

	if(ptr == NULL){	//if ptr is null, no op is done
		return; 
	}

	h1 = (bHeader *)(ptr) - 1; //minus size of headr 
	kval = h1->kval; //getting headers kval

	void* buddyAddy;
	buddyAddy = getBuddy(kval, (void *)h1);
	h2 = (bHeader *)(buddyAddy);

	//combining buddies
	// while k not max || h2 tag not reserved || check h2 is free but not right kval
	while ( !(( kval == pool.lgsize ) || ( h2->tag == RESERVED ) || (( h2->tag == FREE) && ( h2->kval != kval)))) {
		h2->prev->next = h2->next;
		h2->next->prev = h2->prev;
		kval++;

		if(h2 < h1){
			h1 = h2;
		}
		h2 = (bHeader *)(getBuddy(kval, (void*) h1));
	}
	h1->tag = FREE;
	h1->next = pool.avail[kval].next;
	pool.avail[kval].next->prev = h1;
	h1->kval = kval;
	h1->prev = &pool.avail[kval];
	pool.avail[kval].next = h1;
}


/**
 * Prints out all the lists of available blocks in the Buddy system.
 */
void printBuddyLists(){
	
	int blocksFree = 0; 
	int i = 0; //going to be our kval

	while(i <= pool.lgsize){ 
		printf("List %d head = %p --> [tag=%d,kval=%d,address=%p] ", i , &pool.avail[i], pool.avail[i].tag, pool.avail[i].kval, pool.avail[i].next);
		
		bHeader* h = &pool.avail[i];

		while (h->next != &pool.avail[i]) // while not at end of pool
		{
			printf("-->%p", h);
			h = h->next;
		}

		printf("--> <null>\n");

		if(pool.avail[i].tag == FREE) // if the block is free
		{
			blocksFree++;
		}
		i++;
	}
	printf("\nNumber of free blocks = %d\n", blocksFree);
	blocksFree = 0;

}

/**
 * @brief Converts user inputted size to a power of 2
 * ie input = 511 this would help round it up to 512 which is 2^9
 * 
 * @param size the inputted size of mem block 
 * @return new block size
 */
size_t powerUpSize(size_t size){
	size_t blockSize = 1;
	
	while(blockSize < size){ //increase until meet base requirements
		blockSize *= 2; 
	}

	return blockSize; 
}

/**
 * @brief Getting kval from size (size = 2^kval)
 * @param blockSize 
 * @return kval 
 */
int getKval(size_t blockSize){
	int kval = 0;
	blockSize--;

	while (blockSize > 0) {
		blockSize >>= 1;
		kval++;
	}
	return kval; 
}

/**
*
* Using our kval, we can "map" to a block within pool
* @return blockNeeded
*/
int getBlockFromPool(int kval){
	int bNeed = kval;

	while(bNeed <= pool.lgsize){
		if(pool.avail[bNeed].next != &pool.avail[bNeed]){
			return bNeed;
		}
		bNeed++;
	}
	return -1; 
}

/**
 * @brief Get Buddy
 * @param kval 
 * @param header 
 * @return void* buddy
 */
void* getBuddy(int kval, void* header){
	long k = (long)kval;
	long h = (long)header;

	return (void*) (((1 << k) ^ (h - (long)pool.start)) + (long)pool.start);
}
