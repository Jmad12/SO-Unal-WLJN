#include "stdio.h"
//Poder manejar bool en C
#include <stdbool.h>
#include "stdlib.h"
#include "string.h"
#include "time.h"


#include <ctype.h>
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
//EL arreglo que tiene las cabezas de todos los hash
int allHashPets[1000];

//La cantidad de perros insertados (variable debe almacenarse y cargarse)
int dogAmount = 0;

//Archivos necesarios para el funcionamiento del programa
FILE *dataDogs;
FILE *hashDog;
FILE *petAmount;
FILE *historicalTxt;
FILE *nombresMascotas;
char tablaNombres[1800] [32];
char tablaTipos[1800] [32];
char tablaRazas[1800] [32];
int nameAmount;


void saveHashArray(){
    fseek(hashDog,0,SEEK_SET);
    size_t response = fwrite(&allHashPets, sizeof(int), 1000, hashDog);
    //TODO validacion fwrite
}
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
//Leer una mascota de pantalla
void generatePet(void *x){
    //Creando un puntero que conoce la estructura dogType
    struct dogType *p;
    p = x;
    //inicializando variables de generacion aleatoria

    int random = 0;
    //Guardando cada una de las propiedades en el puntero pasado como parametro
    fflush(stdin);

    random = rand();
    strcpy(p->name,tablaNombres[random%nameAmount]);
    random = rand();
    strcpy(p->type,tablaTipos[random%5]);
    random = rand();
    p->age = random%10;
    random = rand();
	strcpy(p->race,tablaRazas[random%5]);
    random = rand();
    p->height = random%10;
    random = rand();
    p->weight = random%10;
    fflush(stdin);
    random = rand()%2;
    if(random==0){
    	p->gender = 'M';
    }
    else{
    	p->gender = 'F';
    }
    p->nextPet = -1;
    p->prevPet = -1;
}

void savePet(void *x){
    struct dogType *pet;
    pet = x;
    fseek(dataDogs, 0, SEEK_END);
    //Guardando el perro en la última posición
    size_t response = fwrite(pet, sizeof(struct dogType), 1, dataDogs);
    //TODO validacion de fwrite 
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

//Insertar un registro
void generateRandomDog(){
    struct dogType *newPet;
    //Guardando el espacio en memoria para la estructura de datos
    newPet = malloc(sizeof(struct dogType));
    generatePet(newPet);
    //Guardando la mascota
    savePet(newPet);
    //Guardando la cantidad de perros
    dogAmount++;
	savePetAmount();
    //Creando hash

    int hash  = hashFunction(newPet->name);
    //printf("\nCabeza actual del hash: %i ", allHashPets[hash]);
    if(allHashPets[hash] != -1){
        changeHashHead(allHashPets[hash]);
    }
    allHashPets[hash] = dogAmount;
    //Guardando la listas de hash
    saveHashArray();
    free(newPet);
    //printf("\nPet saved");
}


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

    nombresMascotas = fopen("nombresMascotas.txt", "r");
    if (nombresMascotas == NULL)
        exit(EXIT_FAILURE);
}

void  retrieveTableName(){
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    nameAmount = 0;

    while ((read = getline(&line, &len, nombresMascotas)) != -1) {
        //Removiendo el ultimo \n 
        line[strlen(line)-1] = '\0';
        strcpy(tablaNombres[nameAmount],line);
        nameAmount++;
    }

    if (line)
        free(line);
	srand(time(NULL));
}

void retrieveTableType(){
	strcpy(tablaTipos[0],"Perro");
	strcpy(tablaTipos[1],"Gato");
	strcpy(tablaTipos[2],"Tortuga");
	strcpy(tablaTipos[3],"Conejo");
	strcpy(tablaTipos[4],"Pajaro");
}
void retrieveTableRace(){
	strcpy(tablaRazas[0],"Pastor aleman");
	strcpy(tablaRazas[1],"Labrador");
	strcpy(tablaRazas[2],"Rottweiler");
	strcpy(tablaRazas[3],"Beagle");
	strcpy(tablaRazas[4],"Bulldog");
}
int main(){
	initProgram();
	
    // Opcion a seleccionar en el menú
    int i = 0; 
    retrieveTableName();
    retrieveTableType();
    retrieveTableRace();
    for(i;i<10000000;i++){
    	generateRandomDog();
    }
    //close files
    fclose(dataDogs);
    fclose(hashDog);
    fclose(nombresMascotas);
    return 0;
}
