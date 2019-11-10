#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_NUM 32

int list[MAX_NUM];
void *mergesort(void *param); /* the thread */

typedef struct {
	int start;
	int end;
} parameters;

int main(int argc, char *argv[]){
	printf("Enter no more than 32 integers, split by space, press enter to submit:\n");
	int cnt = 0, input;
	char end;
	do {
		scanf("%d%c", &input, &end);
		list[cnt] = input;
		cnt += 1;
	} while (end != '\n');

	pid_t pid;
	pthread_t tid; pthread_attr_t attr;
	parameters param;
	param.start = 0;
	param.end = cnt - 1;
	pthread_attr_init(&attr);
	pthread_create(&tid,&attr,mergesort,(void*) &param); 
	pthread_join(tid,NULL);
	printf("Sorted list: \n");
	for (int i = 0; i < cnt; i += 1){
    	printf("%d ", list[i]);
    }
    printf("\n");
    return 0;

}

void *mergesort(void *param) { 
	parameters *p = (parameters*) param;
	int left = p -> start;
	int right = p -> end;
	if (left >= right) return NULL;
	int mid = (left + right) / 2;
	// printf("- %d %d %d\n", left, mid, right);
	parameters left_param, right_param;
	left_param.start = left;
	left_param.end = mid;
	// printf("-- %d %d\n", left_param.start, left_param.end);
	right_param.start = mid + 1;
	right_param.end = right;
	// printf("-- %d %d\n", right_param.start, right_param.end);
	pthread_t left_tid, right_tid;
	pthread_attr_t left_attr, right_attr;
	pthread_attr_init(&left_attr);
	pthread_create(&left_tid, &left_attr, mergesort, (void*) &left_param);
	pthread_attr_init(&right_attr);
	pthread_create(&right_tid, &right_attr, mergesort, (void*) &right_param);
	pthread_join(left_tid, NULL);
	pthread_join(right_tid, NULL);
	int *tmp = (int*) malloc(sizeof(int) * (right - left + 1));
	int p_left = left, p_right = mid + 1, p_tmp = 0;
	while (p_left <= mid && p_right <= right){
		if (list[p_left] < list[p_right]) {
			tmp[p_tmp] = list[p_left];
			p_left += 1;
			p_tmp += 1;
		}
		else {
			tmp[p_tmp] = list[p_right];
			p_right += 1;
			p_tmp += 1;
		}
	}
	while (p_left <= mid){
		tmp[p_tmp] = list[p_left];
		p_left += 1;
		p_tmp += 1;
	}
	while (p_right <= right){
		tmp[p_tmp] = list[p_right];
		p_right += 1;
		p_tmp += 1;
	}
	for (p_tmp = 0; p_tmp < right - left + 1; p_tmp += 1){
		list[p_tmp + left] = tmp[p_tmp];
	}
	free(tmp);
	pthread_exit(0);
}
