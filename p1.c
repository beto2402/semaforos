#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "vars.h"
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/sem.h>
	
int main(){
    // Memoria de matriz 
    int *MATRIX;
    key_t keyMatriz = ftok(".", 'a');
    int shmidMatriz = shmget(keyMatriz, ROWS*COLS*sizeof(int), IPC_CREAT | 0777);
    MATRIX = (int*)shmat(shmidMatriz, NULL, 0);

    // Memoria de semáforo 
    int semid_1;
    key_t keySem = ftok(".", '1');
    semid_1 = semget(keySem, 1, IPC_CREAT | 0644);
    semctl(semid_1, 0, SETVAL, 1);

    // Memoria de semáforo 
    int semid_2;
    key_t keySem2 = ftok(".", '2');
    semid_2 = semget(keySem2, 1, IPC_CREAT | 0644);
    semctl(semid_2, 0, SETVAL, 1);

    // Memoria de semáforo 
    int semid_3;
    key_t keySem3 = ftok(".", '3');
    semid_3 = semget(keySem3, 1, IPC_CREAT | 0644);
    semctl(semid_3, 0, SETVAL, 1);


    // Memoria de resultados 
    int *RESULTADO;
    key_t keyResultados = ftok(".", 'd');
    int shmidResultados = shmget(keyResultados, sizeof(int)*ROWS, IPC_CREAT | 0777);

    printf("Iniciando proceso padre \n");

    for (int i = 0; i < ROWS * COLS; i++) {
        MATRIX[i] = values[i / COLS][i % COLS];
    }
    printf("Matriz guardada \n");

    int procesosCompletos;

    do {
        procesosCompletos = 0;

        printf("Esperando proceso(s): [ ");


        if (semctl(semid_1, 0, GETVAL, 0) == 1) {
            printf("1, ");
        } else {
            procesosCompletos += 1;
        }

        if (semctl(semid_2, 0, GETVAL, 0) == 1) {
            printf("2, ");
        } else {
            procesosCompletos += 1;
        }

        if (semctl(semid_3, 0, GETVAL, 0) == 1) {
            printf("3, ");
        } else {
            procesosCompletos += 1;
        }

        printf("]\n");

        sleep(2);
    } while (procesosCompletos != 3);

    RESULTADO= (int*)shmat(shmidResultados, NULL, 0);
    printf("\n\nLos resultados son: \n(%d, %d, %d) \n", RESULTADO[0], RESULTADO[1], RESULTADO[2]);

    shmdt(MATRIX);
    shmdt(RESULTADO);

    shmctl(shmidMatriz, IPC_RMID, 0);
    shmctl(shmidResultados, IPC_RMID, 0);
    semctl(semid_1, 0, IPC_RMID, 0);

    return 0;
}