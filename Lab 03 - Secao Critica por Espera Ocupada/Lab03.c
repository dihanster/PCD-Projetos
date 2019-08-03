#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N_CLI 4

long solicitar = -1, resposta = -1;
int cont = 1000000000;

void *cliente(void *tid) {
    int thid = *(int*)tid;
    while (1) {
        while (resposta != thid) {
            solicitar = thid;
        }
        cont = cont + 1;
        printf("Thread Cliente ID: %d, Contador = %d\n", thid, cont);
        //sleep(3);
        resposta = -1;
    }
}

void *servidor(void *tid) {
    long thid = *(long*)tid;
    printf("Thread Servidor ID: %ld\n", thid);
    while (1) {
        while (solicitar == -1);
        resposta = solicitar;
        while (resposta != -1);
        solicitar = -1;
    }
}

int main(void) {
    pthread_t th_clientes[N_CLI], th_servidor;
    // long ID_Cliente[N_CLI] = {1, 2};
    long ID_Cliente[N_CLI] = {1, 2, 3, 4};
    long ID = 1;

    for (long cli = 0; cli < N_CLI; cli++)
        pthread_create(&th_clientes[cli], NULL, cliente, (void *)&ID_Cliente[cli]);

    pthread_create(&th_servidor, NULL, servidor, &ID);

    for (long cli = 0; cli < N_CLI; cli++)
        pthread_join(th_clientes[cli], NULL);

    pthread_join(th_servidor, NULL);

    return 0;
}
