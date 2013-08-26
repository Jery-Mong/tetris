#include <pthread.h>
#include <signal.h>
#include <stdio.h>

pthread_t tid0;
pthread_t tid1;

void sig_fn(int signo)
{
	printf("this thread is kill \n");
	pthread_exit(NULL);
}

void *thr_fn0(void * data)
{
	sleep(30);
	printf("this thread exits normally\n ");
}
void *thr_fn1(void * data)
{
	pthread_kill(tid0, SIGUSR1);
	pthread_exit(NULL);
}

int main()
{

	signal(SIGUSR1, sig_fn);
	pthread_create(&tid0, NULL, thr_fn0, NULL);
	sleep(1);
	pthread_create(&tid1, NULL, thr_fn1, NULL);
	pthread_join(tid0,NULL);
	printf("waiting :");
	getchar();
}
