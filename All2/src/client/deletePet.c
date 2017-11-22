#include "client.h"

//Metodo que llama a remove pet para borrar la mascota
bool deletePet(){
    printf("\nBorrando...");
    struct requestType request;
    strcpy(request.action,"CANTIDAD");
    r = send(clientfd, &request, sizeof(request), 0);
    r = recv(clientfd, &dogAmount, sizeof(int), 0);
    printf("\nHay %d perro(s), cual desea eliminar? ", dogAmount);
    int index = 0;
    scanf("%d",&index);
    //Corregimos para el indice insertado (arreglo inicia en 0)
    index = index-1;
    if(index < 0 || index >= dogAmount){
        printf("\nNumero de registro invalido");
        return false;
    }
    strcpy(request.action,"BORRADO");
    request.id = index;
    r = send(clientfd, &request, sizeof(request), 0);
    int response = 0;
    r = recv(clientfd, &response, sizeof(int), 0);
    if(response == 0){
        printf("Borrado satisfactoriamente");
    }else{
        printf("Hubo un error, intentelo más tarde");
    }
    pressAny();
    return true;
}