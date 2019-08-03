#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define SIZE 341425
#define QTD_C 1056397
#define MAX_CODE 1000

int main(int argc, char **argv){
	int data[QTD_C], i = 0, last_code = 0;
	int num = 0, cn, cont = 0, cont_n = 0, j = 0;
	int *counters, *counters_aux, bufsize;
	int processId; /* rank dos processos */
	int noProcesses; /* Número de processos */
  	int nameSize; /* Tamanho do nome */
  	char *buf, computerName[MPI_MAX_PROCESSOR_NAME];
  	double inicio, fim;
	MPI_File fh; 
	MPI_Status status;
	MPI_Offset filesize, globalend, globalstart;

	/*Inicializacao do OpenMPI*/
  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
  	MPI_Comm_rank(MPI_COMM_WORLD, &processId);
  	MPI_Get_processor_name(computerName, &nameSize);

  	/*Processo 0 printa o inicio*/
	if (processId == 0)
		printf("Inicializando Map-Reduce com %d processos...\n", noProcesses);

	/*Controle de Tempo*/
	inicio = MPI_Wtime();

	/*Faz a leitura do arquivo e pega seu tamanho*/
	MPI_File_open(MPI_COMM_WORLD, "data.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
	MPI_File_get_size (fh, &filesize);
	
	/*Faz as separação dos tamanho dos blocos, buffer...*/
	filesize = filesize/sizeof(char); 
	bufsize = filesize/noProcesses ; 
    buf = (char*) malloc (bufsize * sizeof (char));

    /*Desloca o offset do arquivo de acordo com o id do processo*/
	MPI_File_seek(fh, processId * bufsize * sizeof (char), MPI_SEEK_SET);
	MPI_File_read(fh, buf, bufsize, MPI_CHAR, &status);

	/*Inicia o vetor de dados*/
	for (i = 0; i < bufsize; i++){
		data[i] = 0; 
	}

	/*Transformar caractere lidos do buffer em numeros int para o vetor data*/
	for (i = 0; i < bufsize; i++){
		cont++;
		if (buf[i] != '\n'){
			cn = buf[i] % 48;
			num = (num * 10) +  cn;
		}
		else {
			data[j] = num;
			cont_n++;
			if (data[j] > last_code)
				last_code = data[j];
			num = 0;
			j++;
		}
	}

	/*Declara e inicia os vetores de counters e counters_aux para receber dos outros proc.*/
	counters = (int*) malloc (sizeof (int) * MAX_CODE);
	counters_aux = (int*) malloc (sizeof (int) * MAX_CODE);

	for(i = 0; i < MAX_CODE; i++)
		counters[i] = counters_aux[i] = 0;

	/*Fase MAP do Processo i*/
	for(i = 0; i < bufsize; i++){
		counters[data[i]]++;
	}

	/*Separacao das tarefas*/
	if (processId != 0){ /*Processos escravos*/
		MPI_Send(counters, MAX_CODE, MPI_INT, 0, 12, MPI_COMM_WORLD);
	}
	else { /*Processos mestres*/
		for (int i = 1; i < noProcesses; i++){
			MPI_Recv(counters_aux, MAX_CODE, MPI_INT, i, 12, MPI_COMM_WORLD, &status);
			for (j = 0; j < MAX_CODE; j++){
				counters[j] += counters_aux[j];
			}
		}
	}

	/*Processo 0 faz o REDUCE*/
	if (processId == 0){
		for(i = 0; i < MAX_CODE; i++)
				if(counters[i] != 0) 
					printf("codigo %d: %d ocorrencias\n", i, counters[i]);
		fim = MPI_Wtime();
		printf("Map-Reduce concluido. Tempo gasto %lf segundos\n", fim - inicio); 
	}

	MPI_Finalize();
}
