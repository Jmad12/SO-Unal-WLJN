#include "server.h"

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