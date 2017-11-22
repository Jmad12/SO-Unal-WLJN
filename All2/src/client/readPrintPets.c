#include "client.h"

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
