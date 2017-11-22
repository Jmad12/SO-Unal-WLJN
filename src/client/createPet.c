#include "client.h"

//Insertar un registro
void createPet(){
    struct dogType *newPet;
    //Guardando el espacio en memoria para la estructura de datos
    newPet = malloc(sizeof(struct dogType));
    readPet(newPet);
    struct requestType request;
    strcpy(request.action,"INSERCION");
    request.pet = *newPet;
    r = send(clientfd, &request, sizeof(request), 0);
    if(r < 0){
        perror("Error enviando los datos al servidor");
    }else{
        printf("\nPet saved");
    }
    free(newPet);
    pressAny();
}