#include "generator.h"

void retrieveTableName(){
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