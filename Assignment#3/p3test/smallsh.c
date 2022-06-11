/*
 * Name: Michael Smith
 * Email: smitmic5@oregonstate.edu
 * Assignment#3
 * Descritption: This program creates your own shell called smallsh with both unique and custom
 * command sets. The shell prompts the user for running commands, handles blank lines and comments
 * and executes 3 custom commands(exit, cd, and status). creates new process when exectuing other
 * commands and supports input/output redirection. 
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
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define CMD_LENGTH 2048
#define MAX_CMD 512
#define FILE_SIZE 300

int bg_flag = 1;
int bg_toggle;
int tstpFlag;

void get_commands(char*[], int*, char[], char[], int);
void execute(char*[], int*, struct sigaction, int*, char[], char[]);
void SIG_TSTP_HANDLE(int);

void get_commands(char* arr[], int* bg_toggle, char inputName[], char outputName[], int pid) {
	char input[CMD_LENGTH];

	printf(": ");
	fflush(stdout);
	fgets(input, CMD_LENGTH, stdin);

	int found = 0;
	for (int i = 0; !found && i<CMD_LENGTH; i++) {
		if (input[i] == '\n') {
			input[i] = '\0';
			found = 1;
		}
	}
	if (!strcmp(input, "")) {
		arr[0] = strdup("");
		return;
	}
	const char space[2] = " ";
	char *token = strtok(input, space);

	for (int i = 0; token; i++) {
		if (!strcmp(token, "&")) {
			*bg_toggle = 1;
		}else if (!strcmp(token, "<")) {
			token = strtok(NULL, space);
			strcpy(inputName, token);
		}else if (!strcmp(token, ">")) {
			token = strtok(NULL, space);
			strcpy(outputName, token);
		}
		else {
			arr[i] = strdup(token);
			for (int j = 0; arr[i][j]; j++) {
				if (arr[i][j] == '$' &&
					 arr[i][j+1] == '$') {
					arr[i][j] = '\0';
					snprintf(arr[i], 256, "%s%d", arr[i], pid);
				}
			}
		}
		token = strtok(NULL, space);
	}
}

void execute(char* arr[], int* exitStatus, struct sigaction sa, int* bg_toggle, char inputName[], char outputName[]) {
	pid_t childPID = 0;
	int input;
	int output;
	int wc_result;
	childPID = fork();
	switch (childPID){
		case 0:
			sa.sa_handler = SIG_DFL;
			sigaction(SIGINT, &sa, NULL);
			if (strcmp(inputName, "")) {
				input = open(inputName, O_RDONLY);
				if (input == -1) {
					printf("cannot open %s for input\n", inputName);
					exit(1);
				}
				wc_result = dup2(input, 0);
				fcntl(input, F_SETFD, FD_CLOEXEC);
			}
			if (strcmp(outputName, "")) {
				output = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (output == -1) {
					printf("cannot open %s for output\n", outputName);
					exit(1);
				}
				wc_result = dup2(output, 1);
				fcntl(output, F_SETFD, FD_CLOEXEC);
			}
			if (execvp(arr[0], arr) && tstpFlag !=1) {
				printf("%s: no such file or directory\n", arr[0]);
				exit(1);
			}
			break;
		default:
			if (*bg_toggle && bg_flag) {
				pid_t tempPID = waitpid(childPID, exitStatus, WNOHANG);
				printf("background pid is %d\n", childPID);
				fflush(stdout);
			}
			else {
				pid_t tempPID = waitpid(childPID, exitStatus, 0);
			}
		while ((childPID = waitpid(-1, exitStatus, WNOHANG)) > 0) {
			printf("background pid %d is done: ", childPID);
			if(WIFEXITED(*exitStatus)){
        printf("exit value %d\n", WEXITSTATUS(*exitStatus));
      }else {
        printf("terminated by signal %d\n", WTERMSIG(*exitStatus));
      }
			fflush(stdout);
		}
	}
}

void SIG_TSTP_HANDLE(int signo) {
	if (bg_flag == 1) {
		char* msg1 = "Entering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, msg1, 50);
		bg_flag = 0;
	}
	else {
		char* msg2 = "Exiting foreground-only mode\n";
		write (STDOUT_FILENO, msg2, 30);
		bg_flag = 1;
	}
	tstpFlag = 1;
	fflush(stdout);
}

void SIG_INT_HANDLE(int signo){
	char* msg = "Caught SIGINT, sleeping for 10 seconds\n";
	write(STDOUT_FILENO, msg, 40);
	sleep(10);
}

int main(){
	int exitStatus = 0;
	char* input[MAX_CMD];
  int pid = getpid();
	char inFILE[FILE_SIZE] = "";
	char outFILE[FILE_SIZE] = "";
  struct sigaction intHandler = {{0}}, tstpHandler = {{0}};

	tstpHandler.sa_handler = SIG_TSTP_HANDLE;
	sigfillset(&tstpHandler.sa_mask);
	tstpHandler.sa_flags = 0;
	sigaction(SIGTSTP, &tstpHandler, NULL);

	intHandler.sa_handler = SIG_INT_HANDLE;
	sigfillset(&intHandler.sa_mask);
	intHandler.sa_flags = 0;
	sigaction(SIGINT, &intHandler, NULL);

	bg_toggle = 0;
	for (int i = 0; i < MAX_CMD; i++) {
		input[i] = NULL;
	}
	while (1){
		get_commands(input, &bg_toggle, inFILE, outFILE, pid);
    if (strcmp(input[0], "exit") == 0) {
			break;
		}else if (input[0][0] == '\0' || input[0][0] == '#') {
			continue;
		}else if (strcmp(input[0], "cd") == 0) {
			if (input[1]) {
				if (chdir(input[1]) == -1) {
					printf("Directory not found.\n");
					fflush(stdout);
				}
			}else {
				chdir(getenv("HOME"));
			}
		}else if (strcmp(input[0], "status") == 0) {
      if(WIFEXITED(exitStatus)){
        printf("exit value %d\n", WEXITSTATUS(exitStatus));
      }else {
        printf("terminated by signal %d\n", WTERMSIG(exitStatus));
      }
		}else {
			execute(input, &exitStatus, intHandler, &bg_toggle, inFILE, outFILE);
		}
		for (int j = 0; input[j]; j++){
			input[j] = NULL;
		}
		bg_toggle = 0;
		inFILE[0] = '\0';
		outFILE[0] = '\0';
	}
	return 0;
}