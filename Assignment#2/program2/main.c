/*
 * Name: Michael Smith
 * Email: smitmic5@oregonstate.edu
 * Assignment#2
 * Descritption: This program reads an input directory and finds a file within that
 * directory based on the users initial inputs. From there, it reads the chosen file
 * and creates a new directory. It then creates new txt files within that new directory for each year a movie struct has within the linked list.
 * that correspond to the chosen file inputs. 
 * 
 * References:
 * 		Some of my older code - https://github.com/Hippo-Jedi/DynamicArray-and-Linked-List
 * 													- https://github.com/Hippo-Jedi/Merge-Insert-Sort 
 * 		
 * 		Geeksforgeeks					- https://www.geeksforgeeks.org/merge-sort/ 
 * 
 * 													- https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/ 
 * 		
 * 		Stackoverflow					- https://stackoverflow.com/questions/28189289/merge-sort-in-c
 * 													- https://stackoverflow.com/questions/25070751/navigating-through-files-using-dirent-h 
 * 														
 * 		Others								- https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html 
 * 
 * 													- https://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/stat.h.html
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
#include <time.h>
#include "header.h" 

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
This is a simple function that just prints the menu options for the user and then utilizes a switch function depending on the users inputs.
 * 
 */
void file_menu(){
	int choice;
	char str[2];
	char *ptr = NULL;
	ptr = malloc(sizeof(char)*2);

	printf("\nWhich file you want to process?\n");
	printf("Enter 1 to pick the largest file\n");
	printf("Enter 2 to pick the smallest file\n");
	printf("Enter 3 to specify the name of a file\n");
	printf("\nEnter a choice from 1 to 3: ");
	scanf("%s", str);
	strcpy(ptr, str);
	choice = atoi(ptr);

	while (str[1] !='\0' || (choice>3 || choice<1)){
		strcpy(str, "");
		choice = 0;
		printf("Invalid choice. Try again\n");
		printf("\nWhich file you want to process?\n");
		printf("Enter 1 to pick the largest file\n");
		printf("Enter 2 to pick the smallest file\n");
		printf("Enter 3 to specify the name of a file\n");
		printf("\nEnter a choice from 1 to 3: ");
		scanf("%s", str);
		strcpy(ptr, str);
		choice = atoi(str);	
	}
	free(ptr);

	switch (choice){
		case 1: largest_file(); break;
		case 2: smallest_file(); break;
		case 3: specific_file(); break;
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

	printf("\nEnter a choice from 1 or 2: ");
	scanf("%s", str);
	strcpy(ptr, str);
	choice = atoi(ptr);

	while (str[1] !='\0' || (choice>2 || choice<1)){
		strcpy(str, "");
		choice = 0;
		printf("Invalid choice. Try again\n");
		printf("\n1. Select the file to process\n");
		printf("2. Exit the program\n");
		printf("Enter a choice from 1 or 2: ");
		scanf("%s", str);
		strcpy(ptr, str);
		choice = atoi(str);	
	}
	free(ptr);

	return choice;	
}

/*
 * 
This function is for option 1 of the second menu and is used to find the largest .csv file beginging with movies_. The function opens the directory and sifts through it to compare the .csv file sizes. It keeps track of the file size and then calls the process_file() function with the largest file as it parameter.

Param
	none
 * 
 */
void largest_file(){
	DIR* directory = opendir(".");
  char fileEX[5] = ".csv";
	char *ptr = malloc(sizeof(char));
	struct dirent *stDir;
	struct stat buf;

	unsigned long int largest = 0;
	int count = 0;
	char buffer[5];

	while ((stDir = readdir(directory)) != NULL){
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
	
	process_file(ptr);
	closedir(directory);
}

/*
 * 
This function is for option 2 of the second menu and is used to find the smallest .csv file beginging with movies_. The function opens the directory and sifts through it to compare the .csv file sizes. It keeps track of the file size and then calls the process_file() function with the smallest file as it parameter.

Param
	none
 * 
 */
void smallest_file(){
	DIR* directory = opendir(".");
  char fileEX[5] = ".csv";
	char *ptr = malloc(sizeof(char));
	struct dirent *stDir;
	struct stat buf;

	unsigned long int smallest = 0;
	int count = 0;
	char buffer[5];

	while ((stDir = readdir(directory)) != NULL){
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
	process_file(ptr);
	closedir(directory);
}

/*
 * 
This function is for option 3 of the second menu and is used find a specific file the user has entered. It checks to see if the file actually exists and if so, processes it and creates a directory.

Param
	none
 * 
 */
void specific_file(){
	char filename[] = "";
	printf("Enter the complete filename: ");
	scanf("%s", filename);
	FILE* file = fopen(filename, "r");

	if(!file){
		printf("The file %s was not found. Try again\n", filename);
		file_menu();
	} else {
		printf("Now processing the chosen file named %s\n", filename);

		struct movie *list = create_movie_list(file);
		struct movie *sorted = sort(list);
		struct movie *temp = sorted;

		fclose(file);
		create_directory(sorted);
		sorted = temp;
		list_free(sorted);
	}
}

/*
 * 
This function is used for when options 1 & 2 are entered on the second menu. This function takes in a directory/file name and calls the create_movie_list() function from assignment#1 to create a linked list off all the movies within the entered file. It then creates a directory by calling createDirecty().

Param
	File/Directory path
 * 
 */
void process_file(char *dirName){
	char filename[] = "";
	FILE* file = fopen(dirName, "r");

	if(!file){
		printf("The file %s was not found. Try again\n", filename);
		file_menu();
	} else {
		printf("Now processing the chosen file named %s\n", dirName);

		struct movie *list = create_movie_list(file);
		struct movie *sorted = sort(list);
		struct movie *temp = sorted;

		fclose(file);
		create_directory(sorted);
		sorted = temp;
		list_free(sorted);
	}
}

/*
 * 
This function creates and names a new directory for the movie list. It utilizes the random library to randomly generate a number to be used for the directory name. It also adjusts the permissions of the new directory according to the assignment.
It then calls the create_year_files() functions.

Param
	Sorted linked list of movies
 * 
 */
void create_directory(struct movie *list){
	srand(time(0));
	char onid[] = "smitmic5.movies.";
	unsigned int max = 99999;
	int random = rand() % (max + 1); 
	char *num = malloc(sizeof(char) * 6);
	num[5] = '\0';

	sprintf(num, "%d", random);
  strcat(onid, num);	

	int new = mkdir(onid, 0750);

	while (new < 0){
		perror("Error");
		strcpy(onid, "smitmic5.movies.");
	  random = rand() % max;
		sprintf(num, "%d", random);
		num[5] = '\0';
		strcat(onid, num);	
	  new = mkdir(onid, 0750);
	}
	printf("Created directory with name ");
	printf(" %s\n", onid);

	create_year_file(onid, list);
	chdir("..");
	free(num);
}

/*
 * 
This function creates txt files for each corresponding year within the linked list of movies to fill the new directory. It locates movies with the same year and then puts them inside a txt file for that year. It also adjusts the txt file permissions according to the assignment guidelines.

Param
	file/directory
	sorted linked list
 * 
 */
void create_year_file(const char *dir, struct movie* list){
	unsigned int current = list->year;
	struct movie *temp = list;
	char cstr[9]; 
	int new = chdir(dir);
	FILE *file = 0;

	while (list != 0){
		if (current != list->year){
			current = list->year;
			sprintf(cstr, "%d", list->year);
			strcat(cstr, ".txt");
   		file = fopen(cstr, "a+");
			if (list != 0){
				while (current == list->year){
					fputs(list->title, file);
					fputs("\n", file);
					if (chmod(cstr, 0640) < 0){
						perror("permision error\n");
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
This function is part of a merge-sort algorithm that sorts the list of movies. This function in particular puts the two sorted halfs of the 
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
This function is part of a merge-sort algorithm that sorts the list of movies. This function in particular splits the movies linked list into 
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
This function is meant to free up the memory of after the program is finished.

Param:
		Sorted linked list
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
			file_menu();
		}
	}
	return 0;
}