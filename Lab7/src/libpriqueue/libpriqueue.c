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
  // Allocate and initialize the link without taking ownership of item.
  node_t *node = malloc(sizeof(*node));
  if (node == NULL)
    return NULL;

  node->item = item;
  node->next = next;
  return node;
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
  if (node == NULL)
    return NULL;

  // Preserve the stored pointer; only the node itself belongs to this list.
  void *item = node->item;
  free(node);
  return item;
}

/**
  Recursively destroys a linked list beginning at the specified node.

  Each node in the list is freed, starting with the last node and
  proceeding backward to the first. The data pointed to by each
  node's item field is not freed.

  @param node a pointer to the first node in the list, or NULL
*/

void destroy_list (node_t *node) {
  if (node == NULL)
    return;

  // Free from the tail backward so each next pointer remains valid.
  destroy_list(node->next);
  destroy_node(node);
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
  if (index < 0 || node == NULL)
    return NULL;
  if (index == 0)
    return node;

  // Advance one link while reducing the remaining offset.
  return node_at(node->next, index - 1);
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
  if (node == NULL)
    return 0;

  // Count this node and recursively count the rest of the chain.
  return 1 + list_size(node->next);
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
  if (q == NULL)
    return;

  // Keep the caller's ordering rule and start with an empty queue.
  q->comparer = comparer;
  q->top = NULL;
}


/**
  Insert the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
  if (q == NULL || q->comparer == NULL)
    return -1;

  node_t **position = &q->top;
  int index = 0;

  // Find the first lower-priority position; passing equals keeps insertion stable.
  while (*position != NULL && q->comparer((*position)->item, ptr) <= 0) {
    position = &(*position)->next;
    index++;
  }

  node_t *node = new_node(ptr, *position);
  if (node == NULL)
    return -1;

  *position = node;
  return index;
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
  // The head is the highest-priority item; do not modify the queue.
  return q == NULL || q->top == NULL ? NULL : q->top->item;
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
  // Removing index zero implements polling while sharing unlink logic.
  return priqueue_remove_at(q, 0);
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
  // Resolve the node first so invalid indices naturally return NULL.
  node_t *node = q == NULL ? NULL : node_at(q->top, index);
  return node == NULL ? NULL : node->item;
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
  if (q == NULL)
    return 0;

  int removed = 0;
  node_t **position = &q->top;
  // Compare item addresses and keep the link in place after a deletion.
  while (*position != NULL) {
    if ((*position)->item == ptr) {
      node_t *node = *position;
      *position = node->next;
      destroy_node(node);
      removed++;
    } else {
      position = &(*position)->next;
    }
  }

  return removed;
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
  if (q == NULL || index < 0)
    return NULL;

  node_t **position = &q->top;
  // Walk to the requested link, returning NULL if the list ends first.
  while (*position != NULL && index > 0) {
    position = &(*position)->next;
    index--;
  }
  if (*position == NULL)
    return NULL;

  node_t *node = *position;
  *position = node->next;
  return destroy_node(node);
}


/**
  Return the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
  // Queue size is the number of nodes in its linked list.
  return q == NULL ? 0 : list_size(q->top);
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
  if (q == NULL)
    return;

  // Release nodes only; item pointers remain owned by the caller.
  destroy_list(q->top);
  q->top = NULL;
  q->comparer = NULL;
}
