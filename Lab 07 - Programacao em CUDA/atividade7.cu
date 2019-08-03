// ATIVIDADE 7 - PROGRAMACAO EM CUDA
// Ana Julia de Oliveira Bellini - RA 111.774
// Willian Dihanster Gomes de Oliveira - RA 112.269

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <sys/time.h>

#define tam 1.0
#define dx 0.00001
#define dt 0.000001
#define T 0.01
#define kappa 0.000045
#define THREADS_PER_BLOCK 512

typedef long long int nint;

__global__ void calcular(double *u, double *prev, double k, double t, double x, int n) {
    nint i =  blockDim.x * blockIdx.x + threadIdx.x;
    if (i > 0 && i < (n - 1))
        u[i] = prev[i] + k * t / (x * x) * (prev[i-1] - 2 * prev[i] + prev[i+1]);
}

__global__ void contorno(double *u, int n) {
    nint i =  blockDim.x * blockIdx.x + threadIdx.x;
    if(i == 1) u[0] = u[n] = 0.0;
}

int main(void) {
  double *tmp, *u, *u_prev;
  double x = dx, t;
  nint i, n, maxloc, size, num_blocos;
  double *d_u, *d_u_prev;

  /* Calculando quantidade de pontos */
  n = tam / dx;
  num_blocos = ceil((n + 1) / THREADS_PER_BLOCK);

  /* Alocando vetores */
  u = (double *) malloc((n + 1) * sizeof(double));
  u_prev = (double *) malloc((n + 1) * sizeof(double));

  size = (n + 1) * sizeof(nint);
  cudaMalloc((void **) &d_u, size);
  cudaMalloc((void **) &d_u_prev, size);

  printf("Inicio: qtde=%ld, dt=%g, dx=%g, dx²=%g, kappa=%f, const=%f\n",
	 (n+1), dt, dx, dx*dx, kappa, kappa*dt/(dx*dx));
  printf("Iteracoes previstas: %g\n", T/dt);

  x = 0;
  for (i = 0; i < n + 1; i++) {
      if (x <= 0.5)
          u_prev[i] = 200 * x;
      else
          u_prev[i] = 200 * (1.-x);
      x += dx;
  }

  printf("dx=%g, x=%g, x-dx=%g\n", dx, x, x-dx);
  printf("u_prev[0,1]=%g, %g\n",u_prev[0],u_prev[1]);
  printf("u_prev[n-1,n]=%g, %g\n",u_prev[n-1],u_prev[n]);

  cudaMemcpy(d_u_prev, u_prev, size, cudaMemcpyHostToDevice);

  t = 0.;
  while (t < T) {
    x = dx;

    /* Chama a funcao de que sera realizada no device */
    calcular <<<num_blocos, THREADS_PER_BLOCK>>> (d_u, d_u_prev, kappa, dt, dx, n + 1);

    /* Espera que toda threads terminem os calculos */
    cudaDeviceSynchronize();

    /* Agora deve calcular a condicao de contorno */
    contorno <<<num_blocos, THREADS_PER_BLOCK>>> (d_u, n);

    /* Troca entre ponteiros */
    tmp = d_u_prev; d_u_prev = d_u; d_u = tmp;

    t += dt;
  }

  cudaMemcpy(u, d_u, size, cudaMemcpyDeviceToHost);

  /* Calculando o maior valor e sua localizacao */
  maxloc = 0;
  for (i = 1; i < n + 1; i++)
      if (u[i] > u[maxloc]) maxloc = i;

  printf("Maior valor u[%ld] = %g\n", maxloc, u[maxloc]);

  /* Liberando a memeoria do host */
  free(u);
  free(u_prev);
  u = NULL;
  u_prev = NULL;

  /* Liberando a memeoria do device */
  cudaFree(d_u);
  cudaFree(d_u_prev);

  return 0;
}
