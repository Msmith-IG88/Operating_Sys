#include "movie_io.h"
#include "movie_algo.h"

struct movie *_merge(struct movie *sideA, struct movie *sideB)
{
	struct movie *sortedTemp = malloc(sizeof(struct movie));
	struct movie *currentNode = sortedTemp;
	struct movie *ref = sortedTemp;
	
	while (sideA != 0 && sideB != 0)
	{	
		if (sideA->year < sideB->year)
		{
			currentNode->next = sideA;
			sideA = sideA->next;
		}
		else 
		{
			currentNode->next = sideB;
			sideB = sideB->next;
		}

		currentNode = currentNode->next;
	}
	
	if (sideA != 0)
	{
		currentNode->next = sideA;
		sideA = sideA->next;
	}
	if (sideB != 0)
	{
		currentNode->next = sideB;
		sideB = sideB->next;
	}	
	
	ref = sortedTemp;
	free(sortedTemp);
	return ref->next;
}

struct movie *_mergeSort(struct movie *head)
{

	// check if there is only one node in the linked list
	if (head == 0 || head->next == 0)	
	{
		return head;
	}

	// create a fast and slow pointer that will divide
	// the linked list into two halves.
	struct movie *slow = head;
	struct movie *fast = head;
	struct movie *temp = head;
	struct movie *listA = 0;
	struct movie *listB = 0;

	while (fast != 0) 
	{
		fast = fast->next;
		if (fast != 0)
		{
			temp = slow;
			slow = slow->next;
			fast = fast->next;
		}
	}	

	temp->next = 0;

	listA = _mergeSort(head);
	listB = _mergeSort(slow);
	
	return _merge(listA, listB);
}