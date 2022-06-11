#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_NARGS      		512
#define MAX_INPUT_LENGTH    2048

int bgToggle;
int sigtstpFlag;
int sigintFlag;

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
 
void getCommand(char** argList, int *nArgs) {
	char inBuff[MAX_INPUT_LENGTH];
	int i;
	char* argKey;
	do {
		printf(": ");
		fgets(inBuff, MAX_INPUT_LENGTH, stdin);
		strtok(inBuff, "\n");
	} while(inBuff[0] == '#'|| strlen(inBuff) < 1);


    for (i = 0; i < strlen(inBuff); i ++) {
		if ( (inBuff[i] == '$')  && (inBuff[i + 1] == '$') && (i + 1 < strlen(inBuff))) {
			char * temp = strdup(inBuff);
			temp[i] = '%';
			temp[i + 1] = 'd';
			sprintf(inBuff, temp, getpid());
			free(temp);
		}
	}

	argKey = strtok(inBuff, " ");	
	while(argKey != NULL) {
		argList[*nArgs] = strdup(argKey);
		(*nArgs)++;
		argKey = strtok(NULL, " ");
	}
}

void runShell(char** argList, int *nArgs) {
	int bgFlag = 0;
	int status;
	int redirect;
	int FD;
	int i;
	pid_t pid = 0;
	struct sigaction SIGINT_action = {{0}}, SIGTSTP_action = {{0}};

	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);

	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	//pid - getpid();

		if (strcmp(argList[*nArgs - 1], "&") == 0) {
			(*nArgs)--;
			argList[*nArgs] = NULL;
			if (bgToggle == 0){
				bgFlag = 1;
				(*nArgs)++;
			}
		}

		if (strcmp(argList[0], "exit") == 0) {
			fflush(stdout);
			while (1){
				exit(0);
			}
		} 

		else if (strcmp(argList[0], "cd") == 0) {
			if (*nArgs == 1)
				chdir(getenv("HOME"));
			else
				chdir(argList[1]);
		}

		else if (strcmp(argList[0], "status") == 0){
			if (WIFEXITED(status)){
				printf("exit value %d\n", WEXITSTATUS(status));
			}
			else if (WIFSIGNALED(status)){
				printf("terminated by signal %d\n", WTERMSIG(status));
			}
		}
		
		else{
			pid = fork();
			switch (pid){
				case -1:
					perror("fork() failed\n");
					exit(1);
					break;
				case 0: 
					if (bgFlag == 0){
						SIGINT_action.sa_handler = SIG_DFL;
						sigaction(SIGINT, &SIGINT_action, NULL);
					}
					
					for (i = 1; i < *nArgs; i++) {
						if (bgFlag == 1) {	
							redirect = 1;
							FD = open("/dev/null", O_RDONLY);
							int result = dup2(FD, STDIN_FILENO);
							if (result == -1) { 
								perror("source open()"); 
								exit(1); 
							}
						}
						if ((strcmp(argList[i], "<") == 0)) {
							redirect = 1;
							
							FD = open(argList[i + 1], O_RDONLY);
							int result = dup2(FD, STDIN_FILENO);
							if (result == -1) { 
								perror("source open()"); 
								exit(1); 
							}
						}
						if ((strcmp(argList[i], ">") == 0))  {
							redirect = 1;
							FD = open(argList[i + 1], O_CREAT | O_RDWR | O_TRUNC, 0644);
							int result = dup2(FD, STDOUT_FILENO); 
							if (result == -1){
								perror("source open()"); 
								exit(1); 
							}
						}
					}
					if (redirect == 1){
						close(FD);
						for(i = 1; i < *nArgs; i++)
							argList[i] = NULL;
					}
					if (execvp(argList[0], argList) && sigtstpFlag != 1 && sigintFlag != 1){
						fprintf(stderr, "%s: No such file or directory\n", argList[0]);
						exit(1);
					}
					break;

				default:
					if (bgFlag == 1){
						printf("background pid is %d\n", pid);
					}
					else { 
						waitpid(pid, &status, 0);
						if(sigtstpFlag != 1){
							if (WIFSIGNALED(status) == 1 && WTERMSIG(status) != 0){
								printf("terminated by signal %d\n", WTERMSIG(status));
							}
							while (pid != -1){
								pid = waitpid(-1, &status, WNOHANG);	
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

int main() {
	int nArgs;
	char* argList[MAX_NARGS];
	bgToggle = 0;

	while (1){
		nArgs = 0;
		sigtstpFlag = 0;
		sigintFlag = 0;
		memset(argList, '\0', MAX_NARGS);
		fflush(stdout);
		fflush(stdin);

		getCommand(argList, &nArgs);
		runShell(argList, &nArgs);
	}
	
	return 0;
}