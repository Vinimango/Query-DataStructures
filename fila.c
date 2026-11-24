#include "fila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inicializar (struct cabeca *c) {
    if (c != NULL) {
        c->comeco = NULL;
        c->fim = NULL;
    }
}

int fila_vazia(struct cabeca *c) {
    if (c == NULL || c->comeco == NULL) {
        return 1;
    }
    return 0;
}

int enfileirar(struct cabeca *c, struct comando *cmd) {
    struct no_fila *novo = (struct no_fila*) malloc(sizeof(struct no_fila));
    if (!novo) {
        printf("Erro ao alocar memoria");
        return 0;
    }
    novo->comando = (struct comando*) malloc(sizeof(struct comando));
    if (!novo->comando) {
        printf("Erro ao alocar memoria");
        free(novo);
        return 0;
    }
    *(novo->comando) = *cmd;
    novo->prox = NULL;
    if (c->fim == NULL) {
        c->comeco = novo;
    } else {
        c->fim->prox = novo;
    }
    c->fim = novo;
    return 1;
}

int desenfileirar(struct cabeca *c, struct comando *cmd_saida) {
    if (fila_vazia(c)) {
        return 0;
    }
    struct no_fila *removido = c->comeco;
    if (cmd_saida) {
        *cmd_saida = *(removido->comando);
    }
    c->comeco = removido->prox;
    if (c->comeco == NULL) {
        c->fim = NULL;
    }
    free(removido->comando);
    free(removido);
    return 1;

}

void liberar_fila(struct cabeca *c) {
    while (!fila_vazia(c)) {
        desenfileirar(c, NULL);
    }
}

