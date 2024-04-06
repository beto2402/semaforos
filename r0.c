#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "vars.h"
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
	int (*MATRIX) = (int*)shmat(shmidMatriz, NULL, 0);

    // Memoria de semáforo 
    int semid;
    key_t keySem = ftok(".", 0);
    semid = semget(keySem, 1, IPC_CREAT | 0644);

	// Memoria de resultados 
	int *RESULTADO;
    key_t keyResultados = ftok(".",'d');
    int shmidResultados = shmget(keyResultados, sizeof(int)*ROWS, IPC_CREAT| 0777);
    RESULTADO= (int*)shmat(shmidResultados, NULL, 0);
	
	while(1) {
		printf("Esperando proceso padre\n");
		sleep(2);
		if(semctl(semid, 0, GETVAL, 0) == 1){break;}
	}

	// Inicializar primer valor para evitar multiplicación por 0
	RESULTADO[0] = MATRIX[0];

	printf("Iniciando multiplicación\n");
	for (int i = 1; i < COLS; i++) {
		RESULTADO[0] *= MATRIX[i];
	}

	printf("\nLa multiplicación de la primer fila es %d \n", RESULTADO[0]);
	semctl(semid, 0, SETVAL, 0);

    return 0;
}