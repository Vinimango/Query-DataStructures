#include "arvore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct no_arvore *inserir_na_arvore(struct no_arvore *raiz, const char *chave, void *registro )
{
    if (raiz == NULL) {
        struct no_arvore *novo = (struct no_arvore *)malloc(sizeof(struct no_arvore));
        if (!novo) {
            printf("Erro ao alocar memoria!\n");
            return NULL;
        }
        strncpy(novo->chave, chave, 99);
        novo->chave[99] = '\0';
        novo->registro = registro;
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }
    if (strcmp(chave,raiz->chave) < 0) {
        raiz->esq = inserir_na_arvore(raiz->esq, chave, registro);
    }
    else {
        raiz->dir = inserir_na_arvore(raiz->dir, chave, registro);
    }
    return raiz;
}

void percorrer_em_ordem(struct no_arvore *raiz, void (*imprimir)(void *registro))
{
    if (raiz) {
        percorrer_em_ordem(raiz->esq, imprimir);
        if (imprimir) {
            imprimir(raiz->registro);
        }
        percorrer_em_ordem(raiz->dir, imprimir);
    }
}

void *buscar_na_arvore(struct no_arvore *raiz, const char *chave) {
    if (!raiz) {
        return NULL;
    }
    int comparacao = strcmp(chave, raiz->chave);
    if ( comparacao == 0) {
        return raiz->registro;
    }
    else if (comparacao < 0 ) {
         return buscar_na_arvore(raiz->esq, chave);
    } else {
       return buscar_na_arvore(raiz->dir, chave);
    }

}

void liberar_arvore(struct no_arvore *raiz) {
    if (raiz) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        free(raiz);
    }
}



