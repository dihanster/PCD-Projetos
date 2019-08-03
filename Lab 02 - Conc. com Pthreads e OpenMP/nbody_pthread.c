#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>
#include <pthread.h>
/*
 * pRNG based on http://www.cs.wm.edu/~va/software/park/park.html
 */
#define MODULUS    2147483647
#define MULTIPLIER 48271
#define DEFAULT    123456789
#define N_THREAD 8

static long seed = DEFAULT;

double Random(void)
/* ----------------------------------------------------------------
 * Random returns a pseudo-random real number uniformly distributed
 * between 0.0 and 1.0.
 * ----------------------------------------------------------------
 */
{
  const long Q = MODULUS / MULTIPLIER;
  const long R = MODULUS % MULTIPLIER;
  long t;

  t = MULTIPLIER * (seed % Q) - R * (seed / Q);
  if (t > 0)
    seed = t;
  else
    seed = t + MODULUS;
  return ((double) seed / MODULUS);
}

/*
 * End of the pRNG algorithm
 */

typedef struct {
  double x, y, z;
  double mass;
} Particle;
typedef struct {
  double xold, yold, zold;
  double fx, fy, fz;
} ParticleV;

double result_force[N_THREAD];
double result_pos[N_THREAD];
double N = 25000;
double max_f;

Particle  *particles;   /* Particulas */
ParticleV *pv;          /* Velocidade da Particula */

void InitParticles( Particle[], ParticleV [], int );
void *ComputeForces(void*);
void *ComputeNewPos(void*);

int main()
{
  //double time;
  //Particle  * particles;   /* Particulas */
  //ParticleV * pv;          /* Velocidade da Particula */
  int         npart;
  double      sim_t;       /* Tempo de Simulacao */
  pthread_t t[N_THREAD];

  scanf("%d",&npart);
  //npart = 25000;
  /* Allocate memory for particles */
  particles = (Particle *) malloc(sizeof(Particle)*npart);
  pv = (ParticleV *) malloc(sizeof(ParticleV)*npart);
  /* Generate the initial values */
  InitParticles( particles, pv, npart);

  sim_t = 0.0;

  struct timeval inicio, final2;
  int tmili;

  int tid, i;

  gettimeofday(&inicio, NULL);

  long th;
  /* Compute forces (2D only) */
  for(th = 0; th < N_THREAD; th++) {
        pthread_create(&t[th], NULL, ComputeForces, (void *) th);
  }

  for(th = 0; th < N_THREAD; th++) {
      pthread_join(t[th],NULL);
  }

  double max_global = result_force[0];

  for (i = 0; i < N_THREAD; i++){
    if (result_force[i] > max_global)
      max_global = result_force[i];
  }

  max_f = max_global;

    
  /* Once we have the forces, we compute the changes in position */
  //sim_t = ComputeNewPos(particles, pv, npart, max_f);

  for(th = 0; th < N_THREAD; th++) {
        pthread_create(&t[th], NULL, ComputeNewPos, (void *) th);
  }

  for(th = 0; th < N_THREAD; th++) {
      pthread_join(t[th],NULL);
  }

  for (int i = 0; i < N_THREAD; i++){
    printf("eh %lf\n", result_pos[i]);
  }
  
  sim_t = result_pos[0];

  gettimeofday(&final2, NULL);
  tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) + 
		 (final2.tv_usec - inicio.tv_usec) / 1000);

  //for (i=0; i<npart; i++)
  //fprintf(stdout,"%.5lf %.5lf %.5lf\n", particles[i].x, particles[i].y, particles[i].z);

  printf("%g\n", max_f);
  printf("%g\n", sim_t);

  printf("%d\n", tmili);

  return 0;
}

void InitParticles( Particle particles[], ParticleV pv[], int npart )
{
  int i;
  for (i=0; i<npart; i++) {
    particles[i].x	  = Random();
    particles[i].y	  = Random();
    particles[i].z	  = Random();
    particles[i].mass = 1.0;
    pv[i].xold	  = particles[i].x;
    pv[i].yold	  = particles[i].y;
    pv[i].zold	  = particles[i].z;
    pv[i].fx	  = 0;
    pv[i].fy	  = 0;
    pv[i].fz	  = 0;
  }
}

void *ComputeForces(void *tid){
  double max_f;
  int i;
  max_f = 0.0;
  long thid = (long) tid;

  int j;
  double xi, yi, rx, ry, mj, r, fx, fy, rmin;

  for (i = thid; i < ((1 + thid) * (N/N_THREAD)); i++){
    rmin = 100.0;
    xi   = particles[i].x;
    yi   = particles[i].y;
    fx   = 0.0;
    fy   = 0.0;
    for (j = thid; j < ((1 + thid) * (N/N_THREAD)); j++){
      rx = xi - particles[j].x;
      ry = yi - particles[j].y;
      mj = particles[j].mass;
      r  = rx * rx + ry * ry;
      /* ignore overlap and same particle */
      if (r == 0.0) continue;
      if (r < rmin) rmin = r;
      r  = r * sqrt(r);
      fx -= mj * rx / r;
      fy -= mj * ry / r;
    }
    pv[i].fx += fx;
    pv[i].fy += fy;
    fx = sqrt(fx*fx + fy*fy)/rmin;
    if (fx > max_f) max_f = fx;
  }
  result_force[thid] = max_f;
  //return max_f;
}

void *ComputeNewPos(void *tid){
  int i;
  double a0, a1, a2;
  static double dt_old = 0.001, dt = 0.001;
  double dt_new;
  a0	 = 2.0 / (dt * (dt + dt_old));
  a2	 = 2.0 / (dt_old * (dt + dt_old));
  a1	 = -(a0 + a2);

  long thid = (long) tid;

  for (i = thid; i < ((1 + thid) * (N/N_THREAD)); i++){
    double xi, yi;
    xi	           = particles[i].x;
    yi	           = particles[i].y;
    particles[i].x = (pv[i].fx - a1 * xi - a2 * pv[i].xold) / a0;
    particles[i].y = (pv[i].fy - a1 * yi - a2 * pv[i].yold) / a0;
    pv[i].xold     = xi;
    pv[i].yold     = yi;
    pv[i].fx       = 0;
    pv[i].fy       = 0;
  }
  dt_new = 1.0/sqrt(max_f);
  /* Set a minimum: */
  if (dt_new < 1.0e-6) dt_new = 1.0e-6;
  /* Modify time step */
  if (dt_new < dt) {
    dt_old = dt;
    dt     = dt_new;
  }
  else if (dt_new > 4.0 * dt) {
    dt_old = dt;
    dt    *= 2.0;
  }
  result_force[thid] = dt_old;
  //return dt_old;
}

