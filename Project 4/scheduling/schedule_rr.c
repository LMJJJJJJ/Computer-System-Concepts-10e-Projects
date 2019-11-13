// QUANTUM is defined in cpu.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "schedulers.h"
#include "task.h"
#include "cpu.h"
#include "list.h"

struct node *tasks = NULL;

// add a task to the list 
void add(char *name, int priority, int burst){
	Task *t = malloc(sizeof(Task));
	t->name = name;
	t->priority = priority;
	t->burst = burst;
	insert(&tasks, t);
}

// 后加的Task会出现在队首
Task* get_next(){
	if (tasks == NULL) return NULL;
	struct node *n = tasks; // To find the first added task: n -> task
	while (n->next != NULL){
		n = n->next;
	}
	return n->task;
}

void consume(Task *t){
	if (t->burst <= QUANTUM){
		run(t, t->burst);
		delete(&tasks, t);
		free(t);
	}
	else {
		run(t, QUANTUM);
		delete(&tasks, t);
		t->burst -= QUANTUM;
		insert(&tasks, t);
	}
}

// invoke the scheduler
void schedule(){
	while (tasks != NULL){
		Task *t = get_next();
		consume(t);
	}
}