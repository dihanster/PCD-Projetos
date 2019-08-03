#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define N 10000000 //usar 10^5 e 10^7
#define MAX_THREADS 2 //usar 1, 2, 4 e 8

double *r1, *r2, *teta1, *teta2, *d, *retorno_max;

/*Funcao para calcular a distancia dos pontos de um bloco do vetor atribuido a uma thread*/
void *calcula_distancia(void *tid) {

    long i, thid;

    thid = (long) tid;

    for (i = thid; i < ((1 + thid) * (N/MAX_THREADS)); i++) {
        d[i] = sqrt(r1[i]*r1[i] + r2[i]*r2[i] - 2 * r1[i] * r2[i] * cos((teta1[i] - teta2[i])));
    }

    pthread_exit(0);

}

/*Funcao para achar a maior distancia local de uma dada thread*/
void *acha_max_distancia(void *tid) {
  
    long i, thid;
    double max_loc;

    thid = (long) tid;
    max_loc = d[thid];

    //for(i = thid; i < N; i += MAX_THREADS)
    for (i = thid; i < ((1 + thid) * (N/MAX_THREADS)); i++)
        if (d[i] > max_loc) 
            max_loc = d[i];

    //printf("Thread = %ld tem maximo local = %f\n", thid, max_loc);

    retorno_max[thid] = max_loc;

    pthread_exit(0);

} 

int main (){
    
    /*Declaracao das variaveis loccais*/
    int tmili, tmili1, tmili2;
    long th;
    double max_global;
    struct timeval inicio_tudo, inicio_relativo, final_tudo, final_relativo, inicio_max, final_max;
    
    gettimeofday(&inicio_tudo, NULL);
    pthread_t t[MAX_THREADS];

    /*Alocacao dinamica dos vetores*/
    r1 = (double*) malloc (N * sizeof(double));
    r2 = (double*) malloc (N * sizeof(double));
    teta1 = (double*) malloc (N * sizeof(double));
    teta2 = (double*) malloc (N * sizeof(double));
    d = (double*) malloc (N * sizeof(double));
    retorno_max = (double*) malloc (N * sizeof(double));

    /*Inicialização dos vetores*/
    /*Vetores r1 e r2 variam de 0 a 100 angulo de 0o a 259o*/
    //srand(time(NULL));
    printf("Iniciando os vetores de pontos...\n");
    for (int i = 0; i < N; i++) {
        double n1 =  0 + (rand() % 100);
        double n2 = 0 + (rand() % 100);
        double n3 = 0 + (rand() % 360);
        double n4 = 0 + (rand() % 360);
        r1[i] = n1;
        r2[i] = n2;
        teta1[i] = n3;
        teta2[i] = n4;
        //printf("\nPonto %d eh r1:%lf eh teta1: %lf\n", i, r1[i], teta1[i]);
        //printf("\nPonto %d eh r2:%lf eh teta2: %lf\n", i, r2[i], teta2[i]);
    }

    /*for (int i = 0; i < N; i++) {
        d[i] = sqrt( (r1[i]*r1[i]) + (r2[i]*r2[i]) - (2 * r1[i] * r2[i] * cos((teta1[i] - teta2[i]))));
        printf("id da thread eh %d e o di eh %lf \n", i, d[i]);
    }*/


    /*Calculo das distancias dos pontos*/
    printf("Iniciando o calculo das distancias\n");
    gettimeofday(&inicio_relativo, NULL);

    for (th = 0; th < MAX_THREADS; th++) {
        pthread_create(&t[th], NULL, calcula_distancia, (void *) th);
    }

    for(th = 0; th < MAX_THREADS; th++) {
        pthread_join(t[th],NULL);
    }

    gettimeofday(&final_relativo, NULL);
    tmili = (int) (1000 * (final_relativo.tv_sec - inicio_relativo.tv_sec) + (final_relativo.tv_usec - inicio_relativo.tv_usec) / 1000);
  
    printf("Calculos terminados! :D\n");
    printf("Tempo decorrido com o trecho de calculo das distancias: %d milisegundos\n", tmili);

    
    /*
    for (int i = 0; i < N; i++){
        printf("Distancia dos pontos %d eh %lf \n", i, d[i]);
    }
    */

    /*Calculo da distancia maxima*/
    gettimeofday(&inicio_max, NULL);
  
    for(th = 0; th < MAX_THREADS; th++) {
        pthread_create(&t[th], NULL, acha_max_distancia, (void *) th);
    }

    max_global = d[0];
    for(th = 0; th < MAX_THREADS; th++) {
        pthread_join(t[th], NULL);
        if (retorno_max[th] > max_global) 
            max_global = retorno_max[th];
    }

    gettimeofday(&final_max, NULL);
    tmili2 = (int) (1000 * (final_max.tv_sec - inicio_max.tv_sec) + (final_max.tv_usec - inicio_max.tv_usec) / 1000);
  
    printf("Tempo decorrido da funcao de achar a maxima distancia: %d milisegundos\n", tmili2);
    printf("Maior valor de distancia = %f\n", max_global);

    /*Calculando o tempo total do programa*/
    gettimeofday(&final_tudo, NULL);
    tmili1 = (int) (1000 * (final_tudo.tv_sec - inicio_tudo.tv_sec) + (final_tudo.tv_usec - inicio_tudo.tv_usec) / 1000);
  
    printf("Tempo decorrido com o programa todo: %d milisegundos\n", tmili1);
    
    printf("Terminado :D\n");

    return 0;
}
