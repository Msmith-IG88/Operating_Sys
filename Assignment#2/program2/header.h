/*
	This file contains all struct and function declarations
*/
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
struct movie* sort(struct movie*);
struct movie* merge(struct movie*, struct movie*);
void file_menu();
void largest_file();
void smallest_file();
void specific_file();
void process_file(char*);
void create_year_file(const char*, struct movie*);
void create_directory(struct movie *list);
int input();
void list_free(struct movie*);