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
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define ARGLIM 512
#define LENGTH 2048

int bgToggle;
int sigtstpFlag;
int sigintFlag;

void input(char **command, int *args){
  char check[LENGTH];
  char *key;
  int l = strlen(check);

  while(check[0] == '#' || l < 1){
    printf(": ");
    fgets(check, LENGTH, stdin);
    strtok(check, "\n");
    l = strlen(check);
  }
  for (int i = 0; i < l; ++i){
    if (check[i] == '$' && check[i+1]== '$' && l > i + 1){
      char *temp = strdup(check);
      temp[i] = '%';
      temp[i+1] = 'd';
      sprintf(check, temp, getpid());
      free(temp);
    }
  }
  key = strtok(check, " ");
  while (key != NULL){
    command[*args] = strdup(key);
    *args = *args + 1;
    key = strtok(NULL, " ");
  }
  
}

void handle_SIGTSTP(int signo){
	if (bgToggle == 0){
		char* message = "\nEntering foreground-only mode (& is now ignored)\n";
		bgToggle = 1;
		write(STDOUT_FILENO, message, 50);
	}
	else{
		char* message = "\nExiting foreground-only mode\n";
		bgToggle = 0;
		write(STDOUT_FILENO, message, 30);
	}
	sigtstpFlag = 1;
	fflush(stdout);
}

void shell(char** argList, int *nArgs) {
	int bgFlag = 0;
	int status;
	int redirect;
	int FD;
	int i;
	pid_t pid = 0;
	struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0};

	//ignore sigint signals
	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);

	//custom SIGTSTP handler
	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;
  	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	pid - getpid();

		//check if last char is '&' and raise background flag and delete arg if true
		if (strcmp(argList[*nArgs - 1], "&") == 0) {
			(*nArgs)--;
			argList[*nArgs] = NULL;
			if (bgToggle == 0){
				bgFlag = 1;
				(*nArgs)++;
			}
		}

		//if exit command
		if (strcmp(argList[0], "exit") == 0) {
			fflush(stdout);
			while (1){
				exit(0);
			}
		} 

		//if change directory command
		else if (strcmp(argList[0], "cd") == 0) {
			if (*nArgs == 1)
				chdir(getenv("HOME"));
			else
				chdir(argList[1]);
		}

		//if status command
		else if (strcmp(argList[0], "status") == 0){
			if (WIFEXITED(status)){
				printf("exit value %d\n", WEXITSTATUS(status));
			}
			else if (WIFSIGNALED(status)){
				printf("terminated by signal %d\n", WTERMSIG(status));
			}
		}
		
		else{
			//for all other commands, spawn child
			pid = fork();
			switch (pid){
				case -1:
					perror("fork() failed\n");
					exit(1);
					break;
				case 0: //child stuff
					//custom sa handler for foreground
					if (bgFlag == 0){
						SIGINT_action.sa_handler = SIG_DFL;
						sigaction(SIGINT, &SIGINT_action, NULL);
					}
					
					for (i = 1; i < *nArgs; i++) {
						//if file not specified, pass io to /dev/null
						if (bgFlag == 1) {	
							redirect = 1;
							FD = open("/dev/null", O_RDONLY);
							int result = dup2(FD, STDIN_FILENO);
							if (result == -1) { 
								perror("source open()"); 
								exit(1); 
							}
						}
						// input file direction
						if ((strcmp(argList[i], "<") == 0)) {
							redirect = 1;
							
							FD = open(argList[i + 1], O_RDONLY);
							int result = dup2(FD, STDIN_FILENO);
							if (result == -1) { 
								perror("source open()"); 
								exit(1); 
							}
						}
						// output file direction
						if ((strcmp(argList[i], ">") == 0))  {
							redirect = 1;
							FD = open(argList[i + 1], O_CREAT | O_RDWR | O_TRUNC, 0644);
							int result = dup2(FD, STDOUT_FILENO); //should have made a function for all this repeated code 
							if (result == -1){
								perror("source open()"); 
								exit(1); 
							}
						}
					}
					//truncate arguments in preperation for exec and close FD
					if (redirect == 1){
						close(FD);
						for(i = 1; i < *nArgs; i++)
							argList[i] = NULL;
					}

					//pass trunceted arguments to execvp
					if (execvp(argList[0], argList) && sigtstpFlag != 1 && sigintFlag != 1){
						fprintf(stderr, "I do not understand %s\n", argList[0]);
						exit(1);
					}
					break;

				default: //parent stuff
					if (bgFlag == 1){
						printf("background pid is %d\n", pid);
					}
					else { 
						//wait for child to terminate
						waitpid(pid, &status, 0);
						if(sigtstpFlag != 1){
							//check that child is dead
							if (WIFSIGNALED(status) == 1 && WTERMSIG(status) != 0){
								printf("terminated by signal %d\n", WTERMSIG(status));
							}
							while (pid != -1){
								//re-fetch pid
								pid = waitpid(-1, &status, WNOHANG);	
								//print after results killed
								if (WIFEXITED(status) != 0 && pid > 0){
									printf("background pid %d is done: exit value %d\n", pid, WEXITSTATUS(status));
								}
								else if (WIFSIGNALED(status) != 0 && pid > 0 && bgToggle == 0){
									printf("background pid %d is done: terminated by signal %d\n", pid, WTERMSIG(status));
								}
							}
						}
					}
				
				break;
			}
		}

}

int main(){
  int args;
  char *command[ARGLIM];
  bgToggle = 0;
  
  while(1){
    args = 0;
    sigtstpFlag = 0;
    sigintFlag = 0;
    memset(command,'\0',ARGLIM);
    fflush(stdout);
    fflush(stdin);
    input(command, &args);
    shell(command, &args);
  }
}

