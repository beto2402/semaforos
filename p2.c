#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "globals.h"
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/sem.h>


int main(){
	// Memoria de matriz 
    key_t keyMatriz = ftok(".",'a');
    int shmidMatriz = shmget(keyMatriz, ROWS*COLS*sizeof(int), 0777);
	int (*MATRIZ) = (int*) shmat(shmidMatriz, NULL, 0);

    // Memoria de semáforo 
    int semid;
    key_t keySem = ftok(".", 'b');
    semid = semget(keySem, 1, IPC_CREAT | 0644);
	semctl(semid, 0, SETVAL, 0);

	// Memoria de resultados 
	int *RESULTADOS;
    key_t keyResultados = ftok(".",'d');
    int shmidResultados = shmget(keyResultados, sizeof(int)*ROWS, IPC_CREAT| 0777);
    RESULTADOS= (int*) shmat(shmidResultados, NULL, 0);
	
	while(1){
		printf("Esperando proceso padre\n");
		sleep(2);
		if(semid == 1){break;}
	}

	for (int i = 1; i < COLS; i++) {
		RESULTADOS[0] *= MATRIZ[i];
	}
	printf("Iniciando multiplicación\n");
	// Inicializar primer valor para evitar multiplicación por 0
	RESULTADOS[0] = MATRIZ[0];

	for (int i = 1; i < COLS; i++) {
		RESULTADOS[0] *= MATRIZ[i];
	}

	semid = 2;
	printf("\nLa suma de la primer fila es %d \n", RESULTADOS[0]);
    return 0;
}