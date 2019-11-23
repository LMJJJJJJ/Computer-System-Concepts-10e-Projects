#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"



void init_bank(){
	printf("# Initializing the bank.\n");
	const char MAX_FILE[] = "max.txt";
	FILE *in;
    char *temp;
    char process[MAX_LINE];

    in = fopen(MAX_FILE, "r");
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
    	fgets(process, MAX_LINE, in);
    	temp = strdup(process);
    	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
    		maximum[i][j] = atoi(strsep(&temp,","));
    	}
    	free(temp);
    }

    fclose(in);

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
    	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
    		allocation[i][j] = 0;
    		need[i][j] = maximum[i][j] - allocation[i][j];
    		// printf("%d ", maximum[i][j]);
    	}
    	// printf("\n");
    }
}

void print_info(){
	printf("Available Resources\n");
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
		printf("%d\t", available[j]);
	}
	printf("\n");

	printf("MAXIMUM\n");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
			printf("%d\t", maximum[i][j]);
		}
		printf("\n");
	}
	printf("NEED\n");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
			printf("%d\t", need[i][j]);
		}
		printf("\n");
	}
	printf("ALLOCATE\n");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
			printf("%d\t", allocation[i][j]);
		}
		printf("\n");
	}

}

void allocate_resources(int customer_num, int request[]){
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
		available[j] -= request[j];
		allocation[customer_num][j] += request[j];
		need[customer_num][j] -= request[j];
	}
}

void deallocate_resources(int customer_num, int request[]){
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
		available[j] += request[j];
		allocation[customer_num][j] -= request[j];
		need[customer_num][j] += request[j];
	}
}

int no_greater_than(int a[], int b[], int dim){
	for (int i = 0; i < dim; ++i){
		if (a[i] > b[i]) return 0;
	}
	return 1;
}

int is_safe(int customer_num, int request[]){
	allocate_resources(customer_num, request);
	int finish[NUMBER_OF_CUSTOMERS];
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		finish[i] = 0;
	}
	int flag = 0;
	int count = 0;
	int work[NUMBER_OF_RESOURCES];
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i){
		work[i] = available[i];
	}
	while (1){
		flag = 0;
		for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
			if (finish[i] == 0 && no_greater_than(need[i], work, NUMBER_OF_RESOURCES)){
				flag = 1;
				for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
					work[j] += allocation[i][j];
				}
				finish[i] = 1;
				count += 1;
			}
		}
		if (flag == 0) break;
	}
	deallocate_resources(customer_num, request);
	return count == NUMBER_OF_CUSTOMERS;
}



int request_resources(int customer_num, int request[]){
	for (int j = 0; j < NUMBER_OF_RESOURCES; ++j){
		if (request[j] > need[customer_num][j]) return 1; // exceed maximum need
	}
	if (is_safe(customer_num, request)){
		allocate_resources(customer_num, request);
		return 0; // success
	}
	return 2; // lead to an unsafe state
}



void release_resources(int customer_num, int release[]){
	if (no_greater_than(release, allocation[customer_num], NUMBER_OF_RESOURCES)){
		deallocate_resources(customer_num, release);
	}
	else {
		printf("Failed in releasing resuorces.\n");
	}
}