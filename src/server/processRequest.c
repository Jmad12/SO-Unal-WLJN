#include "server.h"

void *processRequest(void *d){
    int r;
    struct dataClient data;
    data = *(struct dataClient *)d;
    struct requestType request;
    strcpy(request.action,"INICIO");
    while(strcmp(request.action, "SALIR") != 0){
        r = recv(data.clientfd, &request, sizeof(struct requestType), 0);
        //Metodo de switch convencional para string (aceptado por stackoverflow :v )
        if(strcmp(request.action, "INSERCION") == 0){
            createPet(&request.pet);
            makeLog(request, data.client);
        }else if(strcmp(request.action, "LECTURA") == 0){
            struct dogType pet = getPet(request.id);
            r = send(data.clientfd, &pet, sizeof(struct dogType), 0);
            //TODO R
            makeLog(request, data.client);
        }else if(strcmp(request.action, "BORRADO") == 0){
            makeLog(request, data.client);
            int response = 0;
            if(removePet(request.id)){
                r = send(data.clientfd, &response, sizeof(int), 0);
            }else{
                response = 1;
                r = send(data.clientfd, &response, sizeof(int), 0);
            }
            //TODO R
        }else if(strcmp(request.action, "BUSQUEDA") == 0){
            makeLog(request, data.client);
            findPet(request.search, data.clientfd, r);
        }else if(strcmp(request.action, "HISTORIA") == 0){
            //NO LLEVA LOG
            printClinicalHistory(request.id,data.clientfd,r);

        }else if(strcmp(request.action, "SALIR") == 0){
            makeLog(request, data.client);
        }else if(strcmp(request.action,  "CANTIDAD") == 0){
            r = send(data.clientfd, &dogAmount, sizeof(int), 0);
        }
    }
    close(data.clientfd);
}