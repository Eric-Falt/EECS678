#ifndef LIST_QUESTIONS_H
#define LIST_QUESTIONS_H

#include "LinkedList.h"
#include <stdbool.h>
/*
 * Problem: Determine if a linked list has a cycle
 *
 * Description:
 * Given the head of a linked list, determine if the list contains a cycle. A cycle occurs if a node can be
 * reached again by continuously following the `next` pointer. Internally, 'pos' is used to denote the index
 * of the node that the tail's next pointer is connected to, indicating a cycle. Note that 'pos' is not
 * accessible or passed as a parameter; it's only used for problem understanding and explanation.
 *
 * Task:
 * Implement a function to check if the given linked list has a cycle. The function should return 'true' if a
 * cycle is present and 'false' otherwise.
 *
 * Prototype:
 * bool hasCycle(struct Node *head);
 */

static bool hasCycle(struct Node *head)
{
	// makes sure list passed isn't empty
	if (head != NULL)
	{
		struct Node *tempNode = head;
		struct Node *tempNode2 = head->next;

		while (tempNode2 != NULL)
		{
			if (tempNode == tempNode2)
			{
				return true;
			}

			tempNode = tempNode->next;
			tempNode2 = tempNode2->next;
		}
	}
	return false;
}

/*
 * Problem: Merge Two Sorted Lists
 *
 * Description:
 * You are given the heads of two sorted linked lists, list1 and list2. Your task is to merge these two
 * lists into one single sorted list. The merged list should be constructed by splicing together the nodes
 * of the first two lists without creating new nodes, but by rearranging the nodes from the given lists.
 *
 * Task:
 * Implement a function that merges two sorted linked lists and returns the head of the newly merged sorted
 * linked list.
 *
 * Prototype:
 * struct Node* mergeTwoLists(struct Node* list1, struct Node* list2);
 *
 * Note:
 * Both list1 and list2 are sorted in non-decreasing order.
 */

static struct Node *mergeLists(struct Node *list1, struct Node *list2)
{
	// holds the head and tail of the new sorted list
	struct Node *sortedListHead = NULL;
	struct Node *sortedListTail = NULL;

	// runs while both lists have nodes
	while (list1 != NULL && list2 != NULL)
	{
		// if the data in list 1 is smaller add it to the merged list
		if (list1->data <= list2->data)
		{
			// if its the first node set the head and tail otherwise just update the tail
			if (sortedListHead == NULL)
			{
				sortedListHead = list1;
				sortedListTail = list1;
			}
			else
			{
				sortedListTail->next = list1;
				sortedListTail = list1;
			}

			// traverse list1 by one node
			list1 = list1->next;
		}
		else
		{
			// does the same thing as the first case just with list2
			if (sortedListHead == NULL)
			{
				sortedListHead = list2;
				sortedListTail = list2;
			}
			else
			{
				sortedListTail->next = list2;
				sortedListTail = list2;
			}

			list2 = list2->next;
		}
	}

	// if only one list started with elemenets the head becomes the populated list, otherwise it adds the rest of the list to the tail
	if (sortedListHead == NULL)
	{
		if (list1 != NULL)
		{
			sortedListHead = list1;
		}
		else
		{
			sortedListHead = list2;
		}
	}
	else if (list1 != NULL)
	{
		sortedListTail->next = list1;
	}
	else if (list2 != NULL)
	{
		sortedListTail->next = list2;
	}

	return sortedListHead;
}

#endif
