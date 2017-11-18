#include "stdio.h"
//Poder manejar bool en C
#include <stdbool.h>
#include "stdlib.h"
//Libreria de sockets
#include <sys/socket.h>
#include <sys/types.h>
//Para manejar las direcciones IP
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//Funcion close
#include <unistd.h>
//strcmp  function
#include "string.h"
//tolower function
#include <ctype.h>
//Time para los logs
#include <time.h>
//Ctrl C
#include <signal.h>
//Hilos
#include <pthread.h>
//Semaphore
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 3535
//Cantidad de clientes
#define BACKLOG 8
#define NUM_HILOS 8

//SEMAFORO
sem_t *semaforo;
//TUBERIA
pid_t idPipe;
int pipefd[2];
char witness = 'T';
//MUTEX
pthread_mutex_t lock;

//EL arreglo que tiene las cabezas de todos los hash
int allHashPets[1000];
//La cantidad de perros insertados (variable debe almacenarse y cargarse)
int dogAmount = 0;


//Archivos necesarios para el funcionamiento del programa
FILE *dataDogs;
FILE *hashDog;
FILE *petAmount;
FILE *historicalTxt;
FILE *serverLogs;


//Struct de las mascotas
struct dogType{
    char name[32];
    char type[32];
    int age;
    char race[16];
    int height;
    double weight;
    char gender;
    int nextPet;
    int prevPet;
};
//Struct para todos los request que se hagan desde el cliente
struct requestType{
    //Puede ser INSERCION, LECTURA, ,BORRADO, BUSQUEDA, HISTORIA, SALIR
    char action[32];
    struct dogType pet;
    //ID de la mascota
    int id;
    //Cadena a buscar
    char search[32];
};
//Struct para los datos de cada cliente
struct dataClient{
    int clientfd;
    struct sockaddr_in client;
};

//Guardando la tabla hash
void saveHashArray(){
    fseek(hashDog,0,SEEK_SET);
    size_t response = fwrite(&allHashPets, sizeof(int), 1000, hashDog);
    //TODO validacion fwrite
}
//Guardando una mascota
void savePet(void *x){
    struct dogType *pet;
    pet = x;
    fseek(dataDogs, 0, SEEK_END);
    //Guardando el perro en la última posición
    size_t response = fwrite(pet, sizeof(struct dogType), 1, dataDogs);
    //TODO validacion de fwrite 
}
//Guardando la cantidad de mascotas
void savePetAmount(){
    fseek(petAmount,0,SEEK_SET);
    //Guardando la cantidad nueva de perros
    size_t response = fwrite(&dogAmount, sizeof(int), 1, petAmount);
    //TODO validacion fwrite
}
int hashFunction(char name[32]){
    int i;
    for(i = 0; name[i]; i++){
        name[i] = tolower(name[i]);
    }
    int result = 0;
    for (i = 0; i < name[i] != '\0';i++){
        result += name[i];
    }
    result = (result * 33 ) % 1000;
    if(result < 0){
        result = result * -1;
        return result;
    }
    return result;
}
int realLength(char word[]){
    int i = 0;
    int result = 0;
    for(i; word[i] != '\0'; i++){
        result = result + 1;
    }
    return result;
}
//Cambiando la cabeza del hash en caso de que ya exista
void changeHashHead(int actualHead){
    fseek(dataDogs, sizeof(struct dogType)*(actualHead-1), SEEK_SET);
    struct dogType *head;
    head = malloc(sizeof(struct dogType));
    size_t response = fread(head, sizeof(struct dogType), 1, dataDogs);
    head->prevPet = dogAmount;
    //Posicionar el puntero del archivo para que sobreescriba el registro anterior
    fseek(dataDogs, sizeof(struct dogType)*(actualHead-1), SEEK_SET);
    response = fwrite(head, sizeof(struct dogType), 1, dataDogs);
    struct dogType *newHead;
    newHead = malloc(sizeof(struct dogType));
    //Modificar el registro recien ingresado
    fseek(dataDogs, sizeof(struct dogType)*(dogAmount-1), SEEK_SET);
    response = fread(newHead, sizeof(struct dogType), 1, dataDogs);
    newHead->nextPet = actualHead;
    fseek(dataDogs, sizeof(struct dogType)*(dogAmount-1), SEEK_SET);
    response = fwrite(newHead, sizeof(struct dogType), 1, dataDogs);
    free(head);
    free(newHead);
}
void createPet(struct dogType *newPet){
    //MUTEX
    pthread_mutex_lock(&lock);
    //SEMAFORO
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));

    //Guardando la mascota
    savePet(newPet);
    //Guardando la cantidad de perros
    dogAmount++;
    savePetAmount();
    //Creando hash
    int hash  = hashFunction(newPet->name);
    if(allHashPets[hash] != -1){
        changeHashHead(allHashPets[hash]);
    }
    allHashPets[hash] = dogAmount;
    //Guardando la listas de hash
    saveHashArray();
    //TUBERIA
    write(pipefd[1], &witness, sizeof(char));
    //SEMAFORO
    sem_post(semaforo);
    //MUTEX
    pthread_mutex_unlock(&lock);
}
//Inicializar el programa
void initProgram(){
    //Obtener la cantidad de perros
    petAmount = fopen("petAmount.dat", "r+");
    //Validar en caso de error
    size_t response;
    if(petAmount == NULL){
        petAmount = fopen("petAmount.dat", "w+");
        response = fwrite(&dogAmount, sizeof(int), 1, petAmount);
    }else{
        //Guardando el nuevo registro
        response = fread(&dogAmount, sizeof(int), 1, petAmount);
        if(response == 0){
            //printf("%d",(int)response);
            perror("No se pudo leer la mascota en petAmount.dat");
            exit(-1);
        }
    }

    //Obtener la tabla hash
    hashDog = fopen("hashDog.dat", "r+");
    if(hashDog == NULL){
        hashDog = fopen("hashDog.dat", "w+");
        int i = 0;
        for(i; i < 1000;  i++){
            allHashPets[i] = -1;
        }
        response = fwrite(&allHashPets, sizeof(int), 1000, hashDog);  
    }else{
        fseek(hashDog, 0, SEEK_SET);
        response = fread(&allHashPets, sizeof(int), 1000, hashDog);
        if(response == 0){
            //printf("%d",(int)response);
            perror("No se pudo leer la mascota en hashDog.dat");
            exit(-1);
        }
    }
    //Crear dataDogs en caso de que no exista
    dataDogs = fopen("dataDogs.dat", "r+");
    if(dataDogs == NULL){
        dataDogs = fopen("dataDogs.dat", "w+");
    }
    //Crear datalogs en caso de que no exista
    serverLogs = fopen("serverLogs.log", "r+");
    if(serverLogs == NULL){
        serverLogs = fopen("serverLogs.log", "w+");
    }
}
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

//Mostrar una mascota
struct dogType getPet(int index){
    //MUTEX
    pthread_mutex_lock(&lock);
    //SEMAFORO
    sem_wait(semaforo);
    //TUBERIA
    read(pipefd[0], &witness, sizeof(char));

    fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);            
    struct dogType *pet;
    pet = malloc(sizeof(struct dogType));
    size_t response = fread(pet, sizeof(struct dogType), 1, dataDogs);
    write(pipefd[1], &witness, sizeof(char));
    //SEMAFORO
    sem_post(semaforo);
    //MUTEX
    pthread_mutex_unlock(&lock);
    return *pet;
    free(pet);
    
}

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
        sprintf(removedPath, "%i%s",index+1,".txt");
        sprintf(lastPath, "%i%s",dogAmount,".txt");
        remove(removedPath);
        rename(lastPath, removedPath);
    }else{
        //Quitando la historia clinica
        char removedPath[40];
        sprintf(removedPath, "%i%s",index+1,".txt");
        remove(removedPath);        
    }
    
    //disminuimos dogAmount
    dogAmount = dogAmount-1;
    savePetAmount();
    //Disminuir tamaño del archivo creando un archivo nuevo, copiando y renombrando
    FILE *tempfile;
    tempfile = fopen("Datacopy.dat","w+");
    fseek(dataDogs,0,SEEK_SET);
    int i;
    unsigned int tempvar;
    for (i=0; i < (dogAmount)*sizeof(struct dogType); i++){
        tempvar = getc(dataDogs);
        fputc(tempvar,tempfile);
    }   
    fclose(tempfile); 
    fclose(dataDogs);
    remove("dataDogs.dat");
    rename("Datacopy.dat","dataDogs.dat"); 
    dataDogs = fopen("dataDogs.dat","r+");

    free(removedPet);
    free(nextPet);
    free(previousPet);
    free(lastPet);
    write(pipefd[1], &witness, sizeof(char));
    sem_post(semaforo);
    pthread_mutex_unlock(&lock);
    return true;
}
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
    char path[20];
    //Crear el nombre del archivo
    sprintf(path, "%i%s", (index+1), ".txt");
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
    char pathNew[20];
    sprintf(pathNew, "%i%s", (index+1), ".txt");
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

void catch(int sig){
    fclose(dataDogs);
    fseek(hashDog,0,SEEK_SET);
    fwrite(allHashPets, sizeof(allHashPets),1,hashDog);
    fclose(hashDog);
    exit(-1);
}

int main(){
    initProgram();
    signal(SIGINT, &catch);
    int serverfd, clientfd, r, opt = 1;
    struct sockaddr_in server, client;
    socklen_t tamano;
    pthread_t idThread[NUM_HILOS];
    tamano  = sizeof(struct sockaddr_in);
    //MUTEX
    if(pthread_mutex_init(&lock,NULL) != 0){
        perror("\n Error creando el mutex");
        exit(-1);
    }
    //SEMAFORO
    semaforo = sem_open("sema_dog", O_CREAT, 0700, NUM_HILOS);
    //TUBERIA
    pipe(pipefd);
    write(pipefd[1],&witness, sizeof(char));

    serverfd = socket (AF_INET, SOCK_STREAM, 0);
    if(serverfd < 0){
        perror("\n Error abriendo el sockect");
        exit(-1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8);
    //Se configura para que el puerto pueda ser reutilizado después de la ejecución del programa
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));
    r = bind(serverfd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    if(r < 0){
        perror("\n Error en el bind");
        exit(-1);
    }
    r = listen(serverfd, BACKLOG);
    if(r < 0){
        perror("\n Error en la función listen");
        exit(-1);
    }
    int i = 0;
    for(i = 0; i < NUM_HILOS; i++){
        clientfd = accept(serverfd, (struct sockaddr *)&client, &tamano);
        if( clientfd < 0 ){
            perror("\n Error en la funcion accept");
            exit(-1);
        }
        struct dataClient clientData;
        clientData.clientfd  = clientfd;
        clientData.client = client;
        r = pthread_create(&idThread[i], NULL, (void *)processRequest, (void *)(&clientData));
        
        if(r != 0){
            perror("\n No pudo crear el hilo");
            exit(-1);
        }
       
    }   
    char *returnValue;
    for(i = 0; i < NUM_HILOS; i++){
        pthread_join(idThread[i], (void **)&returnValue);
    }
    //MUTEX
    pthread_mutex_destroy(&lock);
    //SEMAFORO
    sem_close(semaforo);
    sem_unlink("sema_dog");
    //TUBERIA
   

    fclose(dataDogs);
    fseek(hashDog,0,SEEK_SET);
    fwrite(allHashPets, sizeof(allHashPets),1,hashDog);
    fclose(hashDog);
    close(serverfd);
}