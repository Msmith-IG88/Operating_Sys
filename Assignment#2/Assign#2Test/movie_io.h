#include <stdio.h>
#include "movie_algo.h"

// used for the list of languages for each movie
struct node
{
	char *val;
	struct node *next;
};

struct movie 
{
	char* title;
	unsigned int year;
	struct node *languages;
	float rating;
	struct movie *next;
};

// reads a line of input from a file and allocates
// memory for a movie struct
// input: a csv line
// output: a movie type containing the csv contents.
struct movie* _createMovie(char*);

// destroys movies
void _destroyMovies(struct movie*);

// creates a linked list of movies, using
// 	_createMovie as a helper function.
// input: an csv file
// output: a linked list (unsorted) of movies.
//struct movie* _processFile(char*);
struct movie* _processFile(FILE*);

// creates a list of unique years for use in the 
// _sortByRating function. Reduces the repeated 
// allocation of memory when option 2 is chosen.
void _createUniqueYearFiles(const char*, struct movie*);

// prints the menu instructions for the user
// to follow.
// input: stdin
// output: stdout
void printMovieMenu();

//void _createFilesByUniqueYear(const char*,struct movie*, struct movie*);
// creates a directory and parses the file(list) for all movies
// created each year. creates a file for each year a movie was
// released. Each file prints all the movies released in the 
// same year on the same line. 
void _createDir(struct movie*);

// auto processes the file
// input: the file to process
// output: the answer to life
void _autoProcessFile(char*); 

void _findLargestFile();

void _findSmallestFile();

void _specifyFile();

// the user is presented with 3 further choices about which
// file to process
// input: none 
// output: hmm
void _selectFile();

// calls the given function the corresponds to the menu
// choice selected by the user.
// input: an integer representing the movie menu choice
// output: the resulting output of the choices made.
void printMenuChoices(int);

// asks the user to enter a choice and gets the user input
// input: none
// output: an integer representing the valid user input.
int getMenuChoice();