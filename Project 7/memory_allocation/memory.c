#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

Block *head;
int capacity;

void init_memory(int max){
	printf("# Initializing Memory -- [0 : %d]\n", max);
	capacity = max;
	head = malloc(sizeof(Block));
	head->begin = -1;
	head->end = -1;
	head->next = NULL; // head is an empty block
	Block *block = malloc(sizeof(Block));
	block->begin = 0;
	block->end = capacity - 1;
	block->next = NULL;
	strcpy(block->info, "Unused");
	head->next = block;
}

void print_single_info(Block *block){
	printf("| Address [%d : %d]\t| %s\t|\n", block->begin, block->end, block->info);
}

void print_memory_info(){
	printf("-----------------------------------------\n");
	Block *p = head->next;
	while (p) {
		print_single_info(p);
		p = p->next;
	}
	printf("-----------------------------------------\n");
}

void insert_block(char name[], Block *b, int length){
	// put the new block after b
	// revise the block after the new one
	Block *block = malloc(sizeof(Block));
	block->begin = b->next->begin;
	block->end = block->begin + length - 1;
	block->next = b->next;
	strcpy(block->info, "Process ");
	strcat(block->info, name);
	b->next = block;
	block->next->begin = block->end + 1;
	// block->next can be empty
	if (block->next == NULL) return;
	if (block->next->begin >= block->next->end) {
		Block *tmp = block->next;
		block->next = tmp->next;
		free(tmp);
	}
}

/* ////////////////////////////////
// check b, b->next, b->next->next
// b->next must be [Unused]
// //////////////////////////////*/
void merge(Block *b){
	// CASE 1: b is head -- only consider b->next, b->next->next
	if (b == head) { 
		b = b->next;
		// check b, b->next
		// b must be [Unused]
		if (b->next == NULL) return;
		// if the next is also unused -- merge!
		if (strcmp(b->next->info, "Unused") == 0){
			Block *tmp = b->next;
			b->end = tmp->end;
			b->next = tmp->next;
			free(tmp);
			return;
		}
		// if the next is used -- return directly.
		return;
	}
	// CASE 2: only consider b, b->next
	else if (b->next->next == NULL){
		Block *tmp = b->next; // tmp must be [Unused]
	    if (tmp->next == NULL){
		    if (strcmp(b->info, "Unused") != 0){
		    	return;
		    }
		    else {
		    	b->next = NULL;
		        b->end = tmp->end;
		        free(tmp);
		        return;
		    }
	    }
	}
	// CASE 3: b, b->next, b->next->next all meaningful
	Block *tmp = b->next; // tmp must be [Unused]
	// check for merging -- 0 0 0, 0 0 1, 1 0 0, 1 0 1
	int previous_work, successor_work;
	previous_work = (strcmp(b->info, "Unused") != 0);
	successor_work = (strcmp(tmp->next->info, "Unused") != 0);
	// 1 0 1
	if (previous_work == 1 && successor_work == 1){
		return;
	}
	int begin = tmp->begin, end = tmp->end;
	b->next = tmp->next;
	free(tmp);
	// 0 0 1
	if (previous_work == 0 && successor_work != 0){
		b->end = end;
		return;
	}
	// 1 0 0
	if (previous_work != 0 && successor_work == 0){
		b->next->begin = begin;
		return;
	}
	// 0 0 0
	b->end = b->next->end;
	tmp = b->next;
	b->next = tmp->next;
	free(tmp);

}

void remove_block(Block *b){
	// remove b->next
	// check for merging
	Block *tmp = b->next;
	strcpy(tmp->info, "Unused");
	merge(b);
	
}

int release(char name[]){
	printf("# Trying to release space of Process %s\n", name);
	Block *p = head;
	char info[MAX_INFO];
	strcpy(info, "Process ");
	strcat(info, name);
	while (p){
		if (strcmp(p->next->info, info) == 0){
			break;
		}
		p = p->next;
	}
	remove_block(p);
	return 0; // success
}

int first_fit(char name[], int length){
	printf("# Trying to allocate space for [%s] using first fit\n", name);
	Block *p = head;
	while (p) {
		if (p->next == NULL){
			return 1; // no space for this allocation
		}
		if (strcmp(p->next->info, "Unused") == 0 && p->next->end - p->next->begin + 1 >= length){
			break;
		}
		p = p->next;
	}
	insert_block(name, p, length); // insert the block after p
	return 0; // success
}

int best_fit(char name[], int length){
	printf("# Trying to allocate space for [%s] using best fit\n", name);
	Block *p = head;
	Block *min_block = NULL;
	int min_length = capacity + 1;
	while (p) {
		if (p->next == NULL){
			if (min_block == NULL) return 1; // no space for this allocation
			else break;
		}
		if (strcmp(p->next->info, "Unused") == 0 && p->next->end - p->next->begin + 1 >= length){
			if (p->next->end - p->next->begin + 1 < min_length){
				min_block = p;
				min_length = p->next->end - p->next->begin + 1;
			}
		}
		p = p->next;
	}
	insert_block(name, min_block, length); // insert the block after p
	return 0; // success
}

int worst_fit(char name[], int length){
	printf("# Trying to allocate space for [%s] using worst fit\n", name);
	Block *p = head;
	Block *max_block = NULL;
	int max_length = 0;
	while (p) {
		if (p->next == NULL){
			if (max_block == NULL) return 1; // no space for this allocation
			else break;
		}
		if (strcmp(p->next->info, "Unused") == 0 && p->next->end - p->next->begin + 1 >= length){
			if (p->next->end - p->next->begin + 1 > max_length){
				max_block = p;
				max_length = p->next->end - p->next->begin + 1;
			}
		}
		p = p->next;
	}
	insert_block(name, max_block, length); // insert the block after p
	return 0; // success
}

void compact(){
	Block *h = head;
	int next_begin = 0;
	Block *tmp;
	Block *p = head->next;
	while (p){
		if (strcmp(p->info, "Unused")){
			tmp = malloc(sizeof(Block));
			tmp->begin = next_begin;
			tmp->end = p->end - p->begin + tmp->begin;
			next_begin = tmp->end + 1;
			strcpy(tmp->info, p->info);
			tmp->next = h->next;
			h->next = tmp;
			h = h->next;
		}
		p = p->next;
	}
	tmp = malloc(sizeof(Block));
	tmp->begin = next_begin;
	tmp->end = capacity - 1;
	tmp->next = h->next;
	strcpy(tmp->info, "Unused");
	h->next = tmp;
	h = h->next;
	while(h->next){
		tmp = h->next;
		h->next = tmp->next;
		free(tmp);
	}
}