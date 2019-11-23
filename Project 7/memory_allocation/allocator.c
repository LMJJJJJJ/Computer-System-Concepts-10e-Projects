#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

int main(int argc, char *argv[]){
	if (argc != 2){
		printf("Parameter Error: Need 1 parameter!\n");
		return 1;
	}
	init_memory(atoi(argv[1]));
	char mode[5];
	char name[5];
	int length;
	char method[5];
	while(1){
		fflush(stdout);
		printf("allocator > ");
		scanf("%s", mode);
		if (strcmp(mode, "X") == 0){
			break;
		}
		else if (strcmp(mode, "C") == 0){
			compact();
			continue;
		}
		else if (strcmp(mode, "STAT") == 0){
			print_memory_info();
			continue;
		}
		else if (strcmp(mode, "RQ") == 0){
			scanf("%s %d %s", name, &length, method);
			if (strcmp(method, "W") == 0){
				worst_fit(name, length);
			}
			else if (strcmp(method, "B") == 0){
				best_fit(name, length);
			}
			else {
				first_fit(name, length);
			}
		}
		else if (strcmp(mode, "RL") == 0){
			scanf("%s", name);
			release(name);
		}

	}
	// print_memory_info();
	// first_fit("P1", 300);
	// print_memory_info();
	// first_fit("P2", 300);
	// print_memory_info();
	// first_fit("P3", 400);
	// print_memory_info();
	// release("P3");
	// print_memory_info();
	// release("P1");
	// print_memory_info();
	// worst_fit("P4", 50);
	// print_memory_info();
	// release("P2");
	// print_memory_info();
	return 0;
}