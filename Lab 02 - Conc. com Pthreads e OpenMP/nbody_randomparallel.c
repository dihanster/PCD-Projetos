#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

/*
 * pRNG based on http://www.cs.wm.edu/~va/software/park/park.html
 */
#define MODULUS    2147483647
#define MULTIPLIER 48271
#define DEFAULT    123456789
#define MAX_THREADS 1

static long seed = DEFAULT;
long *vetor_seed;

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
    int i = omp_get_thread_num();

    t = MULTIPLIER * (vetor_seed[i] % Q) - R * (vetor_seed[i] / Q);
    if (t > 0)
        vetor_seed[i] = t;
    else
        vetor_seed[i] = t + MODULUS;
    return ((double) vetor_seed[i] / MODULUS);
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

void InitParticles( Particle[], ParticleV [], int );
double ComputeForces( Particle [], Particle [], ParticleV [], int );
double ComputeNewPos( Particle [], ParticleV [], int, double);

int main()
{
    struct timeval inicioTudo, finalTudo, inicio, final2;;
    gettimeofday(&inicioTudo, NULL);

    //double time;
    Particle  * particles;   /* Particulas */
    ParticleV * pv;          /* Velocidade da Particula */
    int         i, npart, tmili, tmili2, tid;
    double      sim_t;       /* Tempo de Simulacao */
    double max_f;

    sim_t = 0.0;

    /*Inicia o vetor de sementes*/
    vetor_seed = (long*) malloc(MAX_THREADS * sizeof(long));
    for(i = 0; i < MAX_THREADS; i++){
        vetor_seed[i] = DEFAULT;
    }

    //scanf("%d",&npart);
    npart = 25000;
    /* Allocate memory for particles */
    particles = (Particle *) malloc(sizeof(Particle)*npart);
    pv = (ParticleV *) malloc(sizeof(ParticleV)*npart);

    gettimeofday(&inicio, NULL);

    /* Generate the initial values */
    InitParticles( particles, pv, npart);

    /* Compute forces (2D only) */
    max_f = ComputeForces(particles, particles, pv, npart);
    
    /* Once we have the forces, we compute the changes in position */
    sim_t = ComputeNewPos(particles, pv, npart, max_f);

    gettimeofday(&final2, NULL);
    tmili = (int) (1000 * (final2.tv_sec - inicio.tv_sec) +
                   (final2.tv_usec - inicio.tv_usec) / 1000);

    //for (i=0; i<npart; i++)
    //fprintf(stdout,"%.5lf %.5lf %.5lf\n", particles[i].x, particles[i].y, particles[i].z);

    printf("%g\n", max_f);
    printf("%g\n", sim_t);

    printf("Paralelo: %d\n", tmili);

    gettimeofday(&finalTudo, NULL);
    tmili2 = (int) (1000 * (finalTudo.tv_sec - inicioTudo.tv_sec) +
                   (finalTudo.tv_usec - inicioTudo.tv_usec) / 1000);
    
    printf("Tudo: %d\n", tmili2);

    return 0;
}

void InitParticles( Particle particles[], ParticleV pv[], int npart )
{
    int i;
    #pragma omp parallel for private(i) num_threads(MAX_THREADS)
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

double ComputeForces( Particle myparticles[], Particle others[], ParticleV pv[], int npart )
{
    double max_f;
    int i;
    max_f = 0.0;

    int j;
    double xi, yi, rx, ry, mj, r, fx, fy, rmin;

    #pragma omp parallel for private(xi, yi, rx, ry, mj, r, rmin) reduction(-:fx, fy) num_threads(MAX_THREADS)
    for (i=0; i<npart; i++) {
        rmin = 100.0;
        xi   = myparticles[i].x;
        yi   = myparticles[i].y;
        fx   = 0.0;
        fy   = 0.0;
        for (j=0; j<npart; j++) {
            rx = xi - others[j].x;
            ry = yi - others[j].y;
            mj = others[j].mass;
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
    return max_f;
}

double ComputeNewPos( Particle particles[], ParticleV pv[], int npart, double max_f)
{
    int i;
    double a0, a1, a2;
    static double dt_old = 0.001, dt = 0.001;
    double dt_new;
    a0	 = 2.0 / (dt * (dt + dt_old));
    a2	 = 2.0 / (dt_old * (dt + dt_old));
    a1	 = -(a0 + a2);

    double xi, yi;
    #pragma omp parallel for private(xi, yi, i) num_threads(MAX_THREADS)
    for (i=0; i<npart; i++) {
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
    return dt_old;
}

