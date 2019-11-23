#define MAX_INFO 20

typedef struct block {
	char info[MAX_INFO];
	int begin;
	int end;
	struct block* next;
} Block;

void init_memory(int max);

void print_single_info(Block *block);
void print_memory_info();
int first_fit(char name[], int length);
int best_fit(char name[], int length);
int worst_fit(char name[], int length);
int release(char name[]);
void compact();