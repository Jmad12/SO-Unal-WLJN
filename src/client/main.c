#include "client.h"

int main(int argc, char *argv[]){
    struct sockaddr_in client;
    struct hostent *he;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0){
        perror("\n Error abriendo el socket");
        exit(1);
    }

    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    bzero(client.sin_zero, 8);
    inet_aton(argv[1], &client.sin_addr);

    r = connect(clientfd, (struct sockaddr *)&client, (socklen_t)sizeof(struct sockaddr));
    if(r < 0){
        perror("\n Error conectando al servidor");
        exit(-1);
    }
    
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

    struct requestType request;
    strcpy(request.action,"SALIR");
    r = send(clientfd, &request, sizeof(request), 0);

    close(clientfd);
    return 0;
}

//Funcion para presionar cualquier tecla
void pressAny(){
    char ch;
    printf("\nPresione ENTER para continuar");
    while(getchar()!='\n'); // limpia stdin
    getchar(); // espera un ENTER
    printf("\n");
}