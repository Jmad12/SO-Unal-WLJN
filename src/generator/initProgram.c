#include "generator.h"

void initProgram(){
    //Obtener la cantidad de perros
	petAmount = fopen("../data/petAmount.dat", "r+");
    //Validar en caso de error
    size_t response;
    if(petAmount == NULL){
		petAmount = fopen("../data/petAmount.dat", "w+");
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
    hashDog = fopen("../data/hashDog.dat", "r+");
    if(hashDog == NULL){
        hashDog = fopen("../data/hashDog.dat", "w+");
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
    dataDogs = fopen("../data/dataDogs.dat", "r+");
    if(dataDogs == NULL){
        dataDogs = fopen("../data/dataDogs.dat", "w+");
	}

    nombresMascotas = fopen("../data/nombresMascotas.txt", "r");
    if (nombresMascotas == NULL)
        exit(EXIT_FAILURE);
}