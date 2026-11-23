#ifndef FILA_H
#define FILA_H

struct comando {
    char descricao[512];
};

struct no_fila {
    struct comando *comando;
    struct no_fila *prox;
};

struct fila{
    struct no_fila *comeco;
    struct no_fila *fim;
};

void inicializar(struct fila *f);

int fila_vazia(struct fila *f);

int enfileirar(struct fila *f, struct comando cmd);

int desenfileirar(struct fila *f, struct comando *cmd_saida);

void liberar_fila(struct fila *f);



#endif
