/* buffer.h */
typedef int buffer_item;

#define BUFFER_SIZE 5
#define SINGLE_THREAD_MAX_SLEEP_TIME 5

int is_full();
int is_empty();
int insert_item(buffer_item item); // insert the item
int remove_item(buffer_item *item); // remove and store it in item
void init_buffer(); // initialize the mutex and semaphores