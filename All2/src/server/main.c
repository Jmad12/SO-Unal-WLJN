#include "server.h"

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
    semaforo = sem_open("sema_dog", O_CREAT, 0700, MAX_PROCESS);
    //TUBERIA
    witness = 'T';
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

void catch(int sig){
    fclose(dataDogs);
    fseek(hashDog,0,SEEK_SET);
    fwrite(allHashPets, sizeof(allHashPets),1,hashDog);
    fclose(hashDog);
    exit(-1);
}

int realLength(char word[]){
    int i = 0;
    int result = 0;
    for(i; word[i] != '\0'; i++){
        result = result + 1;
    }
    return result;
}
