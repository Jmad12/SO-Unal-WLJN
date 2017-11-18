#include "stdio.h"
//Poder manejar bool en C
#include <stdbool.h>
#include "stdlib.h"
#include "string.h"


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

//Funcion para presionar cualquier tecla
void pressAny(){
    char ch;
    printf("\nPresione ENTER para continuar");
    while(getchar()!='\n'); // limpia stdin
    getchar(); // espera un ENTER
    printf("\n");
}
//Guardando la tabla hash
void saveHashArray(){
    fseek(hashDog,0,SEEK_SET);
    size_t response = fwrite(&allHashPets, sizeof(int), 1000, hashDog);
    //TODO validacion fwrite
}
//Guardando la cantidad de mascotas
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
//Guardando una mascota
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
//Para guardar en un txt se debe saber los char's necesarios para no rayar más de la cuenta
int realLength(char word[]){
    int i = 0;
    int result = 0;
    for(i; word[i] != '\0'; i++){
        result = result + 1;
    }
    return result;
}
//Insertar un registro
void createPet(){
    struct dogType *newPet;
    //Guardando el espacio en memoria para la estructura de datos
    newPet = malloc(sizeof(struct dogType));
    readPet(newPet);
    //Guardando la mascota
    savePet(newPet);
    //Guardando la cantidad de perros
    dogAmount++;
	savePetAmount();
    //Creando hash
    int hash  = hashFunction(newPet->name);
    printf("\nCabeza actual del hash: %i ", allHashPets[hash]);
    if(allHashPets[hash] != -1){
        changeHashHead(allHashPets[hash]);
    }
    allHashPets[hash] = dogAmount;
    //Guardando la listas de hash
    saveHashArray();
    free(newPet);
    printf("\nPet saved");
    pressAny();
}
//Imprimir y mostrar la historia clinica:
void printClinicalHistory(int index){
    if(index < 0 || index >= dogAmount){
        printf("\nNumero de registro invalido");
        return;
    }
    char path[20];
    //Crear el nombre del archivo
    sprintf(path, "%i%s", (index+1), ".txt");
    historicalTxt = fopen(path, "r+");
    if(historicalTxt == NULL){
        historicalTxt = fopen(path, "w+");
        if(historicalTxt == NULL){
            perror("Error creando el archivo de historia para el registro");
            exit(-1);
        }else{
            //Creando archivo inicial de historia clinica
            fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);            
            struct dogType *pet;
            pet = malloc(sizeof(struct dogType));
            size_t response = fread(pet, sizeof(struct dogType), 1, dataDogs);
            fseek(historicalTxt, 0, SEEK_SET);
            //Escribiendo la historia clinica
            fwrite("Bienvenido a la historia clinica", sizeof(char), 32, historicalTxt);
            //Escribiendo cada propiedad
            
            fwrite("\nNombre: ", sizeof(char),9, historicalTxt);
            fwrite(pet->name, realLength(pet->name) , 1, historicalTxt);
            fwrite("\nTipo: ", sizeof(char),7, historicalTxt);
            fwrite(pet->type, realLength(pet->type) , 1, historicalTxt);
            fwrite("\nEdad: ",sizeof(char), 7, historicalTxt);
            fprintf(historicalTxt, "%i", pet->age);
            fwrite("\nRaza: ", sizeof(char),7, historicalTxt);
            fwrite(pet->race, realLength(pet->race) , 1, historicalTxt);
            fwrite("\nAltura: ", sizeof(char), 8, historicalTxt);
            fprintf(historicalTxt, "%i", pet->height);
            fwrite("\nPeso: ", sizeof(char), 7, historicalTxt);
            fprintf(historicalTxt, "%lf", pet->weight);
            fwrite("\nGenero: ", sizeof(char), 8, historicalTxt);
            fwrite(&pet->gender,sizeof(char), 1, historicalTxt);
            //Escribir espacio para las observaciones
            fwrite("\n\nObservaciones:", sizeof(char), 16, historicalTxt);
            free(pet);
        }
    }
    fclose(historicalTxt);
    //Abrir el archivo
    char gedit[30];
    sprintf(gedit, "%s%s", "gedit ", path);
    system(gedit);
}
//Mostrar una mascota
void printPet(int index){
    if(index < 0 || index >= dogAmount){
        printf("\nNumero de registro invalido");
        return;
    }
    printf("Mostrando el perro %d: \n",(index+1));    
    fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);            
    struct dogType *pet;
    pet = malloc(sizeof(struct dogType));
    size_t response = fread(pet, sizeof(struct dogType), 1, dataDogs);
	printf("\nNombre: %s", pet->name);
    printf("\nTipo: %s", pet->type);
    printf("\nEdad: %i", pet->age);
    printf("\nRaza: %s", pet->race);
    printf("\nAltura: %i", pet->height);
    printf("\nPeso: %lf", pet->weight);
    printf("\nGenero: %c \n", pet->gender);
    free(pet);
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
    printf("\nGet Pet");
	printf("\nHay %d perro(s), cual desea ver? ", dogAmount);
	int index = 0;
	scanf("%d",&index);
    //Corregimos para el indice insertado (arreglo inicia en 0)
    index = index-1;
    printPet(index);
    pressAny();
	return true;
}
//Remover una mascota
void removePet(int index){
    if(index < 0 || index >= dogAmount){
        printf("\nNumero de registro invalido");
        return;
    }
    //Corregir indices de lista enlazada
    struct dogType *removedPet;
    removedPet = malloc(sizeof(struct dogType));
    struct dogType *previousPet;
    previousPet = malloc(sizeof(struct dogType));
    struct dogType *nextPet;
    nextPet = malloc(sizeof(struct dogType));
    struct dogType *lastPet;
    lastPet = malloc(sizeof(struct dogType));

    fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);     
    fread(removedPet, sizeof(struct dogType), 1, dataDogs);

    //Si el perro a remover era cabeza del hash:
    if(removedPet->prevPet == -1){
        allHashPets[hashFunction(removedPet->name)] = (removedPet->nextPet);
        saveHashArray();
    }

    if(removedPet->prevPet != -1){
        //El anterior en la lista del removido, tiene de siguiente el siguiente del removido
        fseek(dataDogs, sizeof(struct dogType)* (removedPet->prevPet-1),SEEK_SET);
        fread(previousPet, sizeof(struct dogType), 1, dataDogs);
        previousPet->nextPet = removedPet->nextPet;
        fseek(dataDogs, sizeof(struct dogType)*(removedPet->prevPet-1),SEEK_SET);
        fwrite(previousPet, sizeof(struct dogType), 1, dataDogs);
    }
    if(removedPet->nextPet != -1){
        //El siguiente en la lista del removido, tiene de anterior el anterior del removido
        fseek(dataDogs, sizeof(struct dogType)*(removedPet->nextPet-1),SEEK_SET);
        fread(nextPet, sizeof(struct dogType), 1, dataDogs);
        nextPet->prevPet = removedPet->prevPet;
        fseek(dataDogs, sizeof(struct dogType)*(removedPet->nextPet-1),SEEK_SET);
        fwrite(nextPet, sizeof(struct dogType), 1, dataDogs);
    }

    if(index != (dogAmount-1)){
         //Voy al ultimo perro de la lista y lo muevo a la posicion del removido
        fseek(dataDogs, sizeof(struct dogType)*(dogAmount-1), SEEK_SET);
        fread(lastPet, sizeof(struct dogType), 1, dataDogs);

        //Antes de insertarlo en su nueva posicion, corregimos sus indices de lista enlazada (nextPet y prevPet ahora son los next y prev del ultimo perro)
        if(lastPet->prevPet != -1){
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->prevPet-1), SEEK_SET);
            fread(previousPet, sizeof(struct dogType), 1, dataDogs);
            previousPet->nextPet = index+1;
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->prevPet-1),SEEK_SET);
            fwrite(previousPet, sizeof(struct dogType), 1, dataDogs);
        }
        if(lastPet->nextPet != -1){
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->nextPet-1), SEEK_SET);
            fread(nextPet, sizeof(struct dogType), 1, dataDogs);
            nextPet->prevPet = index+1;
            fseek(dataDogs, sizeof(struct dogType)*(lastPet->nextPet-1),SEEK_SET);
            fwrite(nextPet, sizeof(struct dogType), 1, dataDogs);
        }
        //Insertamos el ultimo perro en la posicion del perro removido
        fseek(dataDogs, sizeof(struct dogType)*index, SEEK_SET);     
        fwrite(lastPet, sizeof(struct dogType), 1, dataDogs);

        //Si el ultimo perro era cabeza de lista:
        if(lastPet->prevPet == -1){
            allHashPets[hashFunction(lastPet->name)] = index+1;
            saveHashArray();
        }
        //Renombrar la historia clinica al nuevo ID
        char removedPath[40];
        char lastPath[40];
        sprintf(removedPath, "%i%s",index+1,".txt");
        sprintf(lastPath, "%i%s",dogAmount,".txt");
        remove(removedPath);
        rename(lastPath, removedPath);
    }else{
        //Quitando la historia clinica
        char removedPath[40];
        sprintf(removedPath, "%i%s",index+1,".txt");
        remove(removedPath);        
    }
    
    //disminuimos dogAmount
    dogAmount = dogAmount-1;
    savePetAmount();
    //Disminuir tamaño del archivo creando un archivo nuevo, copiando y renombrando
    FILE *tempfile;
    tempfile = fopen("Datacopy.dat","w+");
    fseek(dataDogs,0,SEEK_SET);
    int i;
    unsigned int tempvar;
    for (i=0; i < (dogAmount)*sizeof(struct dogType); i++){
        tempvar = getc(dataDogs);
        fputc(tempvar,tempfile);
    }   
    fclose(tempfile); 
    fclose(dataDogs);
    remove("dataDogs.dat");
    rename("Datacopy.dat","dataDogs.dat"); 
    dataDogs = fopen("dataDogs.dat","r+");

    free(removedPet);
    free(nextPet);
    free(previousPet);
    free(lastPet);
    printf("Eliminando el perro %d: \n", (index+1));    
}
//Metodo que llama a remove pet para borrar la mascota
bool deletePet(){
    printf("\nBorrando...");
	printf("\nHay %d perro(s), cual desea eliminar? ", dogAmount);
	int index = 0;
	scanf("%d",&index);
	removePet(index-1);
    pressAny();
    return true;
}
//Encontrar una mascota
void findPet(){
    printf("Ingrese el nombre que desea buscar (max 32 caracteres): ");
    char searchName[32];
    char petName[32];
    scanf("%s", searchName);
    int founds = 0;
    int headHash = allHashPets[hashFunction(searchName)];
    struct dogType *petFound;
    petFound = malloc(sizeof(struct dogType));
    while(headHash != -1){
        fseek(dataDogs, sizeof(struct dogType)*(headHash-1), SEEK_SET);
        fread(petFound, sizeof(struct dogType), 1, dataDogs);
        strcpy(petName,petFound->name);
        int i;
        for(i = 0; petName[i]; i++){
            petName[i] = tolower(petName[i]);
        }   
        for(i = 0; searchName[i]; i++){
            searchName[i] = tolower(searchName[i]);
        }  
        //TODO Validaciones
        if(strcmp(petName, searchName) == 0){
            founds = founds +1;
            //Se podría llamar a la función printPet, pero habría que hacer cambios
            printf("\nNúmero de registro: %i", headHash);
            printf("\nNombre: %s", petFound->name);
            printf("\nTipo: %s", petFound->type);
            printf("\nEdad: %i", petFound->age);
            printf("\nRaza: %s", petFound->race);
            printf("\nAltura: %i", petFound->height);
            printf("\nPeso: %lf", petFound->weight);
            printf("\nGenero: %c \n", petFound->gender); 
        }
        headHash = petFound->nextPet;
    }
    if(founds == 0){
        printf("\nNo se encontró ninguna coincidencia");
    }
    free(petFound);
    pressAny();
}
//Inicializar el programa
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
        fseek(hashDog, 0, SEEK_SET);
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
}
//Programa principal, Menú
int main(){
	initProgram();
	
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
    //close files
    fclose(dataDogs);
    fseek(hashDog,0,SEEK_SET);
    fwrite(allHashPets, sizeof(allHashPets),1,hashDog);
    fclose(hashDog);
    return 0;
}
