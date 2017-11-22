//Bibliotecas
#include "stdio.h"
//Poder manejar bool en C
#include <stdbool.h>
#include "stdlib.h"
#include "string.h"
//Libreria de sockets
#include <sys/socket.h>
#include <sys/types.h>
//Para manejar las direcciones IP
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//Funcion close
#include <unistd.h>

#define PORT 3535



//Declación de variables
int clientfd, r; 

//La cantidad de perros insertados (variable debe almacenarse y cargarse)
int dogAmount;

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



//Declaración de funciones
void pressAny();
bool printClinicalHistory(int index);
void readPet(void *x);
void printPet(int index, struct dogType *pet);
void findPet();
bool deletePet();
void createPet();
bool getPet();

