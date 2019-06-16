#include "CThreadPoolPlus.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#define COUNT 10000

void *test_func(void *arg)
{
	;
}

void *func(void *arg)
{
	double time_use;
	struct timeval start;
	struct timeval end;

	gettimeofday(&start, NULL);
	for (int ii = 0; ii < COUNT; ++ii)
	{
		CThreadPoolPlus::Instance()->DoTask(test_func, NULL);
	}
	gettimeofday(&end, NULL);

	time_use = (end.tv_sec - start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
	time_use /= 1000000;

	printf("time_use is %4.3f\n", time_use);
}

int main(int argc, char **argv)
{
	pthread_t tid_1, tid_2, tid_3, tid_4;
	pthread_create(&tid_1, NULL, func, NULL);
	pthread_create(&tid_2, NULL, func, NULL);
	pthread_create(&tid_3, NULL, func, NULL);
	pthread_create(&tid_4, NULL, func, NULL);

	pause();

	return 0;
}

