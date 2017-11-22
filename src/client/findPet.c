#include "client.h"

//Encontrar una mascota
void findPet(){
    printf("Ingrese el nombre que desea buscar (max 32 caracteres): ");
    struct requestType request;
    strcpy(request.action,"BUSQUEDA");
    scanf("%s", request.search);
    r = send(clientfd, &request, sizeof(request), 0);
    int id;
    r = recv(clientfd, &id, sizeof(int), 0);
    struct dogType petFound;
    while(id != -2){
        if(id == -1){
            printf("\nNo se encontró ninguna coincidencia");
        }else{
            r = recv(clientfd, &petFound, sizeof(struct dogType), 0);
            printf("\nNúmero de registro: %i", id);
            printf("\nNombre: %s", petFound.name);
            printf("\nTipo: %s", petFound.type);
            printf("\nEdad: %i", petFound.age);
            printf("\nRaza: %s", petFound.race);
            printf("\nAltura: %i", petFound.height);
            printf("\nPeso: %lf", petFound.weight);
            printf("\nGenero: %c \n", petFound.gender); 
        }
        r = recv(clientfd, &id, sizeof(int), 0);
    }
    pressAny();
}