//Bibliotecas
#include <stdio.h>
//Poder manejar bool en C
#include <stdbool.h>
#include <stdlib.h>
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
#define MAX_PROCESS 1

//Variables
//SEMAFORO
sem_t *semaforo;
//TUBERIA
pid_t idPipe;
int pipefd[2];
char witness;
//MUTEX
pthread_mutex_t lock;

//EL arreglo que tiene las cabezas de todos los hash
int allHashPets[1000];
//La cantidad de perros insertados (variable debe almacenarse y cargarse)
int dogAmount;


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



//Funciones
void catch(int sig);
int realLength(char word[]);

void saveHashArray();
void savePet(void *x);
void savePetAmount();

void *processRequest(void *d);

void changeHashHead(int actualHead);

void printClinicalHistory(int index, int clientfd, int r);

void findPet(char searchName[32], int clientfd, int r);

bool removePet(int index);

struct dogType getPet(int index);

void createPet(struct dogType *newPet);

void makeLog(struct requestType request, struct sockaddr_in client);

void initProgram();

int hashFunction(char name[32]);
