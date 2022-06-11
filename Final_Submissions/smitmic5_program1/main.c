/*
 * Name: Michael Smith
 * Email: smitmic5@oregonstate.edu
 * Assignment#1(movies)
 * Descritption: This program opens and a sorts a .csv file provided by the user from 
 * the commandline. It then will display a menu of options for the user to use and view 
 * the movie data within the file.
 * 
 * References:
 * 		Some of my older code - https://github.com/Hippo-Jedi/DynamicArray-and-Linked-List
 * 													- https://github.com/Hippo-Jedi/Merge-Insert-Sort 
 * 		
 * 		Geeksforgeeks					- https://www.geeksforgeeks.org/merge-sort/ 
 * 		
 * 		Stackoverflow					- https://stackoverflow.com/questions/28189289/merge-sort-in-c
 * 													- 
 *    
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h" 

/*
 * 
This function opens the commandline arg file and reads movie data line by line while
counting the amount of movies there are in the data file. It uses the create_movie()
function to create each movies struct along the linked list.

Param:
	Input file
	Movie count pointer
 * 
 */
struct movie *create_movie_list(const char* file, int *n){
	struct movie* list = NULL;
	struct movie* tail = NULL;
	size_t l = 0;
	ssize_t line;
	char* current = NULL;

	FILE* sampleFile = fopen(file, "r");

	line = getline(&current, &l, sampleFile);

	while ((line = getline(&current, &l, sampleFile)) != -1){
		if (current[0] == '\n') {
			continue;
		} else {
			struct movie *new = create_movie(current);
			if (list ==NULL){
				list = new;
				tail = new;
			} else {
				tail->next = new;
				tail = new;
			}
			(*n)++;
		}
	}
	free(current);
	fclose(sampleFile);
	return list;
}

/*
 * 
This function recieves a single line from the data file and creates a movie struct
as part of the linked list. I also allocates memory to make space for the new link.

Param:
	string o characters from a single line
 * 
 */
struct movie* create_movie(char* line){
	struct movie *current = malloc(sizeof(struct movie));
	char *deli;
	char* temp = strtok_r(line, ",", &deli);
	
	current->title = calloc(strlen(temp) + 1, sizeof(char));
	strcpy(current->title, temp);

	temp = strtok_r(NULL, ",", &deli);
	current->year = atoi(temp);

	char *langDeli = deli;
	temp = strtok_r(NULL, ",", &deli);

	temp++;
	temp[strlen(temp) - 1] = '\0';
	char *tempPtr = temp;

	temp = strtok_r(NULL, "\n", &deli);
	current->rating = atof(temp);
	int count = 0;
	int i = 0;
	for (i; i < strlen(tempPtr); ++i){
		if (tempPtr[i] == ';'){
			count++;	
		}
	}

	temp = strtok_r(tempPtr, ";", &langDeli);
	struct link *lang = 0;
	lang = malloc(sizeof(struct link));
	lang->val = calloc(strlen(temp) + 1, sizeof(char));
	strcpy(lang->val, temp);

	struct link *langList = lang;
	struct link *tempLang = 0;

	int x = 0;
	while (x < count){
		tempLang = lang;
		temp = strtok_r(NULL, ";", &langDeli);
		lang = malloc(sizeof(struct link));
		tempLang->next = lang;
		lang->val = calloc(strlen(temp) + 1, sizeof(char));
		lang->next = 0;
		strcpy(lang->val, temp);
		x++;
	}
	current->languages = langList;
	current->next = NULL;
	
	return current;
}

/*
 * 
This is a simple function that just prints the menu options for the user and made it easier for error
handling.
 * 
 */
void menu(){
	printf("\n1. Show movies released in the specified year\n");
	printf("2. Show hightest rated movie for each year\n");
	printf("3. Show the title and year of release of all movies in a specific language\n");
	printf("4. Exit from the program\n\n");
}

/*
 * 
This function prompts the user to input the menu option they wish to use and then returns
that number.
 * 
 */
int input(){
	int choice;
	char str[2];
	char *ptr = NULL;
	ptr = malloc(sizeof(char));

	printf("Enter a choice from 1 to 4: ");
	scanf("%s", str);
	strcpy(ptr, str);
	choice = atoi(str);

	while (str[1] !='\0' || (choice>4 || choice<1)){
		strcpy(str, "");
		choice = 0;
		printf("You entered an incorrect choice. Try again.\n");
		menu();
		printf("Enter a choice from 1 to 4: ");
		scanf("%s", str);
		strcpy(ptr, str);
		choice = atoi(str);	
	}
	free(ptr);

	return choice;	
}

/*
 * 
This function is for option 1 of the menu and asks the user to input a specific year of movies they
want to see. It will then search the linked list for any movie structs that have the equivilant year
and then displays them.

Param:
	The linked list of movies
 * 
 */
void specific_year(struct movie *list){
	int input;	
	printf("Enter the year for which you want to see movies: ");
	scanf("%d", &input);

	int x = 0;
	x = input;
	int count = 0;
	while (list != NULL){
		if (list->year == x){
			printf("%s\n", list->title);
			count++;
		}
		list = list->next;
	}
	if (count == 0){
		printf("No data about movies released in the year %u\n", input);
	}
}

/*
 * 
This function is for option 3 of the menu and asks the user to input a language. It then calls the
check() function to find all movies that are in that language. It will then display those movies
to the user.

Param:
	The linked list of movies
 * 
 */
void specific_lang(struct movie *list){
	char input[10];
	int count = 0;
	struct link *temp = list->languages;
	printf("Enter the language for which you want to see movies: ");
	scanf("%s", input);

	while (list != 0){
		while (temp != 0){
			if (check(temp->val, input)){
				count++;	
				printf("%d %s\n", list->year, list->title);
			}
			temp = temp->next;
		}
		list = list->next;
		if (list != 0){
			temp = list->languages;
		}
	}
	if (count == 0){
		printf("No data about movies released in %s\n", input);
	}
}

/*
 * 
This function evaluates if the users inputed language is equal to a given movie structs language
array. It either returns 0 or 1 depending on if its a match.

Param:
	Value in movie structs lang array
	Users language input
 * 
 */
int check(char const *val, char const *inp){
	char *temp = strstr(val, inp);
	if (temp && strlen(val) == strlen(inp)){
		return 1;
	}else{
		return 0;
	}	
}

/*
 * 
This function if for option 2 of the menu and is the reason I incorporated a merge sort algorithm and sort
the movie data. This function compares to linked lists and finds all movies structs with the same year
and then displays the movie with the highest rating of that year

Param:
	The linked list of movies
	
 * 
 */
void highest_rated(struct movie *list, struct movie *newList) {
	while (newList != 0){
		if (list != 0){
			while (list->year == newList->year){
				if (list->rating > newList->rating){
					newList->title = list->title;
					newList->year = list->year;
					newList->rating = list->rating;
				}
				if (list->next != 0){
					list = list->next;
				} else {
					break;
				}
			}
		}
		printf("%u ", newList->year);
		printf("%.1f ",newList->rating);
		printf("%s",newList->title);
		printf("\n");
		newList = newList->next;
	}
}

/*
 * 
This function is also for option 2 of the menu. It creates a new linked list made up of movies structs with 
the same year. If a movie struct is the only movie with a specific year, its rating is automatically the highest
for that year. So this function creates a new list of only repeated years.

Param:
	The linked list of movies
 * 
 */
struct movie* year_list(struct movie* list){
	struct movie *temp = 0;
	struct movie *temp2 = 0;
	struct movie *new = 0;

	new = malloc(sizeof(struct movie));
	new->year = list->year;
	new->title = list->title;
	new->rating = list->rating;
	new->next = 0;
	temp = new;

	while (list != 0){
		if (new->year != list->year){
			temp2 = new;
			new = malloc(sizeof(struct movie));
			temp2->next = new;
			new->title = list->title;
			new->year = list->year;
			new->rating = list->rating;
			new->next = 0;
		}
		list = list->next;
	}

	return temp;
}

/*
 * 
This function is part of a merge-sort algorithm that sorts the list of movies. I used this because
it made creating option 2 much easier. This function in particular puts the two sorted halfs of the 
linked list back into one after being called from the sort() function.

Param:
		One half of the linked list
		Second half of the linked list
 * 
 */
struct movie *merge(struct movie *left, struct movie *right){
	struct movie *temp = malloc(sizeof(struct movie));
	struct movie *current = temp;
	struct movie *temp2 = temp;
	
	while (left != 0 && right != 0){	
		if (left->year < right->year){
			current->next = left;
			left = left->next;
		}else {
			current->next = right;
			right = right->next;
		}
		current = current->next;
	}
	if (left != 0){
		current->next = left;
		left = left->next;
	}
	if (right != 0){
		current->next = right;
		right = right->next;
	}	
	temp2 = temp;
	free(temp);

	return temp2->next;
}

/*
 * 
This function is part of a merge-sort algorithm that sorts the list of movies. I used this because
it made creating option 2 much easier. This function in particular splits the movies linked list into 
two linked lists and recursivley calls itself to sort the movies, then calls the merge() function to
put back together the sorted linked list. I decided to sort the movies by year so that finding the 
highest rated movie per year would be easier.

Param:
		The linked list of movies
 * 
 */
struct movie *sort(struct movie *list){
	struct movie *L1 = 0;
	struct movie *L2 = 0;
	struct movie *left = list;
	struct movie *right = list;
	struct movie *temp = list;

	if (list == 0 || list->next == 0){
		return list;
	}
	while (right != 0) {
		right = right->next;
		if (right != 0){
			temp = left;
			left = left->next;
			right = right->next;
		}
	}	

	temp->next = 0;
	L1 = sort(list);
	L2 = sort(left);
	
	return merge(L1, L2);
}

/*
 * 
This function is meant to free up the memory of after the program is finished. I have yet to finish this
function and I think that is the reason for any seg faults

Param:
		Sorted linked list
		List of duplicate years
 * 
 */
void list_free(struct movie *list1, struct movie *list2){
	struct movie *temp = 0;
	struct link *link1 = 0;
	struct link *link2	= 0;

	while (list1 != 0){
		temp = list1;
		free(temp->title);
		link1 = list1->languages;
		while (link1 != 0){
			link2 = link1;
			free(link2->val);		
			free(link2);
			link1 = link1->next;
		}
		free(temp);
		list1 = list1->next;
	}
	free(list1);

	while (list2 != 0){
		temp = list2;
		list2 = temp->next;
		free(temp);
	}
}

int main(int argc, char *argv[])
{
	struct movie *sorted = 0;
	struct movie *years = 0;

	int choice = 0;
	int count = 0;
	int *countP = &count;

	if (argc < 2){
		printf("Must provide a file.\n");
		return 1;
	}

	struct movie *list = create_movie_list(argv[1], countP);
	sorted = sort(list);
	years = year_list(sorted);

	printf("Processed file %s and parsed data for %u movies\n",
		argv[1], count);

	while (choice !=4 ){
		menu();
		choice = input();
		if (choice == 1){
		specific_year(sorted);
		} else if (choice == 2){
		highest_rated(sorted, years);
		} else if (choice == 3){
		specific_lang(sorted);
		} else if (choice == 4){
			break;
		}
	}

	printf("\n");
	list_free(sorted, years);

	return 0;
}