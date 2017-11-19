#include "server.h"

//Encontrar una mascota
void findPet(char searchName[32], int clientfd, int r){
    pthread_mutex_lock(&lock);
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));
    
    char petName[32];
    int founds = 0;
    int headHash = allHashPets[hashFunction(searchName)];
    struct dogType *petFound;
    petFound = malloc(sizeof(struct dogType));
    while(headHash != -1){
        fseek(dataDogs, sizeof(struct dogType)*(headHash-1), SEEK_SET);
        fread(petFound, sizeof(struct dogType), 1, dataDogs);
        strcpy(petName,petFound->name);
        int i;
        for(i = 0; petName[i]; i++){
            petName[i] = tolower(petName[i]);
        }   
        for(i = 0; searchName[i]; i++){
            searchName[i] = tolower(searchName[i]);
        }  
        //TODO Validaciones
        if(strcmp(petName, searchName) == 0){
            founds = founds +1;
            //Se podría llamar a la función printPet, pero habría que hacer cambio
            r = send(clientfd, &headHash, sizeof(int), 0);
            r = send(clientfd, petFound, sizeof(struct dogType), 0);
        }
        headHash = petFound->nextPet;
    }
    if(founds == 0){
        headHash = -1;
        r = send(clientfd, &headHash, sizeof(int), 0);
    }
    headHash = -2;
    r = send(clientfd, &headHash, sizeof(int), 0);
    free(petFound);
    write(pipefd[1], &witness, sizeof(char));
    sem_post(semaforo);
    pthread_mutex_unlock(&lock);
}