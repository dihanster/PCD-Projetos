#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "omp.h"

#define N 10000000
#define N_THREADS 2

/*GERAR VALORES ALEATORIOS*/
void gerar_valores(double *valores) {
    srand(time(NULL));
    for (int i = 0; i < N; i++) 
        valores[i] = (double) (rand() % 100)/3.758;
}

/*IMPRIMIR LISTA DE VALORES*/
void imprimir_resultados(double *valores) {
    for (int i = 0; i < N; i++) 
        printf("%lf ", valores[i]);
}

/*ALGORITMO MERGE SORT*/
void merge(double *valores, int esq, int dir) {
    int i = 0;
    int meio = (esq + (dir - esq)/2);
    double *temp = (double*) malloc ((dir - esq + 1) * sizeof(double));
    int n1 = esq;
    int n2 = meio+1;

    while(n1 <= meio && n2 <= dir) {
        if (valores[n1] < valores[n2]) {
            temp[i] = valores[n1];
            i++;
            n1++;
        }
        else {
            temp[i] = valores[n2];
            i++;
            n2++;
        }
    }

    while(n1 <= meio) {
        temp[i] = valores[n1];
        i++;
        n1++;
    }

    while(n2 <= dir) {
        temp[i] = valores[n2];
        i++;
        n2++;
    }

    for(i = esq; i <= dir; i++) {
        valores[i] = temp[i - esq];
    }
}

void merge_sort(double *valores, int esq, int dir) {
    if (esq < dir){
        int meio = (esq + (dir-esq)/2);
        omp_set_num_threads(N_THREADS);
        #pragma omp parallel
        {
            #pragma omp single
            {
                #pragma omp task
                merge_sort(valores, esq, meio);

                #pragma omp task
                merge_sort(valores, meio+1, dir);
            }
        }
    }

   merge(valores, esq, dir);
}

/*FUNCAO PRINCIPAL*/
int main(int argc, char *argv[]) {
    // variaveis
    double *valores = (double*) malloc (N * sizeof(double));
    double inicioExecucao, fimExecucao;

    // gerar valores do vetor
    gerar_valores(valores);

    // imprimir lista antes da ordenacao
    printf("Antes:\n");
    //imprimir_resultados(valores);

    // iniciar ordenacao
    printf("Iniciando ordenacao...\n");
    inicioExecucao = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        merge_sort(valores, 0, N-1);
    }

    fimExecucao = omp_get_wtime();

    // imprimir lista depois da ordenacao
    printf("\nDepois:\n");
    //imprimir_resultados(valores);

    // retornar tempo e encerrar programa
    double tempoTotal = fimExecucao - inicioExecucao;
    printf("\nOrdenacao concluida. Tempo de Execução: %lf\n", tempoTotal);
    return 0;
}

