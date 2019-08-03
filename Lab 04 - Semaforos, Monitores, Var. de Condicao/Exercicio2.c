#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define N 10000000

double *A, *B, soma = 0.0, soma_local = 0.0, soma_red = 0.0;

int main () {
	int i, tempoSC, tempoReduction, tempoTotal;
	A = (double*) malloc(N * sizeof(double));
	B = (double*) malloc(N * sizeof(double));
	struct timeval inicioGeral, finalGeral, inicioSC, finalSC, inicioReduction, finalReduction;

	gettimeofday(&inicioGeral, NULL);
	for (int i = 0; i < N; i++){
		A[i] = rand() % 100;
		B[i] = rand() % 100;
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

	// gettimeofday(&inicioReduction, NULL);
	/*Versao Reduction*/
	/*#pragma omp parallel for private(i) shared(A, B) num_threads(8) reduction (+:soma_red)
	for (i = 0; i < N; i++){
		soma_red += A[i] * B[i];
	}
	gettimeofday(&finalReduction, NULL); */

	gettimeofday(&inicioSC, NULL);
	/*Versao com secao critica*/
	#pragma omp parallel default(shared) firstprivate(soma_local) num_threads(8)
	{
		//soma_local = 0.0;
		#pragma omp for
		for (i = 0; i < N; i++){
			soma_local += A[i] * B[i];
		}
		#pragma omp critical
		soma += soma_local;
		printf("soma local %lf\n",soma_local );
	}
	gettimeofday(&finalSC, NULL);

	gettimeofday(&finalGeral, NULL);

	tempoSC = (int) (1000 * (finalSC.tv_sec - inicioSC.tv_sec) + (finalSC.tv_usec - inicioSC.tv_usec) / 1000);
	// tempoReduction = (int) (1000 * (finalReduction.tv_sec - inicioReduction.tv_sec) + (finalReduction.tv_usec - inicioReduction.tv_usec) / 1000);
	tempoTotal = (int) (1000 * (finalGeral.tv_sec - inicioGeral.tv_sec) + (finalGeral.tv_usec - inicioGeral.tv_usec) / 1000);

	printf("N = %d, MAX_THREADS = 1\n", N);
	printf("SC: %d, Geral: %d\n", tempoSC, tempoTotal);
	// printf("Reduction: %d, Geral: %d\n", tempoReduction, tempoTotal);

	printf("Soma global, red=%lf e secao=%lf\n", soma_red, soma);

}
