#include "server.h"

// Insertar una nueva mascota
void createPet(struct dogType *newPet){
    //MUTEX
    pthread_mutex_lock(&lock);
    //SEMAFORO
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));

    //Guardando la mascota
    savePet(newPet);
    //Guardando la cantidad de perros
    dogAmount++;
    savePetAmount();
    //Creando hash
    int hash  = hashFunction(newPet->name);
    if(allHashPets[hash] != -1){
        changeHashHead(allHashPets[hash]);
    }
    allHashPets[hash] = dogAmount;
    //Guardando la listas de hash
    saveHashArray();
    //TUBERIA
    write(pipefd[1], &witness, sizeof(char));
    //SEMAFORO
    sem_post(semaforo);
    //MUTEX
    pthread_mutex_unlock(&lock);
}