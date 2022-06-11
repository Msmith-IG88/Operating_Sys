struct movie 
{
	char* title;
	unsigned int year;
	char** languages;
	float rating;
	struct movie *next;
};

struct node
{
	char *val;
	struct node *next;
};