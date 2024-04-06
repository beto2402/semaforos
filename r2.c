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
#include <errno.h>
#include <sys/sem.h>


int main(){
	// Memoria de matriz 
    key_t keyMatriz = ftok(".",'a');
    int shmidMatriz = shmget(keyMatriz, ROWS*COLS*sizeof(int), 0777);
	int (*MATRIX) = (int*)shmat(shmidMatriz,NULL, 0);

    // Memoria de semáforo 
    int semid_3;
    key_t keySem = ftok(".", 2);
    semid_3 = semget(keySem, 1, IPC_CREAT | 0644);

	// Memoria de resultados 
	int *RESULTADO;
    key_t keyResultados = ftok(".",'d');
    int shmidResultados = shmget(keyResultados, sizeof(int)*ROWS, IPC_CREAT| 0777);
    RESULTADO= (int*)shmat(shmidResultados, NULL, 0);

	while(1){
	printf("Esperando proceso padre\n");
		sleep(2);
		printf("%d\n", semctl(semid_3, 0, GETVAL, 0));
		if(semctl(semid_3, 0, GETVAL, 0) == 1){break;}
	}

	// Inicializar primer valor para evitar multiplicación por 0
	RESULTADO[2] = MATRIX[18];

	printf("Iniciando multiplicación\n");
	for (int i = 19; i < COLS*3; i++) {
		RESULTADO[2] *= MATRIX[i];
	}

	printf("\nLa multiplicación de la tercera fila es %d \n", RESULTADO[2]);
	semctl(semid_3, 0, SETVAL, 0);

    return 0;
}