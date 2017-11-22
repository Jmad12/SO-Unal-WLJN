//Bibliotecas
#include "stdio.h"
//Poder manejar bool en C
#include <stdbool.h>
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include <ctype.h>

//Variables
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
int dogAmount;

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

//Funciones
int hashFunction(char name[32]);
void changeHashHead(int actualHead);
void initProgram();
void saveHashArray();
void savePetAmount();
void savePet(void *x);
void generatePet(void *x);
void retrieveTableName();
void retrieveTableType();
void generateRandomDog();
void retrieveTableRace();