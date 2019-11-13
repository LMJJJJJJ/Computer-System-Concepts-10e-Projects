#include "buffer.h"
#include <stdlib.h> /* required for rand() */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *producer(void *param){
	buffer_item item;
	while (1){
		/* sleep for a random period of time */
		sleep(0.1);
		/* generate a random number */
		item = rand();
		if (insert_item(item))
			fprintf(stderr, "report error condition\n");
		else
			; // do nothing
			// printf("producer produced %d.\n", item);
	}
}

void *consumer(void *param){
	buffer_item item;
	while (1){
		/* sleep for a random period of time */
		sleep(rand() % SINGLE_THREAD_MAX_SLEEP_TIME + 1);
		if (remove_item(&item))
			fprintf(stderr, "report error condition\n");
		else
			; // do nothing
			// printf("consumer consumed %d.\n", item);
	}
}


int main(int argc, char *argv[]){
	/* 1. Get command line arguments argv[1],argv[2],argv[3] */
	/* 2. Initialize buffer */
	/* 3. Create producer thread(s) */
	/* 4. Create consumer thread(s) */
	/* 5. Sleep */
	/* 6. Exit */
	if (argc != 4) {
		printf("# ERROR: Entered %d args. Need to be exactly 3 args. \n", argc - 1);
		return 0;
	}
	// get the arguments
	int sleep_time = atoi(argv[1]);
	int producer_num = atoi(argv[2]);
	int consumer_num = atoi(argv[3]);
	printf("[SLEEP_TIME]=%d, [PRODUCER_NUM]=%d, [CONSUMER_NUM]=%d\n", sleep_time, producer_num, consumer_num);
	// initialize the buffer
	init_buffer();
	// create producer and consumer threads.
	pthread_t *producers = malloc(producer_num * sizeof(pthread_t));
	pthread_t *consumers = malloc(consumer_num * sizeof(pthread_t));
	for (int i = 0; i < producer_num; ++i){
		pthread_create(&producers[i], NULL, producer, NULL);
	}
	for (int i = 0; i < consumer_num; ++i){
		pthread_create(&consumers[i], NULL, consumer, NULL);
	}

	sleep(sleep_time);
	for(int i = 0; i < producer_num; ++i){
        pthread_cancel(producers[i]);
        pthread_join(producers[i], NULL);
    }
    for(int i = 0; i < consumer_num; ++i){
        pthread_cancel(consumers[i]);
        pthread_join(consumers[i], NULL);
    }
    free(producers);
    free(consumers);
	return 0;
}