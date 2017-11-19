#include "generator.h"

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