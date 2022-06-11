#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
  srand(time(NULL));
  int len = atoi(argv[1]);
  int x;
  if(argc < 2){
    fprintf(stderr, "%s", "Not enough arguments");
    exit(1);
  }
  for (int i=0; i<len; i++){
    x = (rand() % (91-65+1)+65);
    if(x == 91){
      x = 32;
    }
    printf("%c", x);
  }
  printf("\n");
  return 0;
}