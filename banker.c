#include <stdio.h>
#include <stdlib.h>

int NUMBER_OF_CUSTORMERS, NUMBER_OF_RESOURCES;

int contar()
{
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
    NUMBER_OF_CUSTORMERS = contar();
    NUMBER_OF_RESOURCES = argc - 2;
    printf("Numero de recursos: %d\n", NUMBER_OF_RESOURCES);
    printf("Numero de clientes: %d\n", NUMBER_OF_CUSTORMERS);

    return 0;
}