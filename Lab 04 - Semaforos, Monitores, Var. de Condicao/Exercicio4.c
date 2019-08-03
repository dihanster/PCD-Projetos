#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

sem_t inicio, s1, s2;

void *processo1(void *id){
	sem_wait(&s1);
	printf("C");
	sem_post(&s2);
	printf("E");
	sem_post(&s2);
	pthread_exit(0);
}

void *processo2(void *id){
	sem_wait(&inicio);
	printf("A");
	sem_post(&s1);
	sem_wait(&s2);
	printf("R");
	sem_wait(&s2);
	printf("O");
	sem_post(&inicio);
	pthread_exit(0);
}

int main(){
	sem_init(&s1, 0, 0);
	sem_init(&s2, 0, 0);
	sem_init(&inicio, 0, 1);

	pthread_t t[2];
	for (int i = 0; i < 10; i++){
		pthread_create(&t[1], NULL, processo2, (void*) 1);
		pthread_create(&t[0], NULL, processo1, (void*) 0);
		pthread_join(t[0], NULL);
		pthread_join(t[1], NULL);
		printf("\n");
	}	
}