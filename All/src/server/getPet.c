#include "server.h"

//Mostrar una mascota
struct dogType getPet(int index){
    //MUTEX
    pthread_mutex_lock(&lock);
    //SEMAFORO
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));

    fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);            
    struct dogType *pet;
    pet = malloc(sizeof(struct dogType));
    size_t response = fread(pet, sizeof(struct dogType), 1, dataDogs);
    write(pipefd[1], &witness, sizeof(char));
    //SEMAFORO
    sem_post(semaforo);
    //MUTEX
    pthread_mutex_unlock(&lock);
    return *pet;
    free(pet);
    
}