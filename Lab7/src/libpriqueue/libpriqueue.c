/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"

// Node helper methods

/**
  Creates a new node containing the specified item and a pointer to
  the next node in the linked list.

  @param item a pointer to the data to be stored in the node
  @param next a pointer to the next node in the linked list, or NULL
  @return a pointer to the newly allocated node
*/

node_t *new_node (void *item, node_t *next) {
  node_t *newNode = malloc(sizeof(node_t));

  newNode->item = item;
  newNode->next = next;

  return newNode;
}

/**
  Destroys a single node and returns a pointer to its stored data.

  The memory allocated for the node is freed, but the data pointed
  to by the node's item field is not freed.

  @param node a pointer to the node to be destroyed
  @return a pointer to the data previously stored in the node
  @return NULL if node is NULL
*/

void *destroy_node (node_t *node) {
  void *data = NULL;

  if (node != NULL)
  {
    data = node->item;

    free(node);
  }
  
  return data;
  
}

/**
  Recursively destroys a linked list beginning at the specified node.

  Each node in the list is freed, starting with the last node and
  proceeding backward to the first. The data pointed to by each
  node's item field is not freed.

  @param node a pointer to the first node in the list, or NULL
*/

void destroy_list (node_t *node) {

}

/**
  Retrieves the node at the specified index in a linked list.

  The list is traversed recursively, with an index of 0 referring
  to the first node. No nodes are removed or modified.

  @param node a pointer to the first node in the linked list
  @param index the zero-based position of the node to retrieve
  @return a pointer to the node at the specified index
  @return NULL if the index is invalid or does not exist
*/

node_t *node_at (node_t *node, int index) {

}

/**
  Calculates the number of nodes in a linked list.

  The list is traversed recursively, counting each node until
  the end of the list is reached.

  @param node a pointer to the first node in the linked list
  @return the total number of nodes in the list
  @return 0 if node is NULL
*/

int list_size(node_t *node) {

}


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements. If comparer(x, y) < 0, then the priority of x is higher than the priority of y and therefore should be placed earlier in the queue.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
  
}


/**
  Insert the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{

}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{

}	


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{

}




/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
  
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
  
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{

}


/**
  Return the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{

}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{

}
