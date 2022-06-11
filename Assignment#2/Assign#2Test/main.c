#include <stdio.h>
#include <stdlib.h>
#include "movie_io.h" 

/****DESCRIPTION****/
// - read a csv file as an arg to main
// - create structs to hold each movie
// - create a linked list of these structs
// - give user choices to answer questions 
//   and create files containing info 
//   about the movie data

/*** REFERENCES ***/
// https://en.cppreference.com/w/c/string/byte/strtok
// https://en.cppreference.com/w/c/memory/calloc
// https://linux.die.net/man/3/getline
// mergesort help (JAVA): 
// 	- https://www.youtube.com/watch?v=pNTc1bM1z-4
// 	- https://www.geeksforgeeks.org/c-program-for-merge-sort-for-linked-lists/

/* see header files for function descriptions */

int main(void)
{
	int value = 0;

	/*
	int movieCount = 0;
	int *mcPtr = &movieCount;
	struct movie *sortedList = 0;
	struct movie *uniqueYears = 0;
	struct movie *ref1 = 0;
	// for the languages in each movie
	struct node *ref2 = 0;
	struct node *ref3	= 0;
	*/
	/*
	if (argc < 2)
	{
		printf("Provide a file to process.\n");
		return 1;
	}
	struct movie *list = _processFile(argv[1], mcPtr);
	sortedList = _mergeSort(list);
	ref1 = sortedList;
	*/
	// create the unique year list for the 
	// _sortByRating function
	//uniqueYears = _createUniqueYearList(sortedList);

	//printf("\nProcessed file: %s and parsed data for %u movies.\n",
		//argv[1], movieCount);

	while (value != 2)
	{
		printMovieMenu();
		value = getMenuChoice();
		printMenuChoices(value);
	}

	printf("\n");

	/*
	while (sortedList != 0)
	{
		ref1 = sortedList;
		free(ref1->title);
		ref2 = sortedList->languages;
		while (ref2 != 0)
		{
			ref3 = ref2;
			free(ref3->val);		
			free(ref3);
			ref2 = ref2->next;
		}
		free(ref1);
		sortedList = sortedList->next;
	}
	free(sortedList);
	while (uniqueYears != 0)
	{
		ref1 = uniqueYears;
		uniqueYears = ref1->next;
		free(ref1);
	}
	*/

	return 0;
}