#include <stdlib.h>

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

struct movie* _mergeSort(struct movie*);
struct movie* _merge(struct movie*, struct movie*);
struct movie* _createMovie(char*);
struct movie* _processFile(const char*, int*);
struct movie* _createUniqueYearList(struct movie*);
int _findStr(char const*, char const*);
void _printLanguages(struct node*);
void _printMovie(struct movie*);
void _printByTitle(struct movie*);
void _printMovieList(struct movie*);
void printMovieMenu();
void _showByYear(struct movie*);
void _showByRating(struct movie*, struct movie*);
void _showByLanguage(struct movie*);
void printMenuChoices(int, struct movie*, struct movie*, int count);
int getMenuChoice();