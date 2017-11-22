#include "client.h"

// Ver historia clinica
bool printClinicalHistory(int index){
    printf("\nImprimiendo...");
    int sizeOfString = 0;
    struct requestType request;
    strcpy(request.action,"HISTORIA");
    request.id = index;
    r = send(clientfd, &request, sizeof(request), 0);
    if(r < 0){
        perror("Error enviando los datos al servidor");
    }
    r = recv(clientfd, &sizeOfString, sizeof(int), 0);
    if(r < 0){
        perror("Error recibiendo los datos del servidor");
    }
    char stringHistory[sizeOfString+1];
    r = recv(clientfd, &stringHistory, sizeOfString, 0);
    if(r < 0){
        perror("Error recibiendo los datos del servidor");
    }

    //Crear el archivo temporal y abrirlo.

    FILE *historicalTxt;
    char path[60];
    sprintf(path, "%s%i%s","../history/client/",(index+1), ".txt");
    historicalTxt = fopen(path, "w+");
    if(historicalTxt == NULL){
        perror("Error creando el archivo de historia para el registro");
        exit(-1);
    }else{
        fwrite (stringHistory , sizeOfString, 1, historicalTxt);
    }
    fclose(historicalTxt);

    //El usuario edita el archivo

    char gedit[60];
    sprintf(gedit, "%s%s", "gedit ", path);
    system(gedit);

    //Abrir el archivo y extraerlo como string

    historicalTxt = fopen(path, "r+");
    fseek(historicalTxt, 0, SEEK_END);
    int fsize = ftell(historicalTxt);
    fseek(historicalTxt, 0, SEEK_SET);  
    char stringHistoryUpdated [fsize + 1];
    fread(stringHistoryUpdated, fsize, 1, historicalTxt);
    fclose(historicalTxt);
    stringHistoryUpdated[fsize] = 0;
    remove(path);

    r = send(clientfd, &fsize, sizeof(int), 0);
    r = send(clientfd, &stringHistoryUpdated, fsize, 0);


    return true;
}
