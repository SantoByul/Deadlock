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

int ler_customer(int **max){
    FILE *file;
    char line[256];
    char *p;
    char *end;
    int ok;

    file = fopen("customer.txt", "r");
    if (file == NULL)
        return 0;

    ok = 1;
    for (int i = 0; i < NUMBER_OF_CUSTORMERS && ok; i++){
        if (fgets(line, sizeof(line), file) == NULL){
            ok = 0;
            break;
        }
        p = line;
        for (int j = 0; j < NUMBER_OF_RESOURCES && ok; j++){
            max[i][j] = strtol(p, &end, 10);
            if (end == p || max[i][j] < 0){
                ok = 0;
                break;
            }
            p = end;
            if (j < NUMBER_OF_RESOURCES - 1){
                if (*p != ','){
                    ok = 0;
                    break;
                }
                p++;
            }
        }
        while (ok && (*p == '\r' || *p == '\n'))
            p++;
        if (ok && *p != '\0')
            ok = 0;
    }

    fclose(file);
    return ok;
}

void linha(FILE *out, int *v){
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if (i > 0)
            fprintf(out, " ");
        fprintf(out, "%d", v[i]);
    }
}

void imprimir(FILE *out, int **max, int **alloc, int **need, int *avail){
    fprintf(out, "MAXIMUM | ALLOCATION | NEED\n");
    for (int i = 0; i < NUMBER_OF_CUSTORMERS; i++){
        linha(out, max[i]);
        fprintf(out, " | ");
        linha(out, alloc[i]);
        fprintf(out, " | ");
        linha(out, need[i]);
        fprintf(out, "\n");
    }
    fprintf(out, "AVAILABLE ");
    linha(out, avail);
    fprintf(out, "\n");
}

int seguro(int **alloc, int **need, int *avail){
    int *work;
    int *finish;
    int achou, ok, total;

    work = malloc(NUMBER_OF_RESOURCES * sizeof(int));
    finish = calloc(NUMBER_OF_CUSTORMERS, sizeof(int));
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        work[i] = avail[i];

    total = 0;
    do {
        achou = 0;
        for (int i = 0; i < NUMBER_OF_CUSTORMERS; i++){
            if (finish[i])
                continue;
            ok = 1;
            for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
                if (need[i][j] > work[j]){
                    ok = 0;
                    break;
                }
            }
            if (ok){
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
                    work[j] += alloc[i][j];
                finish[i] = 1;
                total++;
                achou = 1;
            }
        }
    } while (achou);

    free(work);
    free(finish);
    return total == NUMBER_OF_CUSTORMERS;
}

void requisitar(FILE *out, int client, int *req, int **alloc, int **need, int *avail){
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if (req[i] > need[client][i]){
            fprintf(out, "The customer %d request ", client);
            linha(out, req);
            fprintf(out, " was denied because exceed its maximum allocation\n");
            return;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if (req[i] > avail[i]){
            fprintf(out, "The resources ");
            linha(out, avail);
            fprintf(out, " are not enough to customer %d request ", client);
            linha(out, req);
            fprintf(out, "\n");
            return;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        avail[i] -= req[i];
        alloc[client][i] += req[i];
        need[client][i] -= req[i];
    }

    if (!seguro(alloc, need, avail)){
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
            avail[i] += req[i];
            alloc[client][i] -= req[i];
            need[client][i] += req[i];
        }
        fprintf(out, "The customer %d request ", client);
        linha(out, req);
        fprintf(out, " was denied because result in an unsafe state\n");
        return;
    }

    fprintf(out, "Allocate to customer %d the resources ", client);
    linha(out, req);
    fprintf(out, "\n");
}

void liberar(FILE *out, int client, int *req, int **alloc, int **need, int *avail){
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if (req[i] > alloc[client][i]){
            fprintf(out, "The customer %d release ", client);
            linha(out, req);
            fprintf(out, " was denied because exceed its allocation\n");
            return;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        avail[i] += req[i];
        alloc[client][i] -= req[i];
        need[client][i] += req[i];
    }

    fprintf(out, "Release from customer %d the resources ", client);
    linha(out, req);
    fprintf(out, "\n");
}

int ler_comandos(FILE *out, int **max, int **alloc, int **need, int *avail){
    FILE *file = fopen("commands.txt", "r");
    if (file == NULL){
        return 0;
    }

    char line[256], *p, *end, tipo;
    int *req = malloc(NUMBER_OF_RESOURCES * sizeof(int));
    int client, ok = 1;

    while (ok && fgets(line, sizeof(line), file) != NULL){
        p = line;
        while (*p == ' ' || *p == '\t')
            p++;
        if (*p == '\n' || *p == '\r' || *p == '\0')
            continue;

        if (*p == '*'){
            p++;
            while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
                p++;
            if (*p != '\0')
                ok = 0;
            else
                imprimir(out, max, alloc, need, avail);
            continue;
        }

        if (p[0] != 'R' || (p[1] != 'Q' && p[1] != 'L') || p[2] != ' '){
            ok = 0;
            break;
        }
        tipo = p[1];
        p += 3;

        client = strtol(p, &end, 10);
        if (end == p || client < 0 || client >= NUMBER_OF_CUSTORMERS){
            ok = 0;
            break;
        }
        p = end;

        for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
            req[i] = strtol(p, &end, 10);
            if (end == p || req[i] < 0){
                ok = 0;
                break;
            }
            p = end;
        }
        if (!ok)
            break;

        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
            p++;
        if (*p != '\0'){
            ok = 0;
            break;
        }

        if (tipo == 'Q'){
            requisitar(out, client, req, alloc, need, avail);
        }
        else{
            liberar(out, client, req, alloc, need, avail);
        }
    }

    free(req);
    fclose(file);
    return ok;
}

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "Quantidade de recursos incorretas\n");
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

    int **max;

    max = malloc(NUMBER_OF_CUSTORMERS * sizeof(int *));
    for (int i = 0; i < NUMBER_OF_CUSTORMERS; i++){
        max[i] = malloc(NUMBER_OF_RESOURCES * sizeof(int));
    }

    if (!ler_customer(max)){
        fprintf(stderr, "Formato invalido em customer.txt (confira se o numero de colunas e igual ao numero de recursos informados)\n");
        return 1;
    }
    
    int *avail, **alloc, **need;

    if (NUMBER_OF_CUSTORMERS < 1){
        fprintf(stderr, "customer.txt esta vazio\n");
        return 1;
    }

    avail = malloc(NUMBER_OF_RESOURCES * sizeof(int));
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        char *end;
        avail[i] = strtol(argv[i + 1], &end, 10);

        if (*argv[i + 1] == '\0' || *end != '\0' || avail[i] < 0){
            fprintf(stderr, "Argumento invalido: %s\n", argv[i + 1]);
            return 1;
        }
    }

    alloc = malloc(NUMBER_OF_CUSTORMERS * sizeof(int *));
    need = malloc(NUMBER_OF_CUSTORMERS * sizeof(int *));
    for (int i = 0; i < NUMBER_OF_CUSTORMERS; i++){
        alloc[i] = calloc(NUMBER_OF_RESOURCES, sizeof(int));
        need[i] = malloc(NUMBER_OF_RESOURCES* sizeof(int));

        for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    FILE *file_res = fopen("result.txt", "w");
    if (file_res == NULL){
        fprintf(stderr, "Erro na criação do result.txt\n");
        return 1;
    }

    if (!ler_comandos(file_res, max, alloc, need, avail)){
        fprintf(stderr, "Formato invalido em commands.txt\n");
        fclose(file_res);
        return 1;
    }

    fclose(file_res);

    return 0;
}