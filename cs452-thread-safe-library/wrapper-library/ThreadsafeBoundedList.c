
/**
*	ThreadsafeBoundedList.h: Defines the interface that wraps around a unsynchronized
*	linked list to create a threadsafe, bounded list.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <Node.h>
#include <List.h>

#define TRUE 1
#define FALSE 0
typedef int Boolean;

struct tsb_list {
 struct list *list;
 int capacity;
 Boolean stop_requested;
 pthread_mutex_t mutex;
 pthread_cond_t listNotFull; //meaning there is room to expand
 pthread_cond_t listNotEmpty;  //meaning theres stuff but not fully empty
};

/**
 * Constructor: Allocates a new listist object and initializes its members. Initialize
 * the mutex and condition variables associated with the bounded list monitor. Set the
 * capacity of the list.
 *
 * @return a pointer to the allocated list.
 */
struct tsb_list * tsb_createList(int (*equals)(const void *, const void *),
    char * (*toString)(const void *), void (*freeObject)(void *), int capacity){
    struct tsb_list * list = createList(equals, toString, freeObject);
    pthread_mutex_init(&list->mutex, NULL);
    pthread_cond_init(&list->listNotEmpty, NULL);
    pthread_cond_init(&list->listNotFull, NULL);
    list->capacity = capacity;
    list->stop_requested = FALSE;
}


/**
 * Frees all elements of the given list and the <code>struct *list</code> itself.
 * Does nothing if list is <code>NULL</code>. Also frees the associated mutex and
 * condition varibles and the wrapper structure.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_freeList(struct tsb_list * list){ /* destructor */
    pthread_mutex_lock(&list->mutex);
    pthread_cond_destroy(list);
    pthread_mutex_unlock(&list->mutex);
}
/**
 * Returns the size of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @return The current size of the list.
 */
int tsb_getSize(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    getSize(list);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Returns the maximum capacity of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @return The macimum capacity of the list.
 */
int tsb_getCapacity(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    int cap = list -> capacity;
    //-----------------------------------------------
    return cap;
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Sets the maximum capacity of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @param capacity the maximum allowed capacity of the list
 * @return none
 */
void tsb_setCapacity(struct tsb_list * list, int capacity){
    pthread_mutex_lock(&list->mutex);
    list -> capacity = capacity;
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Checks if the list is empty.
 *
 * @param  list a pointer to a <code>List</code>.
 * @return true if the list is empty; false otherwise.
 */
Boolean tsb_isEmpty(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    isEmpty(list);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Checks if the list is full.
 *
 * @param  list a pointer to a <code>List</code>.
 * @return true if the list is full to capacity; false otherwise.
 */
Boolean tsb_isFull(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    //isFull(list);
    if(tsb_getSize(list) == tsb_getCapacity(list)){
        return TRUE;
    }else{
        return FALSE;
    }
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Adds a node to the front of the list. After this method is called,
 * the given node will be the head of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void tsb_addAtFront(struct tsb_list * list, NodePtr node){
    pthread_mutex_lock(&list->mutex);
    while(list->list->size >= list->capacity){ //list is at/over capacity and cant fit more in
        pthread_cond_wait(&list->listNotFull , &list->mutex);
    }
    addAtFront(list, node);
    pthread_cond_signal(&list->listNotEmpty);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Adds a node to the rear of the list. After this method is called,
 * the given node will be the tail of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void tsb_addAtRear(struct tsb_list * list, NodePtr node){
    pthread_mutex_lock(&list->mutex);
    while(list->list->size >= list->capacity){ //list is at/over capacity and cant fit more in
        pthread_cond_wait(&list->listNotFull , &list->mutex);
    }
    addAtRear(list,node);
    pthread_cond_signal(&list->listNotEmpty);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Removes the node from the front of the list (the head node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeFront(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    while((list->stop_requested == FALSE) && (list->list->size == 0)){
        pthread_cond_wait(&list->listNotEmpty, &list->mutex);
    }
    removeFront(list);
    pthread_cond_signal(&list->listNotFull);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Removes the node from the rear of the list (the tail node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeRear(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    while((list->stop_requested == FALSE) && (list->list->size == 0)){
        pthread_cond_wait(&list->listNotEmpty, &list->mutex);
    }
    removeRear(list);
    pthread_cond_signal(&list->listNotFull);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Removes the node pointed to by the given node pointer from the list and returns
 * the pointer to it. Assumes that the node is a valid node in the list. If the node
 * pointer is NULL, the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to remove.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeNode(struct tsb_list * list, NodePtr node){
    pthread_mutex_lock(&list->mutex);
    while((list->stop_requested == FALSE) && (list->list->size == 0)){
        pthread_cond_wait(&list->listNotEmpty, &list->mutex);
    }
    removeNode(list, node);
    pthread_cond_signal(&list->listNotFull);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Searches the list for a node with the given key and returns the pointer to the
 * found node.
 *
 * @param list a pointer to a <code>List</code>.
 * @param the object to search for.
 * @return a pointer to the node that was found. Or <code>NULL</code> if a node 
 * with the given key is not found or the list is <code>NULL</code> or empty.
 */
NodePtr tsb_search(struct tsb_list * list, const void *obj){
    pthread_mutex_lock(&list->mutex);
    search(list, obj);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Reverses the order of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_reverseList(struct tsb_list *  list){
    pthread_mutex_lock(&list->mutex);
    reverseList(list);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Prints the list.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_printList(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    printList(list);
    pthread_mutex_unlock(&list->mutex);
}

/**
 * Finish up the monitor by broadcasting to all waiting threads
 */
void tsb_finishUp(struct tsb_list * list){
    pthread_mutex_lock(&list->mutex);
    list->stop_requested = TRUE;
    pthread_cond_broadcast(&list->listNotEmpty); //which cond signal should be broadcasted at
    pthread_mutex_unlock(&list->mutex);
}

