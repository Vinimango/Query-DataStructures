#ifndef FILA_H
#define FILA_H

struct comando {
    char descricao[512];
};

struct no_fila {
    struct comando *comando;
    struct no_fila *prox;
};

struct cabeca{
    struct no_fila *comeco;
    struct no_fila *fim;
};

void inicializar(struct cabeca *f);

int fila_vazia(struct cabeca *f);

int enfileirar(struct cabeca *f, struct comando *cmd);

int desenfileirar(struct cabeca *f, struct comando *cmd_saida);

void liberar_fila(struct cabeca *f);



#endif
