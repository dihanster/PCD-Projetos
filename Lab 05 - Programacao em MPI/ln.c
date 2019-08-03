#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define N 100000000
#define u 2.0

int main(int argc, char **argv) {
	int i, tamLocal, pri, qtde, tmili;
	int processId; /* rank dos processos */
	int noProcesses; /* Número de processos */
  int nameSize; /* Tamanho do nome */
  char computerName[MPI_MAX_PROCESSOR_NAME];
	double passo, soma_global = 0.0, x;
	passo = (u-1) / (double) N;
  MPI_Status status;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &processId);
  MPI_Get_processor_name(computerName, &nameSize);

  struct timeval inicio_procs, fim_procs;

  if (processId == 0)
    gettimeofday(&inicio_procs, NULL);

  double soma_local[noProcesses];
    for (i = 0; i < noProcesses; i++)
      soma_local[i] = 0.0;

  tamLocal = N/noProcesses;
  pri = processId * tamLocal;

  if (processId == noProcesses-1)
    qtde = N - pri;
  else
    qtde = floor((double) N/noProcesses);

  printf("Processo %d tem bloco tam = %d\n", processId, qtde);
  for(i = processId * qtde; i < (processId + 1) * qtde; i++) {
    //printf("process id eh %d e i eh %d\n", processId, i);
    x = 1 + ((double) i * passo);
    soma_local[processId] += 0.5*(1/x+1/(x + passo));
  }
  soma_local[processId] *= passo;
  printf("Processo %d tem soma_local = %lf\n", processId, soma_local[processId]);

  if (processId != 0){ /*Processos Escravos*/
    MPI_Send(&soma_local[processId], 1, MPI_DOUBLE, 0, 12, MPI_COMM_WORLD);
  }
  else { /*Processo Mestre*/
    for(i = 1; i < noProcesses; i++) {
      MPI_Recv(&soma_local[i], 1, MPI_DOUBLE, i,
         12, MPI_COMM_WORLD, &status);
    }
  }

  if (processId == 0){
    for (i = 0; i < noProcesses; i++)
      soma_global += soma_local[i];
    gettimeofday(&fim_procs, NULL);
    tmili = (double)(1000.0*(fim_procs.tv_sec-inicio_procs.tv_sec)+(fim_procs.tv_usec-inicio_procs.tv_usec)/1000.0);
    printf("Ln(2.0) = %20.15lf\nCom %d processos tempo gasto = %d milis\n", soma_global, noProcesses, tmili);
  }
	
	MPI_Finalize();

	return 0; 
}
