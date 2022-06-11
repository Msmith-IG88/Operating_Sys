#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "movie_io.h"

/****DESCRIPTION****/
// - read a csv file as an arg to main
// - create structs to hold each movie
// - create a linked list of these structs
// - give user choices to answer questions about the movie data
// - prints info about movies based on user choices
//

/*** REFERENCES ***/
// https://en.cppreference.com/w/c/string/byte/strtok
// https://en.cppreference.com/w/c/memory/calloc
// https://linux.die.net/man/3/getline
//

// reads a line of input from a file and allocates
// memory for a movie struct
// input: a csv line
// output: a movie type containing the csv contents.
struct movie* _createMovie(char* line)
{
	struct movie *currMovie = malloc(sizeof(struct movie));
	// blank node for a new langauge
	struct node *newLang = 0;
	
	struct node *tempLang = 0;
	struct node *langList = 0;
	char *refToken;
	char *saveLangToken;
	char *savePtr;
	int colonCount = 0;
	size_t i = 0;

	// movie title
	char* token = strtok_r(line, ",", &savePtr);
	currMovie->title = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currMovie->title, token);

	// movie year
	token = strtok_r(NULL, ",", &savePtr);
	currMovie->year = atoi(token);

	// movie languages
	//token = strtok_r(NULL, ";", &savePtr);
	saveLangToken = savePtr;
	token = strtok_r(NULL, ",", &savePtr);

	// get rid of the opening bracket
	//langToken++;
	token++;

	// get rid of the closing bracket
	token[strlen(token) - 1] = '\0';

	// preserve the location of the read location
	refToken = token;

	// movie rating
	token = strtok_r(NULL, "\n", &savePtr);
	currMovie->rating = atof(token);

	// count the number of languages
	// NOTE: compiler says that this for loop is not used. After
	// examining GDB, it has to be used because colonCount is 
	// incrementing. I suspect that the gdb warning occurs when 
	// there is only one language, in which case, this logic
	// would not be used.
	for (i; i < strlen(refToken); ++i)
	{
		if (refToken[i] == ';')
		{
			colonCount++;	
		}
	}

	// the number of languages will equal
	// colonCount + 1. Since there is at least always
	// one language, read it and loop over the remaining
	// languages, if any.
	token = strtok_r(refToken, ";", &saveLangToken);
	newLang = malloc(sizeof(struct node));
	newLang->val = calloc(strlen(token) + 1, sizeof(char));
	strcpy(newLang->val, token);

	// save the starting point of the list
	langList = newLang;

	// since i is only used in loops, it does not need
	// to be reset, *it seems*.
	i = 0;

	while (i < colonCount)
	{
		tempLang = newLang;
		token = strtok_r(NULL, ";", &saveLangToken);
		newLang = malloc(sizeof(struct node));
		tempLang->next = newLang;
		newLang->val = calloc(strlen(token) + 1, sizeof(char));
		newLang->next = 0;
		strcpy(newLang->val, token);
		i++;
	}

	// point the list of languages to the movie language list
	currMovie->languages = langList;

	currMovie->next = NULL;
	
	return currMovie;
}

// creates a linked list of movies, using
// 	_createMovie as a helper function.
// input: an csv file and an address reference to 
// 	the number of movies read from the file.
// output: a linked list (unsorted) of movies.
struct movie *_processFile(const char* filePath, int *size)
{
	FILE* movieFile = fopen(filePath, "r");
	
	if (!movieFile)
	{
		printf("oh no\n");
		perror("Failed to open file\n");
		exit(EXIT_FAILURE);
	}

	size_t len = 0;
	ssize_t read;
	char* currLine = NULL;
	struct movie* head = NULL;
	struct movie* tail = NULL;

	// read the first line and discard
	read = getline(&currLine, &len, movieFile);

	while ((read = getline(&currLine, &len, movieFile)) != -1)
	{
		// if the line is blank
		if (currLine[0] == '\n') 
		{
			continue;
		}
		else 
		{
			struct movie *newNode = _createMovie(currLine);

			// used as a reference to keep track of the number of 
			// movies in the provided file.
			(*size)++;

			// create the first node if empty
			if (head == NULL)
			{
				head = newNode;
				tail = newNode;
			}
			else
			{
				tail->next = newNode;
				tail = newNode;
			}
		}
	}
	free(currLine);
	fclose(movieFile);
	return head;
}

// creates a list of unique years for use in the 
// _sortByRating function. Reduces the repeated 
// allocation of memory when option 2 is chosen.
struct movie* _createUniqueYearList(struct movie* list)
{
	struct movie *uniqueYears = 0;
	struct movie *tempNode = 0;
	struct movie *newNode = 0;

	// The list is presorted in asc order, therefore the first
	// value read is assigned to the first node in the uniqueYear
	// linked list.
	
	newNode = malloc(sizeof(struct movie));
	newNode->year = list->year;
	newNode->title = list->title;
	newNode->rating = list->rating;
	newNode->next = 0;
	uniqueYears = newNode;

	while (list != 0)
	{
		if (newNode->year != list->year)
		{
			tempNode = newNode;
			newNode = malloc(sizeof(struct movie));
			tempNode->next = newNode;
			newNode->title = list->title;
			newNode->year = list->year;
			newNode->rating = list->rating;
			newNode->next = 0;
		}
		list = list->next;
	}

	return uniqueYears;
}

// finds a string within a linked list of lang nodes.
// input: the string being searched for 
// 	and the string being searched.
// output: int representing true or false.
int _findStr(char const *str, char const *subStr)
{
	// lazy check of the user input
	char *pos = strstr(str, subStr);

	// strstr return true for substrings as well as
	// substrings of substrings. 
	// if the user input a str that is the length of the 
	// language and it matches
	if (pos && strlen(str) == strlen(subStr))
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

// prints the list of languages for that movie
// input: a list of languages 
// output: stdout
void _printLanguages(struct node *langs)
{
	while (langs != 0)
	{
		printf("%s ", langs->val);	
		langs = langs->next;
	}
}

// prints a node in the linked list of movies
// input: a linked list of movies
// output: stdout
void _printMovie(struct movie *aMovie)
{
	struct node *languages = aMovie->languages;

	printf("%s, %d",
		aMovie->title,
		aMovie->year);
	_printLanguages(languages),
	printf("%f", aMovie->rating);
}

// prints the linked list of movies by title only
// input: a linked list of movies, title only
// output: stdout
void _printByTitle(struct movie *aMovie)
{
	printf("%s\n", aMovie->title);
}

// prints the entire linked list of movies,
// using _printMovie as a helper function.
// input: a linked list of movies
// output: stdout
void _printMovieList(struct movie *list)
{
	while (list != NULL)
	{
		_printMovie(list);
		list = list->next;
	}
}

// prints the menu instructions for the user
// to follow.
// input: stdin
// output: stdout
void printMovieMenu()
{
	printf("\n1. Show movies released in the specified year\n");
	printf("2. Show hightest rated movie for each year\n");
	printf("3. Show the title and year of release of all\n");
	printf("   movies in a specified language\n");
	printf("4. Exit from the program\n\n");
}

// when movie menu option 1 is chosen, the user is
// asked what year they would like to see movies for.
// input: a linked list of movies
// output: a list of movies created that match the 
// 	year the user enters to stdout.
void _showByYear(struct movie *list)
{
	int value = 0;
	int resultCount = 0;
	int input;	

	printf("\nEnter the year for which you want to see movies: ");

	scanf("%d", &input);

	value = input;

	while (list != NULL)
	{
		if (list->year == value)
		{
			_printByTitle(list);
			resultCount++;
		}
		list = list->next;
	}

	if (resultCount == 0)
	{
		printf("No data about movies exists for that year.\n");
	}
}

// when movie menu option 2 is chosen, the user does
// not input any values. Instead, a list of unique years
// is created and destroyed. Movies with the same year 
// have their rating compared and swapped using the greater
// of the two. 
// input: a linked list of movies
// output: stdout of each year that has a movie with the
// highest rating.
void _showByRating(struct movie *list, struct movie *uniqueList)
{
	//struct movie *uniqueYear = 0;
	//struct movie *headRef = list;
	//struct movie *tempNode = 0;

	while (uniqueList != 0)
	{
		if (list != 0)
		{
			// check all the years that == the 
			// year in the uniqueYear node
			while (list->year == uniqueList->year)
			{
				if (list->rating > uniqueList->rating)
				{
					uniqueList->title = list->title;
					uniqueList->year = list->year;
					uniqueList->rating = list->rating;
				}
				if (list->next != 0)
				{
					list = list->next;
				} else {break;}
			}
		}
		printf("%u ", uniqueList->year);
		printf("%0.1f ",uniqueList->rating);
		printf(uniqueList->title);
		printf("\n");
		uniqueList = uniqueList->next;
	}
}

// when movie menu option 3 is chosen, asks the user to enter
// a language that they would like to search the movie list for.
// uses _findStr helper function to search the languages field of the 
// movie, exact matches only. When a movie is with that language is
// found, prints that movie to stdout.
// input: a linked list of movies
// output: stdout of movies that are in the language entered by the user.
void _showByLanguage(struct movie *list)
{
	char lang[20];
	size_t matchCount = 0;
	struct node *langRef = list->languages;
	
	printf("\nEnter the language for which you want to see movies: ");
	scanf("%s", lang);

	while (list != 0)
	{
		while (langRef != 0)
		{
			if (_findStr(langRef->val, lang))
			{
				matchCount++;	
				printf("%d %s\n", list->year, list->title);
			}
			langRef = langRef->next;
		}

		list = list->next;

		if (list != 0)
		{
			langRef = list->languages;
		}
	}

	if (matchCount == 0)
	{
		printf("\nNo movies exist with that langauge.\n");
	}
}

// asks the user to enter a choice and gets the user input
// input: none
// output: an integer representing the valid user input.
int getMenuChoice()
{
	// only expect one char to be entered. no more, no less.
	char str[2];
	char *str1 = NULL;
	int choice;
	
	printf("Enter a choice from 1 to 4: ");
	scanf("%s", str);

	str1 = malloc(sizeof(char));
	strcpy(str1, str);

	choice = atoi(str1);

	while (str[1] != '\0' || (choice < 1 || choice > 4))
	{
		
		strcpy(str, "");
		choice = 0;

		printf("Not a valid choice, try again.\n");
		printf("Enter a choice from 1 to 4: ");
		scanf("%s", str);
		strcpy(str1, str);
	
		choice = atoi(str1);	
		
	}

	free(str1);

	return choice;	
}

// calls the given function the corresponds to the menu
// choice selected by the user.
// input: an integer representing the movie menu choice and
// 	a linked list of movies.
// output: the resulting output of the choices made.
void printMenuChoices(int val, struct movie *list, struct movie *uList)
{
	switch(val)
	{
		case 1: _showByYear(list); break;
		case 2: _showByRating(list, uList); break;
		case 3: _showByLanguage(list); break; 	
		default: break;
	}
}