#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm

/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.

*/
#define LINE_LENGTH 1000
#define LINES 50
// Size of the buffers
#define SIZE 50000
#define COUNT 80
// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 6

void ps(char *context, char *msg){
#ifdef ALVDBG
  printf("%s | %s\n", context, msg);
  fflush(stdout);
#endif
}

// Buffer 1, shared resource between input thread and square-root thread
char buffer_1[SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between square root thread and output thread
char buffer_2[SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the square-root thread will put the next item
int prod_idx_2 = 0;
// Index where the output thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

char buffer_3[SIZE];
int count_3 = 0;
int count_4 = 0;

pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_4 = PTHREAD_COND_INITIALIZER;

/*
Get input from the user.
This function doesn't perform any error checking.
*/
int get_user_input(){
  int value = 0;
  printf("Enter a positive integer: ");
  scanf("%d", &value);
  return value;
}

/*
 Put an item in buff_1
*/
void put_buff_1(int item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  // Put the item in the buffer
  buffer_1[prod_idx_1] = item;
  // Increment the index where the next item will be put.
  prod_idx_1 = prod_idx_1 + 1;
  count_1++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the square_root thread.
*/
void *get_input(void *args)
{
  int value = 0;
  char *l = calloc(LINE_LENGTH, sizeof(char));
  
  while(value == 0){
    fgets(l, LINE_LENGTH, stdin);
    pthread_mutex_lock(&mutex_1);
    strcpy(buffer_1, l);
    count_1++;

    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
    pthread_mutex_lock(&mutex_4);

    while(count_4 ==0){
      pthread_cond_wait(&full_4, &mutex_4);
    }
    count_4++;
    pthread_mutex_unlock(&mutex_4);

    if(strcmp(l, "STOP\n") ==0){
      value = 1;
    }
  }
  ps("read input","read end");
  return NULL;
/*
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      // Get the user input
      int item = get_user_input();
      put_buff_1(item);
    }
    return NULL;
*/
}

/*
Get the next item from buffer 1
*/
int get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  int item = buffer_1[con_idx_1];
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return item;
}

/*
 Put an item in buff_2
*/
void put_buff_2(double item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  buffer_2[prod_idx_2] = item;
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}

char *replace(const char *ln, const char *t, const char *replace_t){
  assert(ln);
  assert(t);
  assert(replace_t);
  assert(strlen(t) >= strlen(replace_t));

  char *temp = calloc(strlen(ln)+1, sizeof(char));
  const char *a = NULL;
  const char *r = ln;

  while((a = strstr(r, t))){
    strncat(temp, r, (size_t)(a - r));
    strcat(temp, replace_t);
    r = a + strlen(t);
  }
  strcat(temp, r);
  return temp;
}

/*
 Function that the square root thread will run. 
 Consume an item from the buffer shared with the input thread.
 Compute the square root of the item.
 Produce an item in the buffer shared with the output thread.

*/
void *replace_plus(void *args)
{
  int item = 0;
  char *l = calloc(LINE_LENGTH, sizeof(char));
  char *newL;

  while(item == 0){
    pthread_mutex_lock(&mutex_2);
    while(count_2 == 0){
      pthread_cond_wait(&full_2, &mutex_2);
    }
    strcpy(l, buffer_2);
    count_2--;
    pthread_mutex_unlock(&mutex_2);

    if (strcmp(l, "STOP\n") ==0){
      item = 1;
      ps("plusplus", "plus stop");
      newL = l;
    } else {
      newL = replace(l, "++", "^");
    }
    pthread_mutex_lock(&mutex_3);
    strcpy(buffer_3, newL);
    count_3++;

    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
  }
  ps("plus plus", "plus exit");
  return NULL;
/*    
    double square_root;
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      item = get_buff_1();
      square_root = sqrt(item);
      put_buff_2(square_root);
    }
    return NULL;
*/
}

void *replace_sp(void *args){
  char* str = calloc(LINE_LENGTH, sizeof(char));
	int term_sym = 0;	
	while (term_sym == 0) {

		pthread_mutex_lock(&mutex_1);

		while (count_1 == 0) {
			pthread_cond_wait(&full_1, &mutex_1);
		}

		strcpy(str, buffer_1);
		count_1--;

		pthread_mutex_unlock(&mutex_1);
		
		if (strcmp(str, "STOP\n") == 0) {	
			term_sym = 1;
			ps("space replace", "space stop");
		}	else { 
			int len = strlen(str);
			if (str[len - 1] == '\n') {	
				str[len - 1] = ' ';
			}
		}

		pthread_mutex_lock(&mutex_2);
			
		strcpy(buffer_2, str);
		
		count_2++;

		pthread_cond_signal(&full_2);
		pthread_mutex_unlock(&mutex_2);
	}

	ps("space replace", "space exit");

	return NULL;
}

/*
Get the next item from buffer 2
*/
double get_buff_2(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  double item = buffer_2[con_idx_2];
  // Increment the index from which the item will be picked up
  con_idx_2 = con_idx_2 + 1;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return item;
}


/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void *write_output(void *args)
{
  char output[81] = {0};
  int lastLine =0;
  int term = 0;
  char *l = calloc(LINE_LENGTH, sizeof(char));
  char *printBuf = calloc(SIZE, sizeof(char));

  while (term == 0){
    pthread_mutex_lock(&mutex_3);
    while(count_3 == 0){
      pthread_cond_wait(&full_3, &mutex_3);
    }
    strcpy(l, buffer_3);
    strcat(printBuf, buffer_3);
    count_3--;

    pthread_mutex_unlock(&mutex_3);
    if(strcmp(l, "STOP\n") ==0){
      term = 1;
      ps("output", "output stop");
    } else{
      int lnCount = (strlen(printBuf)) / COUNT;
      for (int i = lastLine; i<lnCount; i++){
        memset(output, 0, 81);
        strncpy(output, i*COUNT + printBuf, COUNT);
        printf("%s\n", output);
        fflush(stdout);
      }
      lastLine = lnCount;
    }
    pthread_mutex_lock(&mutex_4);
    count_4--;
    pthread_cond_signal(&full_4);
    pthread_mutex_unlock(&mutex_4);
  }
  ps("output", "output exit");
  return NULL;
}

int main()
{
    //srand(time(0));
    pthread_t input_t, output_t, plus_t, rpl_t;
    // Create the threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&rpl_t, NULL, replace_sp, NULL);
    pthread_create(&plus_t, NULL, replace_plus, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(rpl_t, NULL);
    pthread_join(plus_t, NULL);
    pthread_join(output_t, NULL);
    return EXIT_SUCCESS;
}