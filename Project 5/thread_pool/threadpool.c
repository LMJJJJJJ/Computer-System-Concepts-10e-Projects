/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
} task;



pthread_t bees[NUMBER_OF_THREADS]; // the worker list bees
task queue[QUEUE_SIZE]; // task queue
pthread_mutex_t mutex; // For the enqueue and dequeue of tasks
sem_t available; // available tasks to do

int front = 0, rear = 0, count = 0; // review the implementation of 'circular-queue'

int empty(){
    return count == 0;
}

int full(){
    return count == QUEUE_SIZE;
}

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t){
    // printf("# Enqueue.\n");
    pthread_mutex_lock(&mutex);
    if (full()){
        pthread_mutex_unlock(&mutex);
        return 1; // failure because the queue is full
    }
    queue[rear] = t;
    rear = (rear + 1) % QUEUE_SIZE;
    count += 1;
    pthread_mutex_unlock(&mutex);
    return 0;
}

// remove a task from the queue
task dequeue(){
    // printf("# Dequeue.\n");
    pthread_mutex_lock(&mutex);
    task t = queue[front];
    front = (front + 1) % QUEUE_SIZE;
    count -= 1;
    pthread_mutex_unlock(&mutex);
    return t;
}

// the worker thread in the thread pool
void *worker(void *param){
    task worktodo;
    while (TRUE){
        sem_wait(&available);
        worktodo = dequeue();
        // printf("# Execute the task.\n");
        execute(worktodo.function, worktodo.data); // execute the task
    }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p){
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p){
    printf("# Submit a task.\n");
    task worktodo;
    worktodo.function = somefunction;
    worktodo.data = p;
    if (enqueue(worktodo)){
        printf("# ERROR: SUBMIT FAILURE! QUEUE IS FULL!\n");
        return 1; // failure
    }
    sem_post(&available);
    return 0; // success
}

// initialize the thread pool
void pool_init(void){
    printf("# Initializing the thread pool... [NUMBER_OF_THREADS]=%d, [QUEUE_SIZE]=%d\n",
            NUMBER_OF_THREADS, QUEUE_SIZE);
    pthread_mutex_init(&mutex, NULL); // reference -- 7.3.1
    sem_init(&available, 0, 0); // reference -- 7.3.2
    for (int i = 0; i < NUMBER_OF_THREADS; ++i){
        pthread_create(&bees[i], NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void){
    for (int i = 0; i < NUMBER_OF_THREADS; ++i){
        pthread_cancel(bees[i]);
        pthread_join(bees[i], NULL);
    }
}
