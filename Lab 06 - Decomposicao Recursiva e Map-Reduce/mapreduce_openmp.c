#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N_THREADS 2

int tamanho_arquivo = 0, ultimo_codigo = 0, *contadores, *codigos;

/*MAP*/
void Mapper(FILE *fp) {
	int i;

	#pragma omp parallel for num_threads(N_THREADS) shared(codigos, ultimo_codigo)
	for (i = 0; i < tamanho_arquivo; i++) {
		// printf("Estou no Mapper e sou a Thread %d, mexendo em %d\n", omp_get_thread_num(), i);
		fscanf(fp, "%d\n", &codigos[i]);
		if (codigos[i] > ultimo_codigo) ultimo_codigo = codigos[i];
	}
}

/*REDUCE*/
void Reducer() {
	int i;
	contadores = (int*)malloc(sizeof(int) * ultimo_codigo + 1);

	#pragma omp parallel for num_threads(N_THREADS)
	for (i = 0; i < ultimo_codigo + 1; i++) {
		contadores[i] = 0;
		// printf("Estou no Reducer, sou a Thread %d e inicializei %d\n", omp_get_thread_num(), i);
	}

	#pragma omp parallel for num_threads(N_THREADS)
	for (i = 0; i < tamanho_arquivo; i++) {
		contadores[codigos[i]]++;
		// printf("Estou no Reducer, sou a Thread %d. Incrementei o contador de %d para %d\n",
		// omp_get_thread_num(), codigos[i], contadores[codigos[i]]);
	}
}

/*FUNCAO PRINCIPAL*/
int main(int argc, char **argv) {
	int i;
	char aux;
	FILE *fp;
	double inicioExecucao, fimExecucao;

	// abrir arquivo "data.txt"
	if ((fp = fopen("data.txt","r")) == NULL) {
		printf("Erro ao abrir arquivo.\n\n");
		exit(1);
	}

	// contar numero de linhas do arquivo, para criar vetor de codigos
	while (!feof(fp)) {
		aux = fgetc(fp);
		if (aux == '\n')
			tamanho_arquivo++;
	}

	tamanho_arquivo--;

	codigos = (int*)malloc(sizeof(int) * tamanho_arquivo);

	fseek(fp, 0, SEEK_SET);

	inicioExecucao = omp_get_wtime();
	Mapper(fp);
	Reducer();
	fimExecucao = omp_get_wtime();

	// impressao dos resultados
	for (i = 0; i < ultimo_codigo + 1; i++)
		if (contadores[i] != 0) printf("codigo %d: %d ocorrencias\n", i, contadores[i]);

	double tempoTotal = fimExecucao - inicioExecucao;
	printf("\nTempo de Execução: %lf\n", tempoTotal);
}
