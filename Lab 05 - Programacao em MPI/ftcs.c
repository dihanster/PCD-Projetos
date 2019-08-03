#include <stdio.h>
#include <stdlib.h>

#define tam 1.0
#define dx 0.00001
#define dt 0.000001
#define T  0.01
#define kappa 0.000045

void main(void) {

  double *tmp, *u, *u_prev;
  double x, t;
  long int i, n, maxloc;

  /* Claculando quantidade de pontos */
  n = tam/dx;
  
  /* Alocando vetores */
  u = (double *) malloc((n+1)*sizeof(double));
  u_prev = (double *) malloc((n+1)*sizeof(double));

  printf("Inicio: qtde=%ld, dt=%g, dx=%g, dx²=%g, kappa=%f, const=%f\n", 
	 (n+1), dt, dx, dx*dx, kappa, kappa*dt/(dx*dx));
  printf("Iteracoes previstas: %g\n", T/dt);

  x = 0;
  for (i=0; i<n+1; i++) {
    if (x<=0.5) u_prev[i] = 200*x;
    else        u_prev[i] = 200*(1.-x);
    x += dx;
  }

  printf("dx=%g, x=%g, x-dx=%g\n", dx, x, x-dx);
  printf("u_prev[0,1]=%g, %g\n",u_prev[0],u_prev[1]);
  printf("u_prev[n-1,n]=%g, %g\n",u_prev[n-1],u_prev[n]);

  t = 0.;
  while (t<T) {
    x = dx;
    for (i=1; i<n; i++) {
      u[i] = u_prev[i] + kappa*dt/(dx*dx)*(u_prev[i-1]-2*u_prev[i]+u_prev[i+1]);
      x += dx;
    }
    u[0] = u[n] = 0.; /* forca condicao de contorno */
    tmp = u_prev; u_prev = u; u = tmp; /* troca entre ponteiros */
    t += dt;
  }

  /* Calculando o maior valor e sua localizacao */
  maxloc = 0;
  for (i=1; i<n+1; i++) {
    if (u[i] > u[maxloc]) maxloc = i;
  }
  printf("Maior valor u[%ld] = %g\n", maxloc, u[maxloc]);

}
