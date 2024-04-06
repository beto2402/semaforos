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

    int semids[ROWS];

    // Memoria de semáforo 
    for (int i = 0; i < ROWS; i++) {
        key_t keySem = ftok(".", i);
        semids[i] = semget(keySem, 1, IPC_CREAT | 0644);
    }
    
    
    // Memoria de resultados 
    int *RESULTADO;
    key_t keyResultados = ftok(".", 'd');
    int shmidResultados = shmget(keyResultados, sizeof(int)*ROWS, IPC_CREAT | 0777);

    printf("Iniciando proceso padre \n");

    for (int i = 0; i < ROWS * COLS; i++) {
        MATRIX[i] = values[i / COLS][i % COLS];
    }
    printf("Matriz guardada \n");

    for (int i = 0; i < ROWS; i++) {
        semctl(semids[i], 0, SETVAL, 1);
    }

    int procesosCompletos;

    do {
        procesosCompletos = 0;

        printf("Esperando proceso(s): [ ");

        for (int i = 0; i < ROWS; i++) {
            if (semctl(semids[i], 0, GETVAL, 0) == 1) {
                printf("r%d, ", i);
            } else {
                procesosCompletos += 1;
            }
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
    
    for (int i = 0; i < ROWS; i++) {
        semctl(semids[i], 0, IPC_RMID, 0);
    }

    return 0;
}