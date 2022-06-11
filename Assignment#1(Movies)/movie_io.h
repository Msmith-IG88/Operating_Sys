#include "movie_algo.h"

// used for the list of languages for each movie
struct node
{
	char *val;
	struct node *next;
};

// da move eeeee
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

// creates a linked list of movies, using
// 	_createMovie as a helper function.
// input: an csv file and an address reference to 
// 	the number of movies read from the file.
// output: a linked list (unsorted) of movies.
struct movie* _processFile(const char*, int*);

// creates a list of unique years for use in the 
// _sortByRating function. Reduces the repeated 
// allocation of memory when option 2 is chosen.
struct movie* _createUniqueYearList(struct movie*);

// finds a string within a linked list of lang nodes.
// input: the string being searched for 
// 	and the string being searched.
// output: int representing true or false.
int _findStr(char const*, char const*);

// prints the list of languages for that movie
// input: a list of languages 
// output: stdout
void _printLanguages(struct node*);

// prints a node in the linked list of movies
// input: a linked list of movies
// output: stdout
void _printMovie(struct movie*);

// prints the linked list of movies by title only
// input: a linked list of movies, title only
// output: stdout
void _printByTitle(struct movie*);

// prints the entire linked list of movies,
// using _printMovie as a helper function.
// input: a linked list of movies
// output: stdout
void _printMovieList(struct movie*);

// prints the menu instructions for the user
// to follow.
// input: stdin
// output: stdout
void printMovieMenu();

// when movie menu option 1 is chosen, the user is
// asked what year they would like to see movies for.
// input: a linked list of movies
// output: a list of movies created that match the 
// 	year the user enters to stdout.
void _showByYear(struct movie*);

// when movie menu option 2 is chosen, the user does
// not input any values. Instead, a list of unique years
// is created and destroyed. Movies with the same year 
// have their rating compared and swapped using the greater
// of the two. 
// input: a linked list of movies
// output: stdout of each year that has a movie with the
// highest rating.
void _showByRating(struct movie*, struct movie*);

// when movie menu option 3 is chosen, asks the user to enter
// a language that they would like to search the movie list for.
// uses _findStr helper function to search the languages field of the 
// movie, exact matches only. When a movie is with that language is
// found, prints that movie to stdout.
// input: a linked list of movies
// output: stdout of movies that are in the language entered by the user.
void _showByLanguage(struct movie*);

// calls the given function the corresponds to the menu
// choice selected by the user.
// input: an integer representing the movie menu choice and
// 	a linked list of movies.
// output: the resulting output of the choices made.
void printMenuChoices(int, struct movie*, struct movie*);

// asks the user to enter a choice and gets the user input
// input: none
// output: an integer representing the valid user input.
int getMenuChoice();
