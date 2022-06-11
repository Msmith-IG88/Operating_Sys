#include <stdlib.h>

void execCmd(char*[], int*, struct sigaction, int*, char[], char[]);
void get_command(char*[], int*, char[], char[], int);
void catchSIGTSTP(int);
void printExitStatus(int);