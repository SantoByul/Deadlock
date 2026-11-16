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
    fclose(file);

    if (file2 == NULL){
        fprintf(stderr, "Ocorreu algum erro relacionado a commands.txt\n");
        return 1;
    }
    fclose(file2);

    NUMBER_OF_CUSTORMERS = contar();
    NUMBER_OF_RESOURCES = argc - 1;

    FILE *file_res = fopen("result.txt", "w");
    if (file_res == NULL){
        fprintf(stderr, "Erro na criação do result.txt");
        return 1;
    }

    fprintf(file_res,"Numero de recursos: %d\n", NUMBER_OF_RESOURCES);
    fprintf(file_res,"Numero de clientes: %d\n", NUMBER_OF_CUSTORMERS);
    fprintf(file_res, "AVAILABLE");
    for (int i = 1; i < argc; i++)
        fprintf(file_res, " %s", argv[i]);
    fprintf(file_res, "\n");


    fclose(file_res);


    return 0;
}