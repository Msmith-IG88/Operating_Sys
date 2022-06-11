#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "movie_io.h"
#include <time.h>

#define POSTFIX ".csv"
#define PREFIX "movies_"
#define ONID "johnsal.movies."

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
	newLang->val[strlen(newLang->val)] = '\0';
	newLang->next = 0;

	// save the starting point of the list
	langList = newLang;

	// reset
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
		newLang->val[strlen(newLang->val)] = '\0';
		i++;
	}
	newLang->next = 0;

	// point the list of languages to the movie language list
	currMovie->languages = langList;

	currMovie->next = 0;
	
	return currMovie;
}

// destroys movies
void _destroyMovies(struct movie *list)
{
	struct movie *ref1 = 0;
	//struct movie *ref2 = 0;
	//struct node *ref3 = 0;
	//struct node *ref4 = 0;
	///*
	while (list != 0)
	{
		//printf("title: %s ", list->title);
		//printf(", year: %u ", list->year);
		ref1 = list;
		free(ref1->title);
		//ref3 = list->languages;
		/* // all my woes go here
		// this entire section worked in a1. now, it doesn't
		while (ref3 != 0)
		{
			//printf("ref3->val: %s ", ref3->val);
			ref4 = ref3;
			free(ref4->val); // the mem leak issue 
			free(ref4); // free the struct node (the lang node)
			ref3 = ref3->next;
		}
		free(ref3);
		*/
		//printf(", rating: %f \n", list->rating);
		free(ref1);
		list = list->next;
	}
	free(list);
	//*/
}

// creates a linked list of movies, using
// 	_createMovie as a helper function.
// input: an csv file 
// output: a linked list (unsorted) of movies.
//struct movie *_processFile(char* filePath)
struct movie *_processFile(FILE* movieFile)
{
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

	return head;
}

// creates a list of unique years for use in the 
// _sortByRating function. Reduces the repeated 
// allocation of memory when option 2 is chosen.
void _createUniqueYearFiles(const char *dirname, struct movie* list)
{
	FILE *newFile = 0;
	char cstr[9]; 
	// move into the <dirname> dir
	int newDir = chdir(dirname);
	if (newDir < 0)
	{
		perror("Error\n");
		exit(EXIT_FAILURE);
	}
	unsigned int currYear = list->year;
	struct movie *ref = list;

	while (list != 0)
	{
		if (currYear != list->year)
		{
			// update the year
			currYear = list->year;
			// create a new file <year>.txt
			sprintf(cstr, "%d", list->year);
			strcat(cstr, ".txt");
   		newFile = fopen(cstr, "a+");
			if (list != 0)
			{
				while (currYear == list->year)
				{
					fputs(list->title, newFile);
					fputs("\n", newFile);
					if (chmod(cstr, 0640) < 0)
					{
						perror("error changing file permissions\n");
						break;
					}
					if (list->next != 0)
					{
						list = list->next;
					} else {break;}
				}
			}
		}
		list = list->next;
	}

	list = ref;
}


// prints the menu instructions for the user
// to follow.
// input: stdin
// output: stdout
void printMovieMenu()
{
	printf("\n1. Select the file to process\n");
	printf("2. Exit from the program\n\n");
}


void _createDir(struct movie *list)
{
	srand(time(0));
	int newDir;
	char str[] = ONID;
	unsigned int max = 99999;
	int randNum = rand() % (max + 1); 
	char *numStr = malloc(sizeof(char) * 6);
	numStr[5] = '\0';

	sprintf(numStr, "%d", randNum);
  strcat(str, numStr);	

	// create a new dir named <onid>.movies.<rand_num>
	// with permissions set to 0750
	newDir = mkdir(str, 0750);

	while (newDir < 0)
	{
		perror("Error");
		strcpy(str, ONID);
	  randNum = rand() % max;
		sprintf(numStr, "%d", randNum);
		numStr[5] = '\0';
		strcat(str, numStr);	
	  newDir = mkdir(str, 0750);
	}

	//newDir = mkdir(str, 0750);	
	
	// print the name of the dir 
	printf("Created directory with name ");
	printf(" %s\n", str);

	// parse data to find out the movies released in each year. 
	// create a file named <year>.txt for each year in 
	// which >= 1 movie was released
	// and set each file permission to 0640
	_createUniqueYearFiles(str, list);

	// in each file, write the titles of every movie with 
	// the same year on a single line.
	
	// go back to the root dir
	chdir("..");
	free(numStr);
}

// autoprocess file name
void _autoProcessFile(char *str)
{
	FILE* movieFile;
	struct movie *list = 0;
	struct movie *sortedList = 0;
	struct movie *ref = 0;

	char filename[] = "";

	//printf("Enter the complete filename: ");
	//scanf("%s", filename);
	movieFile = fopen(str, "r");

	while (!movieFile)
	{
		perror("Error");	
		system("pwd");
		strcpy(filename, "");
		printf("Enter the complete filename: ");
		scanf("%s", filename);
		movieFile = fopen(filename, "r");
	}

	printf("Now processing the chosen file named %s\n", str);

	list = _processFile(movieFile);
	sortedList = _mergeSort(list);

	ref = sortedList;

	fclose(movieFile);

	_createDir(sortedList);

	sortedList = ref;
	// destroy the movies
	_destroyMovies(sortedList);
	//_destroyMovies(list);
}

void _findLargestFile()
{
	DIR* currDir = opendir(".");
  struct dirent *aDir;
	struct stat sb;
  int i,j = 0;
  char ext[5] = ".csv";
	char buf[5];
	char *strBuf = malloc(sizeof(char));

	unsigned long int largest = 0;


	while ((aDir = readdir(currDir)) != NULL)
  {

		// ignoring current
		if (!strcmp(aDir->d_name, "."))
		{
			continue;	
		}
		// ignore the parent
		if (!strcmp(aDir->d_name, ".."))
		{
			continue;	
		}

		// read the extension type for each file
		for (i = strlen(aDir->d_name) - strlen(POSTFIX); i <= strlen(aDir->d_name); i++)
    {
			buf[j] = aDir->d_name[i];
			j++;
    }

		j = 0;

		// find the files beginning with the PREFIX
		if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0)
		{	
			// find the size of the file
			if (strcmp(buf, ext) == 0)
			{
				stat(aDir->d_name, &sb);
				if (sb.st_size > largest)
				{
					largest = sb.st_size;
					free(strBuf);
					strBuf = malloc(sizeof(char) * strlen(aDir->d_name) + 1);
					strcpy(strBuf, aDir->d_name);
				}
			}
		}
		
	}	
	
	printf("The largest file is: %s %lu\n", strBuf, largest);

	_autoProcessFile(strBuf);

	closedir(currDir);
}

void _findSmallestFile()
{
	DIR* currDir = opendir(".");
  struct dirent *aDir;
	struct stat sb;
  int i,j = 0;
  char ext[5] = ".csv";
	char buf[5];
	char *strBuf = malloc(sizeof(char));

	unsigned long int smallest = 0;


	while ((aDir = readdir(currDir)) != NULL)
  {

		// ignoring current
		if (!strcmp(aDir->d_name, "."))
		{
			continue;	
		}
		// ignore the parent
		if (!strcmp(aDir->d_name, ".."))
		{
			continue;	
		}

		// read the extension type for each file
		for (i = strlen(aDir->d_name) - strlen(POSTFIX); i <= strlen(aDir->d_name); i++)
    {
			buf[j] = aDir->d_name[i];
			j++;
    }

		j = 0;

		// find the files beginning with the PREFIX
		if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0)
		{	
			// find the size of the file
			if (strcmp(buf, ext) == 0)
			{
				stat(aDir->d_name, &sb);

				// have to start somewhere
				if (smallest == 0)
				{
					smallest = sb.st_size;	
				}

				if (sb.st_size < smallest)
				{
					smallest = sb.st_size;
					free(strBuf);
					strBuf = malloc(sizeof(char) * strlen(aDir->d_name) + 1);
					strcpy(strBuf, aDir->d_name);
				}
			}
		}
		
	}	
	
	printf("The smallest file is: %s %lu\n", strBuf, smallest);

	_autoProcessFile(strBuf);

	closedir(currDir);
}


// asks for the name of the file.
// checks if file exists and writes err msg if the file 
// is not found. Repeats this process until a file is 
// found.
// input:
// output:
void _specifyFile()
{
	FILE* movieFile;
	struct movie *list = 0;
	struct movie *sortedList = 0;
	struct movie *ref = 0;

	char filename[] = "";

	printf("Enter the complete filename: ");
	scanf("%s", filename);
	movieFile = fopen(filename, "r");

	while (!movieFile)
	{
		perror("Error");	
		system("pwd");
		strcpy(filename, "");
		printf("Enter the complete filename: ");
		scanf("%s", filename);
		movieFile = fopen(filename, "r");
	}

	printf("Now processing the chosen file named %s\n", filename);

	list = _processFile(movieFile);
	sortedList = _mergeSort(list);
	ref = sortedList;

	fclose(movieFile);

	_createDir(sortedList);

	sortedList = ref;

	_destroyMovies(sortedList);
	//_destroyMovies(list);
}

void _selectFile()
{
	char str[2];
	char *str1 = NULL;
	int choice;
	
	printf("Which file do you want to process?\n");
	printf("Enter 1 to pick the largest file\n");
	printf("Enter 2 to pick the smallest file\n");
	printf("Enter 3 to specify the name of a file\n");
	printf("Enter a choice from 1 to 3: ");
	// only expect one char to be entered. no more, no less.
	scanf("%s", str);

	str1 = malloc(sizeof(char) * 2);
	strcpy(str1, str);

	choice = atoi(str1);

	while (str[1] != '\0' || (choice < 1 || choice > 3))
	{
		strcpy(str, "");
		choice = 0;

		printf("Not a valid choice, try again.\n");
		printf("Enter a choice between 1 and 3: ");
		scanf("%s", str);
		strcpy(str1, str);
	
		choice = atoi(str1);	
		
	}
	free(str1);

	switch (choice)
	{
		case 1: _findLargestFile(); break;
		case 2: _findSmallestFile(); break;
		case 3: _specifyFile(); break;
		default: break;
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
	
	printf("Enter a choice 1 or 2: ");
	scanf("%s", str);

	str1 = malloc(sizeof(char) * 2);
	strcpy(str1, str);

	choice = atoi(str1);

	while (str[1] != '\0' || (choice < 1 || choice > 2))
	{
		
		strcpy(str, "");
		choice = 0;

		printf("Not a valid choice, try again.\n");
		printf("Enter a choice 1 or 2: ");
		scanf("%s", str);
		strcpy(str1, str);
	
		choice = atoi(str1);	
		
	}

	free(str1);

	return choice;	
}

// calls the given function the corresponds to the menu
// choice selected by the user.
// input: an integer representing the movie menu choice
// output: the resulting output of the choices made.
void printMenuChoices(int val)
{
	switch(val)
	{
		case 1: _selectFile(); break;
		default: break;
	}
}