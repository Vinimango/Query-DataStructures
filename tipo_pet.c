#include "tipo_pet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct no_tipo_pet *carregar_tipos_pet(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");

    struct no_tipo_pet *inicio_lista = NULL;

    if (arquivo == NULL) {
        return NULL;
    }

    struct tipo_pet temp;

    while (fread(&temp, sizeof(struct tipo_pet), 1, arquivo) == 1) {

        inserir_tipo_pet(&inicio_lista, &temp);
    }

    fclose(arquivo);
    return inicio_lista;
}


void salvar_tipos_pet(struct no_tipo_pet *lista, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wb");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return;
    }
    struct no_tipo_pet *atual = lista;
    while (atual != NULL) {
        fwrite(atual->dadosDoTipoPet, sizeof(struct tipo_pet), 1, arquivo);
        atual = atual->prox;
    }

    fclose(arquivo);
}


struct no_tipo_pet *buscar_tipo_pet (struct no_tipo_pet *lista, int codigo) {

    struct no_tipo_pet *aux= lista;
    while (aux && aux->dadosDoTipoPet->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return NULL;
    }
    return aux;
}

int inserir_tipo_pet(struct no_tipo_pet **lista, struct tipo_pet *pet) {

    if (buscar_tipo_pet(*lista, pet->codigo) != NULL) {
        printf("Erro: Codigo %d ja cadastrado!\n", pet->codigo);
        return 0;
    }


    struct no_tipo_pet *novo = (struct no_tipo_pet *) malloc(sizeof(struct no_tipo_pet));
    if (!novo) {
        printf("Erro ao alocar memoria!\n");
        return 0;
    }
    novo->dadosDoTipoPet = (struct tipo_pet *) malloc(sizeof(struct tipo_pet));
    if (!novo->dadosDoTipoPet) {
        free(novo);
        return 0;
    }
    *(novo->dadosDoTipoPet) = *pet;
    novo->ant = NULL;
    novo->prox = *lista;
    if (*lista) {
        (*lista)->ant = novo;
    }
    *lista = novo;
    return 1;
}

int alterar_tipo_pet(struct no_tipo_pet **lista, int codigo,const char *novo_nome) {
    struct no_tipo_pet *aux = *lista;
    while (aux && aux->dadosDoTipoPet->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        printf("Não foi possivel encontrar este tipo de pet!\n");
        return 0;
    }
    strcpy(aux->dadosDoTipoPet->nome, novo_nome);
    return 1;
}

int remover_tipo_pet(struct no_tipo_pet **lista, int codigo) {
    struct no_tipo_pet *aux = *lista;
    while (aux && aux->dadosDoTipoPet->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        printf("Não foi possivel remover este tipo de pet!\n");
        return 0;
    }

    if (aux->dadosDoTipoPet-> codigo == (*lista)->dadosDoTipoPet->codigo)//Preciso remover o primeiro elemento dessa lista
    {
        *lista = (*lista)->prox;
        if (*lista) {
            (*lista)->ant = NULL;
        }
    }
    else {
        aux->ant->prox = aux->prox;
        if (aux->prox) {
            aux->prox->ant = aux->ant;
        }
    }
    free(aux->dadosDoTipoPet);
    free(aux);
    return 1;
}