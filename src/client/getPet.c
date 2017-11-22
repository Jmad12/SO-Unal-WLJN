#include "client.h"

//Obtener una mascota
bool getPet(){
    struct requestType request;
    strcpy(request.action,"CANTIDAD");
    r = send(clientfd, &request, sizeof(request), 0);
    r = recv(clientfd, &dogAmount, sizeof(int), 0);
    printf("\nHay %d perro(s), cual desea ver? ", dogAmount);
    int index = 0;
    scanf("%d",&index);
    //Corregimos para el indice insertado (arreglo inicia en 0)
    index = index-1;
    if(index < 0 || index >= dogAmount){
        printf("\nNumero de registro invalido");
        return NULL;
    }
    strcpy(request.action,"LECTURA");
    request.id = index;
    r = send(clientfd, &request, sizeof(request), 0);
    struct dogType pet;
    r = recv(clientfd, &pet, sizeof(struct dogType), 0);
    printPet(index, &pet);
    pressAny();
    return true;
}