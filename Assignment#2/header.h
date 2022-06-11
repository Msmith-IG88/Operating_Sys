#include <stdlib.h>

struct link{
	char *val;
	struct link *next;
};

struct movie {
	char* title;
	unsigned int year;
	struct link *languages;
	float rating;
	struct movie *next;
};

struct movie* create_movie_list(FILE*);
struct movie* create_movie(char*);
struct movie* year_list(struct movie*);
struct movie* sort(struct movie*);
struct movie* merge(struct movie*, struct movie*);
void menu();
void fileMenu();
void largestFile();
void smallestFile();
void specificFile();
void processFile(char*);
void createYearFile(const char*, struct movie*);
void createDirectory(struct movie *list);
int input();
void specific_year(struct movie*);
void specific_lang(struct movie*);
int check(char const*, char const*);
void highest_rated(struct movie*, struct movie*);
void list_free(struct movie*);