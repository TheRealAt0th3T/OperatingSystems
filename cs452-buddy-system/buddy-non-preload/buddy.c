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
#define  MIN_KVAL  10  //
#define  KVAL      29  //
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



int buddy_init(size_t size){ 
    bHeader *bh; //pointer to the header
	int i = 0;

	if(!initialized){
		size = pow(2, ceil(log(size)/log(2))); //rounds up request to the next power of 2
		pool.lgsize = log2(size); //kval (size = 2^kval)

		if (pool.lgsize > MAX_KVAL ) { //if kval larger than MAX_KVAL (37)
			size = 1UL << MAX_KVAL; //resize 
			pool.lgsize = MAX_KVAL; //set to kval 
		}

		if (pool.lgsize < MIN_KVAL) { //if lower than MIN_KVAL
			size = 1UL << KVAL;	//resize
			pool.lgsize = KVAL;	//set to MIN_KVAL
		}

		pool.start = sbrk(size);

		/*
		if(size < 1 && size != 0){
			return ENOMEM;
		}

		for(i = 0; i< MAX_KVAL; i++){
			pool.avail[i].tag = UNUSED;
			pool.avail[i].kval = i;
			pool.avail[i].next = &pool.avail[i];
			pool.avail[i].prev = &pool.avail[i];
		}

		pool.avail[i].next = pool.start;
		pool.avail[i].prev = pool.start;
		*/

		if(pool.size < 0){ //allocation error
			return -ENOMEM; 
		}

		for(i = 0; i < pool.lgsize; i++){
			pool.avail[i].tag = RESERVED;
			pool.avail[i].kval = i;
			pool.avail[i].next = &pool.avail[i];
			pool.avail[i].prev = &pool.avail[i];
		}

		//last iteration of i
		pool.avail[i].tag = FREE;
		pool.avail[i].kval = pool.lgsize;
		pool.avail[i].next = pool.start;
		pool.avail[i].prev = pool.start;

		// 
		bh = pool.avail[i].next; 
		bh->tag = FREE;
		bh->kval = i;
		bh->next = &pool.avail[i];
		bh->prev = &pool.avail[i];

	}
	
	initialized = TRUE; //now program knows buddy_init has already been called and performed
	return TRUE;
}

/**
 * @brief Allocating dynamic memory. 
 * If size not power of 2, will roundup.
 * 
 * @param size amount of mem for new block
 * @return void* pointer to new mem block
 */
void *buddy_malloc(size_t size){
	int i = 0; // for loops

	if(size == 0){ //if size =0, break out and do nothing
		return NULL;
	}

	if(!initialized){	
		buddy_init(1 << KVAL); //---------------why the need to resize
	}

	bHeader *h1 = NULL;
	bHeader *h2 = NULL;

	size_t totalBlockSize = powerUpSize(size + sizeof(bHeader)); //need to include size of header in each block & powerUp
	int kval = getKval(totalBlockSize);

	if(kval > pool.lgsize){ //checking if kval within bounds
		errno = ENOMEM;
		return (void*) NO_MEM;
	}

	i = getBlockFromPool(kval);	//getting block from the list/pool (dictated by kval)
	
	if(i != -1){ //if we found the block --------------------why not if unused
		h1 = pool.avail[i].next; //pointing at pool.start
		
		//setting new blocks header information
		h1->tag = RESERVED;
		h1->kval = kval;
		pool.avail[i].next = h1->next;
		h1->next->prev = &pool.avail[i];

		while(i != kval){
			i--; 
			
			h2 = (bHeader*)(long)h1 + (1<<i); 

			h2->tag = FREE;
			h2->kval = i;
			h2->next = &pool.avail[i];
			h2->prev = &pool.avail[i];

			pool.avail[i].tag = FREE;		//-------why no kval here
			pool.avail[i].next = h2;
			pool.avail[i].prev = h2;
		}
	}else{	//no block space found
		errno = ENOMEM;
		return (void*) NO_MEM;
	}
	
	h1++; //incrementing pointer

	return (void*) h1;
}


/**
 * @brief allocate and clear mem. 
 * wrapper func to call buddy_malloc()
 * 
 * @param nmemb members needed
 * @param size each member size
 * @return void* to the start of the array of members
 */
void *buddy_calloc(size_t nmemb, size_t size){
	
	size_t nPower = powerUpSize(nmemb * size);
	void *newMemBlock = buddy_malloc(nPower);

	int i = 0; 
	while(i < nPower){
		newMemBlock = (void *)'0'; //clear mem and set to '0'
		newMemBlock = 1 + (char *)newMemBlock;
		i++;
	}
	return newMemBlock;
}

/**
 * @brief Changes size of mem block. 
 * if ptr = null, following is essentially buddy_malloc(size),
 * if size = 0, then essetially is buddy_free(ptr).
 * 
 * @param ptr to existing mem block
 * @param size new mem block size
 * @return void* ptr to resized block
 */
void *buddy_realloc(void *ptr, size_t size){ //if ptr = NULL, call equivalent to this
	if (ptr == NULL){
		return malloc(size);
	}
	
	if(size == 0){	 //if size = 0, call equivalent to this
		return free(ptr); //--------------------type? ??? ?   ? ?  
	}

	bHeader *h1 = (bHeader *)ptr -1; 

	if(sizeof(bHeader) >= size){	//if block header > size then good 
		return ptr;
	}

	void *ret = buddy_malloc(size);

	if(ret){	// ------------------------------?? ? ?  ? ??    ? ? ? ??
		memcpy(ret, ptr, sizeof(bHeader));
		free(ptr);
	}

	return ret;
}

/**
 * @brief frees mem space ptr is pointing at, which was returned by all alloc()'s.
 * If free has been called, undef occrus, if NULL, no op is done.  
 * 
 * @param ptr to called on mem block 
 */
void buddy_free(void *ptr){
	if(ptr == NULL){	//if ptr is null, no op is done
		return; 
	}

	bHeader *h1 = (bHeader *)(ptr) - 1; //minus size of headr 
	int kval = h1-> kval; //getting headers kval

	void* buddyAddy = getBuddy(kval, (void *)h1);
	bHeader *h2 = (bHeader *)(buddyAddy);

	//combining buddies
	//-----------------------------? C? ?? ? ? DISCREPENSIES WITH NOTES AND WHILE LOOP-----------------------
	// while k not max || h2 tag is not reserved || tag h2 is free but not the right kval
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
 * @brief prints list of available blocks in buddy sys
 * 
 */
void printBuddyLists(){
	
	int blocksFree = 0; 
	int i = 0; //going to be our kval
	while(i <= pool.lgsize){ 
		printf("List %d head = %p --> [tag=%d,kval=%d,address=%p] ", i , &pool.avail[i], pool.avail[i].tag, pool.avail[i].kval, pool.avail[i].next);
		
		bHeader* h1 = &pool.avail[i];

		while (h1->next != &pool.avail[i]) // while not at end of pool
		{
			printf("-->%p", h1);
			h1 = h1->next;
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
 * @brief Getting kval from size 
 * size = 2^kval
 * 
 * @param blockSize 
 * @return kval 
 */
int getKval(size_t blockSize){
	int kval = log2(blockSize);
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
		if(pool.avail[bNeed].next != &pool.avail[bNeed]){	//--------------------What is this 
			return bNeed;
		}
		bNeed++;
	}
	return -1; 
}

