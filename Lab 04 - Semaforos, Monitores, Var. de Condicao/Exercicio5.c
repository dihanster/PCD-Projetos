#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define N 100
#define TP 4
#define TC 2

sem_t fila_cheia, fila_vazia, mutex_produtor, mutex_consumidor;
pthread_barrier_t barreira;

int i, j, fila[N], cont_num = 0;

/*INICIALIZAR VARIAVEIS*/
void inicializarVariaveis(void) {
    sem_init(&fila_cheia, 0, 0);
    sem_init(&fila_vazia, 0, N);
    sem_init(&mutex_produtor, 0, 1);
    sem_init(&mutex_consumidor, 0, 1);
    pthread_barrier_init(&barreira, NULL, TP);
    i = 0; j = 0;
    srand(time(NULL));
}

/*THREADS TP*/
void *ThreadP(void *args) {
    int tid = *(int *)args;
    int num_random;
    printf("Sou o Produtor\n");

    while(1) {
        num_random = rand() % 10;
        cont_num++;
        if (cont_num > N) num_random = -1;
        if (cont_num > (N + TC)) exit(0);

        sem_wait(&fila_vazia);
        sem_wait(&mutex_produtor);

        // entra na regiao critica
        fila[i] = num_random;
        printf("Prod %d produziu fila[%d] = %d\n", tid, i, num_random);
        i = (i + 1) % N;
        // sai da regiao critica

        sem_post(&mutex_produtor);
        sem_post(&fila_cheia);
        pthread_barrier_wait(&barreira);
    }
}

/*THREADS TC*/
void *ThreadC(void *args) {
    int tid = *(int *)args;
    int num_verificar, primo = 1, d = 2;
    printf("Sou o Consumidor\n");

    while(1) {
        sem_wait(&fila_cheia);
        sem_wait(&mutex_consumidor);

        num_verificar = fila[j];
        primo = 1;
        d = 2;

        if (num_verificar < 0) {
            printf("Encerrando\n");
        }

        if (num_verificar <= 1)
            primo = 0;

        while (primo == 1 && d < num_verificar / 2) {
            if (num_verificar % d == 0) primo = 0;
            d = d + 1;
        }

        if (primo == 1) printf("Consumidor %d consumiu fila[%d] = %d e %d eh primo!\n", tid, j, num_verificar, num_verificar);
        else printf("Consumidor %d consumiu fila[%d] = %d e %d nao eh primo!\n", tid, j, num_verificar, num_verificar);

        j = (j + 1) % N;

        sem_post(&mutex_consumidor);
        sem_post(&fila_vazia);
    }
}

/*FUNÇÃO PRINCIPAL*/
int main(int argc, char *argv[]) {
    // declaração das threads
    pthread_t produtor[TP], consumidor[TC];
    inicializarVariaveis();
    int p, c, pid[TP], cid[TC];

    // criar e juntar threads
    for (p = 0; p < TP; p++) {
        pid[p] = p + 1;
        pthread_create(&produtor[p], NULL, (void*)ThreadP, &pid[p]);
    }

    for (c = 0; c < TC; c++) {
        cid[c] = c + 1;
        pthread_create(&consumidor[c], NULL, (void*)ThreadC, &cid[c]);
    }

    for (p = 0; p < TP; p++) {
        pthread_join(produtor[p], NULL);
    }

    for (c = 0; c < TC; c++) {
        pthread_join(consumidor[c], NULL);
    }

    return 0;
}
