#include <stdio.h>
#include <stdlib.h>

int NUMBER_OF_CUSTORMERS, NUMBER_OF_RESOURCES;

int contar(){
    FILE *file;
    int c;
    int last;
    int lines;

    file = fopen("customer.txt", "r");
    if (file == NULL)
        return -1;

    lines = 0;
    last = '\n';
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n')
            lines++;
        last = c;
    }
    if (last != '\n')
        lines++;

    fclose(file);
    return lines;
}

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "Quantidade de recursos incorretas");
        return 1;
    }
    FILE *file = fopen("customer.txt","r");
    FILE *file2 = fopen("commands.txt", "r");
    if (file == NULL){
        fprintf(stderr, "Ocorreu algum erro relacionado a customer.txt\n");
        return 1;
    }
    if (file2 == NULL){
        fprintf(stderr, "Ocorreu algum erro relacionado a commands.txt\n");
        return 1;
    }

    fclose(file);
    fclose(file2);

    NUMBER_OF_CUSTORMERS = contar();
    NUMBER_OF_RESOURCES = argc - 2;
    printf("Numero de recursos: %d\n", NUMBER_OF_RESOURCES);
    printf("Numero de clientes: %d\n", NUMBER_OF_CUSTORMERS);

    return 0;
}