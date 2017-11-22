#include "server.h"

//Crear logs
void makeLog(struct requestType request, struct sockaddr_in client){
    time_t now = time(0);
    struct tm *local = localtime(&now);
    char output[128];
    strftime(output, 128, "%d/%m/%y %H:%M:%S", local);
    fputs("Fecha ", serverLogs);
    fputs(output, serverLogs);
    fputs(" Cliente [", serverLogs);
    fputs(inet_ntoa(client.sin_addr), serverLogs);
    char num[10];
    if(strcmp(request.action, "INSERCION") == 0){
        fputs("] INSERCION ", serverLogs);
        sprintf(num, "%d", 0);
        fputs(num, serverLogs);
    }else if(strcmp(request.action, "LECTURA") == 0){
        fputs("] LECTURA ", serverLogs);
        sprintf(num, "%d", request.id);
        fputs(num, serverLogs);
    }else if(strcmp(request.action, "BORRADO") == 0){
        fputs("] BORRADO ", serverLogs);
        sprintf(num, "%d", request.id);
        fputs(num, serverLogs);
    }else if(strcmp(request.action, "BUSQUEDA") == 0){
        fputs("] BUSQUEDA ", serverLogs);
        fputs(request.search, serverLogs);
    }else if(strcmp(request.action, "SALIR") == 0){
        fputs("] SALIR ", serverLogs);
    }
    fputs("\n", serverLogs);
}