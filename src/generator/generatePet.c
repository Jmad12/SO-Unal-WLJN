#include "generator.h"

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