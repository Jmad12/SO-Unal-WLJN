#include "generator.h"

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
