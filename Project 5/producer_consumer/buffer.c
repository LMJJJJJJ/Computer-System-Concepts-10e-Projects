#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "buffer.h"
/* the buffer */

buffer_item buffer[BUFFER_SIZE];

int count = 0, front = 0, rear = 0;

pthread_mutex_t mutex;
sem_t empty, full;

int is_full(){
	return count == BUFFER_SIZE;
}

int is_empty(){
	return count == 0;
}

int insert_item(buffer_item item) {
	/* insert item into buffer
	   (1) return 0 if successful,
	   (2) otherwise return -1 indicating an error condition */
	int rv = 1;
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	if (is_full()){
		rv = 1;
	}
	else {
		buffer[rear] = item;
		count += 1;
		printf("# Item %d added. Now %d items.\n", item, count);
		rear = (rear + 1) % BUFFER_SIZE;
		rv = 0;
	}
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	return rv;
}

int remove_item(buffer_item *item){
	/* remove an object from buffer, place it in item
	   (1) return 0 if successful
	   (2) otherwise, return -1 indicating an error condition */
	int rv = 1;
	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	if (is_empty()){
		rv = 1;
	}
	else {
		*item = buffer[front];
		count -= 1;
		printf("# Item %d consumed. Now %d items.\n", *item, count);
		front = (front + 1) % BUFFER_SIZE;
		rv = 0;
	}
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);
	return rv;
}

void init_buffer(){
	pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
}