#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"

int main(int argc, char *argv[]){
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
		available[j] = atoi(argv[j + 1]);
	}
	init_bank();
	char mode[5] = "";
	int customer;
	int resource[NUMBER_OF_RESOURCES];
	while (1){
		printf("bank > ");
		scanf("%s", mode);
		if (strcmp(mode, "exit") == 0) break;
		if (strcmp(mode, "*") == 0){
			print_info();
			continue;
		} 
		scanf("%d", &customer);
		for (int i = 0; i < NUMBER_OF_RESOURCES; ++i){
			scanf("%d", &resource[i]);
		}
		if (strcmp(mode, "RQ") == 0){
			int rv = request_resources(customer, resource);
			if (rv == 0){
				printf("# Success! Allocate resources to [%d] -- ", customer);
				for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
					printf("%d ", resource[j]);
				}
				printf("\n");
			}
			else if (rv == 1){
				printf("# ERROR! Succeed the MAX need!\n");
			}
			else{
				printf("# ERROR! May lead to an UNSAFE state!\n");
			}
		}
		else if (strcmp(mode, "RL") == 0){
			release_resources(customer, resource);
		}
	}
	return 0;
}