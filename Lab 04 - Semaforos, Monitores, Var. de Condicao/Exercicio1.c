#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define N 100000
#define n_threads 8

double *A, *B, soma = 0.0;
pthread_mutex_t mutex;

void *calcula_prod_esc(void *tid){
	double soma_local = 0.0;
	long id = (long) tid;
	//printf("Sou a thread %d e vou de %d a %d \n", id, (id*N/n_threads), (id+1)*(N/n_threads));

	for (int i = (id * N/n_threads); i < ((id+1)*(N/n_threads)); i++){
		soma_local += A[i] * B[i];
	}

 	pthread_mutex_lock(&mutex);
	soma += soma_local;
 	pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main () {
	int tmili;
	struct timeval inicio, final2;
	pthread_t t[n_threads];
	pthread_mutex_init(&mutex, NULL);

	A = (double*) malloc(N * sizeof(double));
	B = (double*) malloc(N * sizeof(double));

	gettimeofday(&inicio, NULL);

	for (int i = 0; i < N; i++){
		A[i] = rand(); //% 100;
		B[i] = rand(); //% 100;
	}
/*
	printf("Vetor A\n");
	for (int i = 0; i < N; i++){
		printf("%lf\n", A[i]);
	}
	printf("Vetor B\n");
	for (int i = 0; i < N; i++){
		printf("%lf\n", B[i]);
	}
*/

	for (long i = 0; i < n_threads; i++){
		pthread_create(&t[i], NULL, calcula_prod_esc, (void*) i);
	}
	for (long i = 0; i < n_threads; i++){
		pthread_join(t[i], NULL);
	}

	gettimeofday(&final2, NULL);
  	tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) +
			(final2.tv_usec - inicio.tv_usec) / 1000);

	printf("Soma global eh %lf \n", soma);
	printf("N = %d, MAX_THREADS = %d\n", N, n_threads);
	printf("Tempo Gasto foi %d\n", tmili);

}
