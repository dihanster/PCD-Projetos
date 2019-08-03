#include <stdio.h>

#define N 100000000
#define u 2.0

int main(void) {
	double passo, soma, x;
	int i;
	int processId; /* rank dos processos */
  	int noProcesses; /* Número de processos */
  	MPI_Status status;

	passo = (u-1) / (double) N;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProc);
  	MPI_Comm_rank(MPI_COMM_WORLD, &esteProc);
	for (i=0; i<N;i++) {
		x=1+i*passo;
		soma=soma+0.5*(1/x+1/(x+passo));
	}
	MPI_Finalize();
	printf("ln %f = %20.15f\n", u, passo*soma);
	return 0; 
}