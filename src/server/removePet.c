#include "server.h"

//Remover una mascota
bool removePet(int index){
    pthread_mutex_lock(&lock);
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));

    //Corregir indices de lista enlazada
    struct dogType *removedPet;
    removedPet = malloc(sizeof(struct dogType));
    struct dogType *previousPet;
    previousPet = malloc(sizeof(struct dogType));
    struct dogType *nextPet;
    nextPet = malloc(sizeof(struct dogType));
    struct dogType *lastPet;
    lastPet = malloc(sizeof(struct dogType));

    fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);     
    fread(removedPet, sizeof(struct dogType), 1, dataDogs);

    //Si el perro a remover era cabeza del hash:
    if(removedPet->prevPet == -1){
        allHashPets[hashFunction(removedPet->name)] = (removedPet->nextPet);
        saveHashArray();
    }

    if(removedPet->prevPet != -1){
        //El anterior en la lista del removido, tiene de siguiente el siguiente del removido
        fseek(dataDogs, sizeof(struct dogType)* (removedPet->prevPet-1),SEEK_SET);
        fread(previousPet, sizeof(struct dogType), 1, dataDogs);
        previousPet->nextPet = removedPet->nextPet;
        fseek(dataDogs, sizeof(struct dogType)*(removedPet->prevPet-1),SEEK_SET);
        fwrite(previousPet, sizeof(struct dogType), 1, dataDogs);
    }
    if(removedPet->nextPet != -1){
        //El siguiente en la lista del removido, tiene de anterior el anterior del removido
        fseek(dataDogs, sizeof(struct dogType)*(removedPet->nextPet-1),SEEK_SET);
        fread(nextPet, sizeof(struct dogType), 1, dataDogs);
        nextPet->prevPet = removedPet->prevPet;
        fseek(dataDogs, sizeof(struct dogType)*(removedPet->nextPet-1),SEEK_SET);
        fwrite(nextPet, sizeof(struct dogType), 1, dataDogs);
    }

    if(index != (dogAmount-1)){
         //Voy al ultimo perro de la lista y lo muevo a la posicion del removido
        fseek(dataDogs, sizeof(struct dogType)*(dogAmount-1), SEEK_SET);
        fread(lastPet, sizeof(struct dogType), 1, dataDogs);

        //Antes de insertarlo en su nueva posicion, corregimos sus indices de lista enlazada (nextPet y prevPet ahora son los next y prev del ultimo perro)
        if(lastPet->prevPet != -1){
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->prevPet-1), SEEK_SET);
            fread(previousPet, sizeof(struct dogType), 1, dataDogs);
            previousPet->nextPet = index+1;
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->prevPet-1),SEEK_SET);
            fwrite(previousPet, sizeof(struct dogType), 1, dataDogs);
        }
        if(lastPet->nextPet != -1){
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->nextPet-1), SEEK_SET);
            fread(nextPet, sizeof(struct dogType), 1, dataDogs);
            nextPet->prevPet = index+1;
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->nextPet-1),SEEK_SET);
            fwrite(nextPet, sizeof(struct dogType), 1, dataDogs);
        }
        //Insertamos el ultimo perro en la posicion del perro removido
        fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);     
        fwrite(lastPet, sizeof(struct dogType), 1, dataDogs);

        //Si el ultimo perro era cabeza de lista:
        if(lastPet->prevPet == -1){
            allHashPets[hashFunction(lastPet->name)] = index+1;
            saveHashArray();
        }
        //Renombrar la historia clinica al nuevo ID
        char removedPath[40];
        char lastPath[40];
        sprintf(removedPath, "%s%i%s","../history/server",index+1,".txt");
        sprintf(lastPath, "%s%i%s","../data/",dogAmount,".txt");
        remove(removedPath);
        rename(lastPath, removedPath);
    }else{
        //Quitando la historia clinica
        char removedPath[40];
        sprintf(removedPath, "%s%i%s","../history/server",index+1,".txt");
        remove(removedPath);        
    }
    
    //disminuimos dogAmount
    dogAmount = dogAmount-1;
    savePetAmount();
    //Disminuir tamaño del archivo creando un archivo nuevo, copiando y renombrando
    FILE *tempfile;
    tempfile = fopen("../data/Datacopy.dat","w+");
    fseek(dataDogs,0,SEEK_SET);
    int i;
    unsigned int tempvar;
    for (i=0; i < (dogAmount)*sizeof(struct dogType); i++){
        tempvar = getc(dataDogs);
        fputc(tempvar,tempfile);
    }   
    fclose(tempfile); 
    fclose(dataDogs);
    remove("../data/dataDogs.dat");
    rename("../data/Datacopy.dat","../data/dataDogs.dat"); 
    dataDogs = fopen("../data/dataDogs.dat","r+");

    free(removedPet);
    free(nextPet);
    free(previousPet);
    free(lastPet);
    write(pipefd[1], &witness, sizeof(char));
    sem_post(semaforo);
    pthread_mutex_unlock(&lock);
    return true;
}