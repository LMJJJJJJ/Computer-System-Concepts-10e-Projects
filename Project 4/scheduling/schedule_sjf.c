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
	struct node *n = tasks; // To find shortest task: n -> task
	int min_burst = n->task->burst;
	Task *min_burst_task = n->task;
	n = n->next; // check from the second
	while (n != NULL){
		if (n->task->burst <= min_burst){ // 在时间相同的中，让先来的先做
			min_burst_task = n->task;
			min_burst = min_burst_task->burst;
		}
		n = n->next;
	}
	return min_burst_task;
}

// invoke the scheduler
void schedule(){
	while (tasks != NULL){
		Task *t = get_next();
		run(t, t->burst);
		delete(&tasks, t);
		free(t); // the original version of delete function did not do this.
	}
}