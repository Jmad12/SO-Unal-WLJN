#include "server.h"

void printClinicalHistory(int index, int clientfd, int r){
    pthread_mutex_lock(&lock);
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));

    if(index < 0 || index >= dogAmount){
        printf("\nNumero de registro invalido");
        fflush(stdout);
        return;
    }
    char path[60];
    //Crear el nombre del archivo
    sprintf(path, "%s%i%s","../history/server/",(index+1), ".txt");
    historicalTxt = fopen(path, "r+");
    if(historicalTxt == NULL){
        historicalTxt = fopen(path, "w+");
        if(historicalTxt == NULL){
            perror("Error creando el archivo de historia para el registro");
            exit(-1);
        }else{
            //Creando archivo inicial de historia clinica
            fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);            
            struct dogType *pet;
            pet = malloc(sizeof(struct dogType));
            size_t response = fread(pet, sizeof(struct dogType), 1, dataDogs);
            fseek(historicalTxt, 0, SEEK_SET);
            //Escribiendo la historia clinica
            fwrite("Bienvenido a la historia clinica", sizeof(char), 32, historicalTxt);
            //Escribiendo cada propiedad
            
            fwrite("\nNombre: ", sizeof(char),9, historicalTxt);
            fwrite(pet->name, realLength(pet->name) , 1, historicalTxt);
            fwrite("\nTipo: ", sizeof(char),7, historicalTxt);
            fwrite(pet->type, realLength(pet->type) , 1, historicalTxt);
            fwrite("\nEdad: ",sizeof(char), 7, historicalTxt);
            fprintf(historicalTxt, "%i", pet->age);
            fwrite("\nRaza: ", sizeof(char),7, historicalTxt);
            fwrite(pet->race, realLength(pet->race) , 1, historicalTxt);
            fwrite("\nAltura: ", sizeof(char), 8, historicalTxt);
            fprintf(historicalTxt, "%i", pet->height);
            fwrite("\nPeso: ", sizeof(char), 7, historicalTxt);
            fprintf(historicalTxt, "%lf", pet->weight);
            fwrite("\nGenero: ", sizeof(char), 8, historicalTxt);
            fwrite(&pet->gender,sizeof(char), 1, historicalTxt);
            //Escribir espacio para las observaciones
            fwrite("\n\nObservaciones:", sizeof(char), 16, historicalTxt);
            free(pet);
        }
    }
    fseek(historicalTxt, 0, SEEK_END);
    int fsize = ftell(historicalTxt);
    fseek(historicalTxt, 0, SEEK_SET);  
    char stringHistory [fsize + 1];
    fread(stringHistory, fsize, 1, historicalTxt);
    fclose(historicalTxt);

    stringHistory[fsize] = 0;
    r = send(clientfd, &fsize, sizeof(int), 0);
    r = send(clientfd, &stringHistory, fsize, 0);

    //Cuando el cliente modifique la historia clinica, reciba la string modificada
    int sizeOfString = 0;
    r = recv(clientfd, &sizeOfString, sizeof(int), 0);
    char stringHistoryUpdated[sizeOfString+1];
    r = recv(clientfd, &stringHistoryUpdated, sizeOfString, 0);
    //Almacene el archivo con la string modificada por el cliente
    char pathNew[60];
    sprintf(pathNew, "%s%i%s","../history/server/",(index+1), ".txt");
    historicalTxt = fopen(pathNew, "w+");
    if(historicalTxt == NULL){
        perror("Error creando el archivo de historia para el registro");
        exit(-1);
    }else{
        fwrite(stringHistoryUpdated, sizeOfString, 1, historicalTxt);
    }
    fclose(historicalTxt);

    //Abrir el archivo
    write(pipefd[1], &witness, sizeof(char));
    sem_post(semaforo);
    pthread_mutex_unlock(&lock);
}