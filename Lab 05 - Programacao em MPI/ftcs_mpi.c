#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#include <time.h>
#include <sys/time.h>

#define tam 1.0
#define dx 0.00001
#define dt 0.000001
#define T  0.01
#define kappa 0.000045

void main(int argc, char **argv) {
    double *tmp, *u, *u_prev;
    double x, t;
    long int i, n, maxloc;
    int processId; /* rank dos processos */
    int noProcesses; /* Número de processos */
    int nameSize; /* Tamanho do nome */
    int ierr, qtd, tamLocal, pri, tmili; /*variavel para barreira*/
    char computerName[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

    /* Claculando quantidade de pontos */
    n = tam/dx;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Get_processor_name(computerName, &nameSize);

    struct timeval inicio_procs, fim_procs;

    gettimeofday(&inicio_procs, NULL);

    tamLocal = n / noProcesses;
    pri = processId * tamLocal;

    if (processId == noProcesses-1)
        qtd = n - pri;
    else
        qtd = floor((double) n / noProcesses);

    /* Alocando vetores */
    u = (double *) malloc((n+1)*sizeof(double));
    u_prev = (double *) malloc((n+1)*sizeof(double));

    printf("Inicio: qtde=%ld, dt=%g, dx=%g, dx²=%g, kappa=%f, const=%f\n",
        (n+1), dt, dx, dx*dx, kappa, kappa*dt/(dx*dx));
    printf("Iteracoes previstas: %g\n", T/dt);

    x = 0;
    for (i = processId * qtd; i < (processId + 1) * qtd; i++) {
        if (x<=0.5) u_prev[i] = 200*x;
        else        u_prev[i] = 200*(1.-x);
        x = (i + 1) * dx;
    }

    printf("dx=%g, x=%g, x-dx=%g\n", dx, x, x-dx);
    printf("u_prev[0,1]=%g, %g\n",u_prev[0],u_prev[1]);
    printf("u_prev[n-1,n]=%g, %g\n",u_prev[n-1],u_prev[n]);

    ierr = MPI_Barrier(MPI_COMM_WORLD);
    t = 0.;

    while (t<T) {
        x = dx;
        for (i = processId * qtd; i < (processId + 1) * qtd; i++) {
            u[i] = u_prev[i] + kappa*dt/(dx*dx)*(u_prev[i-1]-2*u_prev[i]+u_prev[i+1]);
            x = (i + 1)*dx;
        }
        u[0] = u[n] = 0.; /* forca condicao de contorno */
        tmp = u_prev; u_prev = u; u = tmp; /* troca entre ponteiros */
        t = (i + 1)*dt;
    }

    /* Calculando o maior valor e sua localizacao */
    maxloc = 0;
    for (i=1; i<n+1; i++) {
        if (u[i] > u[maxloc]) maxloc = i;
    }

    MPI_Finalize();

    gettimeofday(&fim_procs, NULL);
    tmili = (double)(1000.0*(fim_procs.tv_sec-inicio_procs.tv_sec)+(fim_procs.tv_usec-inicio_procs.tv_usec)/1000.0);
    printf("Com %d processos tempo gasto = %d milis\n", noProcesses, tmili);
    printf("Maior valor u[%ld] = %g\n", maxloc, u[maxloc]);
}
