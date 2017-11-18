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

int clientfd, r; 

//La cantidad de perros insertados (variable debe almacenarse y cargarse)
int dogAmount = 0;

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
//Funcion para presionar cualquier tecla
void pressAny(){
    char ch;
    printf("\nPresione ENTER para continuar");
    while(getchar()!='\n'); // limpia stdin
    getchar(); // espera un ENTER
    printf("\n");
}
//Leer una mascota de pantalla
void readPet(void *x){
    //Creando un puntero que conoce la estructura dogType
    struct dogType *p;
    p = x;
    //Guardando cada una de las propiedades en el puntero pasado como parametro
    fflush(stdin);
    printf("\nIngrese el nombre de la mascota, deben ser maximo 32 caracteres: ");
    scanf("%s", p->name);
    printf("\nIngrese el tipo de mascota, maximo 32 caracteres: ");
    scanf("%s", p->type);
    printf("\nIngrese la edad: ");
    scanf("%i", &p->age);
    printf("\nIngrese la raza del animal, maximo 16 caractres: ");
    scanf("%s", p->race);
    printf("\nIngrese la altura de la mascota: ");
    scanf("%i", &p->height);
    printf("\nCuanto pesa su mascota?: ");
    scanf("%lf", &p->weight);
    printf("\nIngrese M si es masculino o F si es femenino: ");
    fflush(stdin); scanf(" %c", &p->gender);
    p->nextPet = -1;
    p->prevPet = -1;
}
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

bool printClinicalHistory(int index){
    printf("\nImprimiendo...");
    int sizeOfString = 0;
    struct requestType request;
    strcpy(request.action,"HISTORIA");
    request.id = index;
    r = send(clientfd, &request, sizeof(request), 0);
    if(r < 0){
        perror("Error enviando los datos al servidor");
    }
    r = recv(clientfd, &sizeOfString, sizeof(int), 0);
    if(r < 0){
        perror("Error recibiendo los datos del servidor");
    }
    char stringHistory[sizeOfString+1];
    r = recv(clientfd, &stringHistory, sizeOfString, 0);
    if(r < 0){
        perror("Error recibiendo los datos del servidor");
    }

    //Crear el archivo temporal y abrirlo.

    FILE *historicalTxt;
    char path[20];
    sprintf(path, "%i%s", (index+1), ".txt");
    historicalTxt = fopen(path, "w+");
    if(historicalTxt == NULL){
        perror("Error creando el archivo de historia para el registro");
        exit(-1);
    }else{
        fwrite (stringHistory , sizeOfString, 1, historicalTxt);
    }
    fclose(historicalTxt);

    //El usuario edita el archivo

    char gedit[30];
    sprintf(gedit, "%s%s", "gedit ", path);
    system(gedit);

    //Abrir el archivo y extraerlo como string

    historicalTxt = fopen(path, "r+");
    fseek(historicalTxt, 0, SEEK_END);
    int fsize = ftell(historicalTxt);
    fseek(historicalTxt, 0, SEEK_SET);  
    char stringHistoryUpdated [fsize + 1];
    fread(stringHistoryUpdated, fsize, 1, historicalTxt);
    fclose(historicalTxt);
    stringHistoryUpdated[fsize] = 0;
    remove(path);

    r = send(clientfd, &fsize, sizeof(int), 0);
    r = send(clientfd, &stringHistoryUpdated, fsize, 0);


    return true;
}

//Mostrar una mascota
void printPet(int index, struct dogType *pet){
    
    printf("Mostrando el perro %d: \n",(index+1));        
    printf("\nNombre: %s", pet->name);
    printf("\nTipo: %s", pet->type);
    printf("\nEdad: %i", pet->age);
    printf("\nRaza: %s", pet->race);
    printf("\nAltura: %i", pet->height);
    printf("\nPeso: %lf", pet->weight);
    printf("\nGenero: %c \n", pet->gender);
    //Si se quiere ver la historia clinica
    printf("Escribe 1 si quieres ver la historia clinica: ");
    char option;
    scanf(" %c", &option);
    if(option == '1'){
        //Index corregido anteriormente
        printClinicalHistory(index);
    }
}



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

int main(int argc, char *argv[]){
    struct sockaddr_in client;
    struct hostent *he;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0){
        perror("\n Error abriendo el socket");
        exit(1);
    }

    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    bzero(client.sin_zero, 8);
    inet_aton(argv[1], &client.sin_addr);

    r = connect(clientfd, (struct sockaddr *)&client, (socklen_t)sizeof(struct sockaddr));
    if(r < 0){
        perror("\n Error conectando al servidor");
        exit(-1);
    }
    
    // Opcion a seleccionar en el menú
    int option = 0; 

    while(option != 5){
        //Desplegar el menú completo
        printf("\nBienvenidos a su tienda de mascotas preferida");
        printf("\nSeleccione entre las siguientes opciones su preferida.");
        printf("\n1. Ingresar registro");
        printf("\n2. Ver registro");
        printf("\n3. Borrar registro");
        printf("\n4. Buscar registro");
        printf("\n5. Salir");
        printf("\nIngrese su opcion: ");
        fflush(stdin); scanf("%i", &option); 
        switch(option){
            case 1: 
                createPet();
                break;
            case 2:
                getPet();
                break;
            case 3:
                deletePet();
                break;
            case 4: 
                findPet();
                break;
            case 5: break;
            default:
                printf("\nElija una opción valida\n");
                break;
        }
    }

    struct requestType request;
    strcpy(request.action,"SALIR");
    r = send(clientfd, &request, sizeof(request), 0);

    close(clientfd);
    return 0;
}