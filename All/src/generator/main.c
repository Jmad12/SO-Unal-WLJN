#include "generator.h"

int main(){
	initProgram();
	
    // Opcion a seleccionar en el menú
    int i = 0; 
    dogAmount = 0;
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
