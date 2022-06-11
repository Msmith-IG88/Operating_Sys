#include <stdlib.h>

// recursive function that divides the movie linked list
// by two. Stores the pointer location of the beginning
// and middle. 
// input: a linked list
// output: a reference to the location of a new linked
// 	list that is at most (list length)/2
struct movie* _mergeSort(struct movie*);

// function takes two pointers to two halves of a 
// complete list, compares and sorts by movie year,
// in ascending order.
// input: two linked lists
// output: a sorted linked list
struct movie* _merge(struct movie*, struct movie*);
