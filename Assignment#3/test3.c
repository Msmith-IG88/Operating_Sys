#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define CMD_LENGTH 2048
#define MAX_CMD 512

int allowBackground = 1;

void getInput(char*[], int*, char[], char[], int);
void execCmd(char*[], int*, struct sigaction, int*, char[], char[]);
void catchSIGTSTP(int);
void printExitStatus(int);

void getInput(char* arr[], int* background, char inputName[], char outputName[], int pid) {
	
	char input[CMD_LENGTH];
	int i, j;

	// Get input
	printf(": ");
	fflush(stdout);
	fgets(input, CMD_LENGTH, stdin);

	// Remove newline
	int found = 0;
	for (i=0; !found && i<CMD_LENGTH; i++) {
		if (input[i] == '\n') {
			input[i] = '\0';
			found = 1;
		}
	}

	// If it's blank, return blank
	if (!strcmp(input, "")) {
		arr[0] = strdup("");
		return;
	}

	// Translate rawInput into individual strings
	const char space[2] = " ";
	char *token = strtok(input, space);

	for (i=0; token; i++) {
		// Check for & to be a background process
		if (!strcmp(token, "&")) {
			*background = 1;
		}
		// Check for < to denote input file
		else if (!strcmp(token, "<")) {
			token = strtok(NULL, space);
			strcpy(inputName, token);
		}
		// Check for > to denote output file
		else if (!strcmp(token, ">")) {
			token = strtok(NULL, space);
			strcpy(outputName, token);
		}
		// Otherwise, it's part of the command!
		else {
			arr[i] = strdup(token);
			// Replace $$ with pid
			// Only occurs at end of string in testscirpt
			for (j=0; arr[i][j]; j++) {
				if (arr[i][j] == '$' &&
					 arr[i][j+1] == '$') {
					arr[i][j] = '\0';
					snprintf(arr[i], 256, "%s%d", arr[i], pid);
				}
			}
		}
		// Next!
		token = strtok(NULL, space);
	}
}

void execCmd(char* arr[], int* childExitStatus, struct sigaction sa, int* background, char inputName[], char outputName[]) {
	
	int input, output, result;
	pid_t spawnPid = -5;

	// Fork that shit
	// The structure of this is from Lecture 3.1, not much changed
	spawnPid = fork();
	switch (spawnPid) {
		
		case -1:	;
			perror("Hull Breach!\n");
			exit(1);
			break;
		
		case 0:	;
			// The process will now take ^C as default
			sa.sa_handler = SIG_DFL;
			sigaction(SIGINT, &sa, NULL);

			// Handle input, code is basically straight from Lecture 3.4
			if (strcmp(inputName, "")) {
				// open it
				input = open(inputName, O_RDONLY);
				if (input == -1) {
					perror("Unable to open input file\n");
					exit(1);
				}
				// assign it
				result = dup2(input, 0);
				if (result == -1) {
					perror("Unable to assign input file\n");
					exit(2);
				}
				// trigger its close
				fcntl(input, F_SETFD, FD_CLOEXEC);
			}

			// Handle output, code is basically straight from Lecture 3.4
			if (strcmp(outputName, "")) {
				// open it
				output = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (output == -1) {
					perror("Unable to open output file\n");
					exit(1);
				}
				// assign it
				result = dup2(output, 1);
				if (result == -1) {
					perror("Unable to assign output file\n");
					exit(2);
				}
				// trigger its close
				fcntl(output, F_SETFD, FD_CLOEXEC);
			}
			
			// Execute it!
			if (execvp(arr[0], (char* const*)arr)) {
				printf("%s: no such file or directory\n", arr[0]);
				fflush(stdout);
				exit(2);
			}
			break;
		
		default:	;
			// Execute a process in the background ONLY if allowBackground
			if (*background && allowBackground) {
				pid_t actualPid = waitpid(spawnPid, childExitStatus, WNOHANG);
				printf("background pid is %d\n", spawnPid);
				fflush(stdout);
			}
			// Otherwise execute it like normal
			else {
				pid_t actualPid = waitpid(spawnPid, childExitStatus, 0);
			}

		// Check for terminated background processes!	
		while ((spawnPid = waitpid(-1, childExitStatus, WNOHANG)) > 0) {
			printf("child %d terminated\n", spawnPid);
			printExitStatus(*childExitStatus);
			fflush(stdout);
		}
	}
}

void catchSIGTSTP(int signo) {

	// If it's 1, set it to 0 and display a message reentrantly
	if (allowBackground == 1) {
		char* message = "Entering foreground-only mode (& is now ignored)\n";
		write(1, message, 49);
		fflush(stdout);
		allowBackground = 0;
	}

	// If it's 0, set it to 1 and display a message reentrantly
	else {
		char* message = "Exiting foreground-only mode\n";
		write (1, message, 29);
		fflush(stdout);
		allowBackground = 1;
	}
}

void printExitStatus(int childExitMethod) {
	
	if (WIFEXITED(childExitMethod)) {
		// If exited by status
		printf("exit value %d\n", WEXITSTATUS(childExitMethod));
	} else {
		// If terminated by signal
		printf("terminated by signal %d\n", WTERMSIG(childExitMethod));
	}
}

int main(){

	int pid = getpid();
	int cont = 1;
	int i;
	int exitStatus = 0;
	int background = 0;

	char inputFile[256] = "";
	char outputFile[256] = "";
	char* input[MAX_CMD];
	for (i=0; i<MAX_CMD; i++) {
		input[i] = NULL;
	}

	// Signal Handlers
	
	// Ignore ^C
	struct sigaction sa_sigint = {{0}};
	sa_sigint.sa_handler = SIG_IGN;
	sigfillset(&sa_sigint.sa_mask);
	sa_sigint.sa_flags = 0;
	sigaction(SIGINT, &sa_sigint, NULL);

	// Redirect ^Z to catchSIGTSTP()
	struct sigaction sa_sigtstp = {{0}};
	sa_sigtstp.sa_handler = catchSIGTSTP;
	sigfillset(&sa_sigtstp.sa_mask);
	sa_sigtstp.sa_flags = 0;
	sigaction(SIGTSTP, &sa_sigtstp, NULL);

	while (cont){
		// Get input
		getInput(input, &background, inputFile, outputFile, pid);

		// COMMENT OR BLANK
		if (input[0][0] == '#' ||
					input[0][0] == '\0') {
			continue;
		}
		
		// EXIT
		else if (strcmp(input[0], "exit") == 0) {
			cont = 0;
		}
	
		// CD
		else if (strcmp(input[0], "cd") == 0) {
			// Change to the directory specified
			if (input[1]) {
				if (chdir(input[1]) == -1) {
					printf("Directory not found.\n");
					fflush(stdout);
				}
			} else {
			// If directory is not specified, go to ~
				chdir(getenv("HOME"));
			}
		}

		// STATUS
		else if (strcmp(input[0], "status") == 0) {
			printExitStatus(exitStatus);
		}

		// Anything else
		else {
			execCmd(input, &exitStatus, sa_sigint, &background, inputFile, outputFile);
		}

		// Reset variables
		for (i=0; input[i]; i++) {
			input[i] = NULL;
		}
		background = 0;
		inputFile[0] = '\0';
		outputFile[0] = '\0';

	}
	
	return 0;
}