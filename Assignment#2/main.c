/*
 * Name: Michael Smith
 * Email: smitmic5@oregonstate.edu
 * Assignment#2(movies)
 * Descritption: This program reads an input directory and finds a file within that
 * directory based on the users initial inputs. From there, it reads the chosen file
 * and creates a new directory. It then creates new csv files within that new directory 
 * that correspond to the chosen file inputs. 
 * 
 * References:
 * 		Some of my older code - https://github.com/Hippo-Jedi/DynamicArray-and-Linked-List
 * 													- https://github.com/Hippo-Jedi/Merge-Insert-Sort 
 * 		
 * 		Geeksforgeeks					- https://www.geeksforgeeks.org/merge-sort/ 
 * 		
 * 		Stackoverflow					- https://stackoverflow.com/questions/28189289/merge-sort-in-c
 * 													
 *    
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "header.h" 

//#define ".csv" ".csv"
//#define PREFIX ""movies_""
//#define ONID "smitmic5.movies."
/*
 * 
This function opens the commandline arg file and reads movie data line by line while
counting the amount of movies there are in the data file. It uses the create_movie()
function to create each movies struct along the linked list.

Param
	Input file
	Movie count pointer
 * 
 */
struct movie *create_movie_list(FILE* file){
	struct movie* list = NULL;
	struct movie* tail = NULL;
	size_t l = 0;
	ssize_t line;
	char* current = NULL;

	//FILE* sampleFile = fopen(file, "r");

	line = getline(&current, &l, file);

	while ((line = getline(&current, &l, file)) != -1){
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
			//(*n)++;
		}
	}
	free(current);
	//fclose(sampleFile);
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

	for (int i; i < strlen(tempPtr); ++i){
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
	printf("\n1. Select the file to process\n");
	printf("2. Exit the program\n");
}

void fileMenu(){
	int choice;
	char str[2];
	char *ptr = NULL;
	ptr = malloc(sizeof(char)*2);

	printf("Which file do you want to process?\n");
	printf("Enter 1 to pick the largest file\n");
	printf("Enter 2 to pick the smallest file\n");
	printf("Enter 3 to specify the name of a file\n");
	printf("Enter a choice from 1 to 3: ");
	scanf("%s", str);
	strcpy(ptr, str);
	choice = atoi(ptr);

	while (str[1] !='\0' || (choice>3 || choice<1)){
		strcpy(str, "");
		choice = 0;
		printf("You entered an incorrect choice. Try again.\n");
		printf("Which file do you want to process?\n");
		printf("Enter 1 to pick the largest file\n");
		printf("Enter 2 to pick the smallest file\n");
		printf("Enter 3 to specify the name of a file\n");
		printf("Enter a choice from 1 to 3: ");
		printf("Enter a choice from 1 to 3: ");
		scanf("%s", str);
		strcpy(ptr, str);
		choice = atoi(str);	
	}
	free(ptr);

	switch (choice){
		case 1: largestFile(); break;
		case 2: smallestFile(); break;
		case 3: specificFile(); break;
		default: break;
	}
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
	ptr = malloc(sizeof(char)*2);

	printf("Enter a choice from 1 to 2: ");
	scanf("%s", str);
	strcpy(ptr, str);
	choice = atoi(ptr);

	while (str[1] !='\0' || (choice>2 || choice<1)){
		strcpy(str, "");
		choice = 0;
		printf("You entered an incorrect choice. Try again.\n");
		printf("\n1. Select the file to process\n");
		printf("2. Exit the program\n");
		printf("Enter a choice from 1 to 2: ");
		scanf("%s", str);
		strcpy(ptr, str);
		choice = atoi(str);	
	}
	free(ptr);

	return choice;	
}

void largestFile(){
	DIR* directory = opendir(".");
  char fileEX[5] = ".csv";
	char *ptr = malloc(sizeof(char));
	struct dirent *stDir = readdir(directory);
	struct stat buf;

	unsigned long int largest = 0;
	int count = 0;
	char buffer[5];

	while (stDir != NULL){
		if (!strcmp(stDir->d_name, ".")){
			continue;	
		}
		if (!strcmp(stDir->d_name, "..")){
			continue;	
		}
		for (int i = strlen(stDir->d_name) - strlen(".csv"); i <= strlen(stDir->d_name); i++)
    {
			buffer[count] = stDir->d_name[i];
			count++;
    }
		count = 0;

		if (strncmp("movies_", stDir->d_name, strlen("movies_")) == 0){	
			if (strcmp(buffer, fileEX) == 0){
				stat(stDir->d_name, &buf);
				if (buf.st_size > largest){
					largest = buf.st_size;
					free(ptr);
					ptr = malloc(sizeof(char) * strlen(stDir->d_name) + 1);
					strcpy(ptr, stDir->d_name);
				}
			}
		}
	}	
	
	printf("The largest file is: %s %lu\n", ptr, largest);
	processFile(ptr);
	closedir(directory);
}

void smallestFile(){
	DIR* directory = opendir(".");
  char fileEX[5] = ".csv";
	char *ptr = malloc(sizeof(char));
	struct dirent *stDir = readdir(directory);
	struct stat buf;

	unsigned long int smallest = 0;
	int count = 0;
	char buffer[5];

	while (stDir != NULL){
		if (!strcmp(stDir->d_name, ".")){
			continue;	
		}
		if (!strcmp(stDir->d_name, "..")){
			continue;	
		}
		for (int i = strlen(stDir->d_name) - strlen(".csv"); i <= strlen(stDir->d_name); i++){
			buffer[count] = stDir->d_name[i];
			count++;
    }
		count = 0;

		if (strncmp("movies_", stDir->d_name, strlen("movies_")) == 0){	
			if (strcmp(buffer, fileEX) == 0){
				stat(stDir->d_name, &buf);
				if (smallest == 0){
					smallest = buf.st_size;	
				}
				if (buf.st_size < smallest){
					smallest = buf.st_size;
					free(ptr);
					ptr = malloc(sizeof(char) * strlen(stDir->d_name) + 1);
					strcpy(ptr, stDir->d_name);
				}
			}
		}
	}	

	printf("The smallest file is: %s %lu\n", ptr, smallest);
	processFile(ptr);
	closedir(directory);
}

void specificFile(){
	char filename[] = "";
	printf("Enter the complete filename: ");
	scanf("%s", filename);
	FILE* file = fopen(filename, "r");

	while (!file){
		perror("Error");	
		system("pwd");
		strcpy(filename, "");
		printf("Enter the complete filename: ");
		scanf("%s", filename);
		file = fopen(filename, "r");
	}
	printf("Now processing the chosen file named %s\n", filename);

	struct movie *list = create_movie_list(file);
	struct movie *sorted = sort(list);
	struct movie *temp = sorted;

	fclose(file);
	createDirectory(sorted);
	sorted = temp;
	list_free(sorted);
}

void processFile(char *dirName){
	char filename[] = "";
	FILE* file = fopen(dirName, "r");

	while (!file){
		perror("Error");	
		system("pwd");
		strcpy(filename, "");
		printf("Enter the complete filename: ");
		scanf("%s", filename);
		file = fopen(filename, "r");
	}
	printf("Now processing the chosen file named %s\n", dirName);

	struct movie *list = create_movie_list(file);
	struct movie *sorted = sort(list);

	struct movie *temp = sorted;
	fclose(file);
	createDirectory(sorted);
	sorted = temp;
	list_free(sorted);
}

void createDirectory(struct movie *list){
	char str[] = "smitmic5.movies.";
	unsigned int max = 99999;
	srand(time(0));
	int random = rand() % (max + 1); 
	char *num = malloc(sizeof(char) * 6);
	num[5] = '\0';

	sprintf(num, "%d", random);
  strcat(str, num);	

	int new = mkdir(str, 0750);

	while (new < 0){
		perror("Error");
		strcpy(str, "smitmic5.movies.");
	  random = rand() % max;
		sprintf(num, "%d", random);
		num[5] = '\0';
		strcat(str, num);	
	  new = mkdir(str, 0750);
	}
	printf("Created directory with name ");
	printf(" %s\n", str);

	createYearFile(str, list);
	chdir("..");
	free(num);
}

void createYearFile(const char *dir, struct movie* list){
	unsigned int current = list->year;
	struct movie *temp = list;
	char cstr[9]; 
	int new = chdir(dir);

	if (new < 0){
		perror("Error\n");
		exit(EXIT_FAILURE);
	}

	while (list != 0){
		if (current != list->year){
			current = list->year;
			sprintf(cstr, "%d", list->year);
			strcat(cstr, ".txt");
   		FILE *file = fopen(cstr, "a+");
			if (list != 0){
				while (current == list->year){
					fputs(list->title, file);
					fputs("\n", file);
					if (chmod(cstr, 0640) < 0){
						perror("error changing file permissions\n");
						break;
					}
					if (list->next != 0){
						list = list->next;
					} else {
						break;
					}
				}
			}
		}
		list = list->next;
	}

	list = temp;
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
void list_free(struct movie *list1){
	struct movie *temp = 0;
	while (list1 != 0){
		temp = list1;
		free(temp->title);
		free(temp);
		list1 = list1->next;
	}
	free(list1);
}

int main(int argc, char *argv[])
{
	//struct movie *sorted = 0;
	//struct movie *years = 0;

	int choice = 0;
	//int count = 0;
	//int *countP = &count;
	while (choice != 2){
		printf("\n1. Select the file to process\n");
		printf("2. Exit the program\n");
		choice = input();
		if (choice == 1){
			fileMenu();
		}
	}
	return 0;
}