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

Task* get_next(){
	if (tasks == NULL) return NULL;
	struct node *n = tasks; // To find highest priority task: n -> task
	int max_priority = n->task->priority;
	Task *max_priority_task = n->task;
	n = n->next; // check from the second
	while (n != NULL){
		if (n->task->priority >= max_priority){ // 在优先级相同的中，让先来的先做
			max_priority_task = n->task;
			max_priority = max_priority_task->priority;
		}
		n = n->next;
	}
	return max_priority_task;
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