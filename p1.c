#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "globals.h"
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/sem.h>
	
int main(){
    // Memoria de matriz 
    int *MATRIZ;
    key_t keyMatriz = ftok(".", 'a');
    int shmidMatriz = shmget(keyMatriz, ROWS*COLS*sizeof(int), IPC_CREAT | 0777);
    MATRIZ = (int*) shmat(shmidMatriz,NULL,0);

    // Memoria de semáforo 
    int semid;
    key_t keySem = ftok(".", 'b');
    semid = semget(keySem, 1, IPC_CREAT | 0644);
    semctl(semid, 0, SETVAL, 0);

    // Memoria de resultados 
    int *RESULTADOS;
    key_t keyResultados = ftok(".", 'd');
    int shmidResultados = shmget(keyResultados, sizeof(int)*ROWS, IPC_CREAT | 0777);
    RESULTADOS = (int*) shmat(shmidResultados,NULL,0);

    printf("Iniciando proceso padre \n");

    for (int i = 0; i < ROWS * COLS; i++) {
        MATRIZ[i] = values[i / COLS][i % COLS];
    }
    printf("... Terminando de guardar matriz \n");

    semctl(semid, 0, SETVAL, 1);

    while (semctl(semid, 0, GETVAL, 0) == 1) {
        printf("Esperando suma 1...\n");
        sleep(2);
    }
    printf("\nLa suma de la primer fila es %d \n", RESULTADOS[0]);

    semctl(semid, 0, SETVAL, 1);

    while (semctl(semid, 0, GETVAL, 0) == 1) {
        printf("Esperando suma 2...\n");
        sleep(2);
    }
    printf("\nLa suma de la segunda fila es %d \n", RESULTADOS[1]);

    semctl(semid, 0, SETVAL, 1);

    while (semctl(semid, 0, GETVAL, 0) == 1) {
        printf("Esperando suma 3...\n");
        sleep(2);
    }
    printf("\nLa suma de la tercera fila es %d \n", RESULTADOS[2]);

    printf("\n\nLos resultados son: (%d, %d, %d) \n", RESULTADOS[0], RESULTADOS[1], RESULTADOS[2]);

    shmdt(MATRIZ);
    shmdt(RESULTADOS);

    shmctl(shmidMatriz, IPC_RMID, 0);
    shmctl(shmidResultados, IPC_RMID, 0);
    semctl(semid, 0, IPC_RMID, 0);

    return 0;
}
