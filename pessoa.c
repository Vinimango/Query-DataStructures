#include "pessoa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct no_pessoa *carregar_pessoas(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");

    struct no_pessoa *lista_pessoas = NULL;

    if (arquivo == NULL) {
        return NULL;
    };
    struct pessoa temp;

    while (fread(&temp, sizeof(struct pessoa) ,1 ,arquivo ) == 1) {
        if (!inserir_pessoas(&lista_pessoas, &temp))
            printf("Aviso: pessoa %d ignorada ao carregar (codigo repetido).\n", temp.codigo);
    }
    fclose(arquivo);
    return lista_pessoas;
}

void salvar_pessoas(struct no_pessoa *lista_pessoas, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wb");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return;
    }
    struct no_pessoa *atual = lista_pessoas;
    while (atual) {
        fwrite(atual->dadosDaPessoa, sizeof(struct pessoa ), 1, arquivo);
        atual = atual->prox;
    }
    fclose(arquivo);

}


struct no_pessoa *buscar_pessoa(struct no_pessoa *lista_pessoas, int codigo) {
    struct no_pessoa *aux = lista_pessoas;
    while (aux && aux->dadosDaPessoa->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return NULL;
    }
    return aux;
}

int inserir_pessoas(struct no_pessoa **lista_pessoas, struct pessoa *pessoa) {
    if (buscar_pessoa(*lista_pessoas, pessoa->codigo) != NULL) {
        return 0;
    }
    struct no_pessoa *novo = (struct no_pessoa *) malloc(sizeof(struct no_pessoa));
    if (!novo) {
        printf("Erro ao alocar memoria!\n");
        return 0;
    }
    novo->dadosDaPessoa =(struct pessoa *) malloc(sizeof(struct pessoa));
    if (!novo->dadosDaPessoa) {
        free(novo);
        return 0;
    }
    *(novo->dadosDaPessoa) = *pessoa;
    novo->prox = NULL;
    novo->ant = NULL;
    if (!*lista_pessoas) {
        *lista_pessoas = novo;
        return 1;
    }
    struct no_pessoa *ult = *lista_pessoas;
    while (ult->prox) ult = ult->prox;
    ult->prox = novo;
    novo->ant = ult;
    return 1;
}

int alterar_pessoa(struct no_pessoa **lista_pessoas, int codigo, struct pessoa *pessoa) {
    struct no_pessoa *aux = *lista_pessoas;
    while (aux && aux->dadosDaPessoa->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return 0;
    }
    *(aux->dadosDaPessoa) = *pessoa;
    return 1;
}

int remover_pessoa(struct no_pessoa **lista_pessoas, int codigo) {
    struct no_pessoa *aux = *lista_pessoas;
    while (aux && aux->dadosDaPessoa->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return 0;
    }
    if (aux == *lista_pessoas) {
        *lista_pessoas = (*lista_pessoas)->prox;
        if (*lista_pessoas) {
            (*lista_pessoas)->ant = NULL;
        }
    }
    else {
        aux->ant->prox = aux->prox;
        if (aux->prox) {
            aux->prox->ant = aux->ant;
        }
    }
    free(aux->dadosDaPessoa);
    free(aux);
    return 1;
}












