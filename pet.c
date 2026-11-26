#include "pet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pessoa.h"
#include "tipo_pet.h"


struct no_pet *carregar_pet( const char *nome_arquivo, struct no_pessoa *listaPessoas, struct no_tipo_pet *listaTipoPet) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return NULL;
    }
    struct no_pet *listaPets = NULL;
    struct pet temp;

    while (fread(&temp, sizeof(struct pet), 1, arquivo) == 1) {
        if (!inserir_pet(&listaPets, &temp, listaPessoas, listaTipoPet))
            printf("Aviso: pet %d ignorado ao carregar (dono/tipo inexistente ou codigo repetido).\n", temp.codigo);
    }
    fclose(arquivo);
    return listaPets;
}

void salvar_pet(struct no_pet *listaPet, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (arquivo == NULL) {
        printf("Nao foi possivel escrever neste arquivo!\n");
        return;
    }
    struct no_pet *atual = listaPet;
    while (atual){
        fwrite(atual->dadoDoPet, sizeof(struct pet), 1, arquivo) ;
        atual = atual->prox;
    }
    fclose(arquivo);
}

struct no_pet *buscar_pet(struct no_pet *listaPets, int codigo) {
    struct no_pet *aux = listaPets;
    while (aux && aux->dadoDoPet->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return NULL;
    }
    return aux;
}

int inserir_pet(struct no_pet **listaPets, struct pet *dadosDoPet, struct no_pessoa *listaPessoas, struct no_tipo_pet *listaTipoPet) {
    if (buscar_pet(*listaPets, dadosDoPet->codigo) != NULL) {
        return 0;
    }
    if (buscar_pessoa(listaPessoas, dadosDoPet->codigo_pessoa) == NULL) {
        return 0;
    }
    if (buscar_tipo_pet(listaTipoPet, dadosDoPet->codigo_tipo_pet) == NULL) {
        return 0;
    }
    struct no_pet *novo = (struct no_pet *) malloc(sizeof(struct no_pet));
    if (!novo) {
        printf("Erro ao alocar memoria!\n");
        return 0;
    }
    novo->dadoDoPet = (struct pet *) malloc(sizeof(struct pet));
    if (novo->dadoDoPet == NULL) {
        printf("Erro ao alocar memoria!\n");
        free(novo);
        return 0;
    }
    *(novo->dadoDoPet) = *dadosDoPet;
    novo->prox = NULL;
    novo->ant = NULL;
    if (!*listaPets) {
        *listaPets = novo;
        return 1;
    }
    struct no_pet *ult = *listaPets;
    while (ult->prox) ult = ult->prox;
    ult->prox = novo;
    novo->ant = ult;
    return 1;


}

int alterar_pet(struct no_pet **listaPets, int codigo, struct pet *dadosDoPet) {
    struct no_pet *aux = *listaPets;
    while (aux && aux->dadoDoPet->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return 0;
    }
    *(aux->dadoDoPet) = *dadosDoPet;
    return 1;
}

int deletar_pet(struct no_pet **listaPets, int codigo) {
    struct no_pet *aux = *listaPets;
    while (aux && aux->dadoDoPet->codigo != codigo) {
        aux = aux->prox;
    }
    if (!aux) {
        return 0;
    }
    if (aux == *listaPets) {
        *listaPets = (*listaPets)->prox;
        if (*listaPets) {
            (*listaPets)->ant = NULL;
        }
    }
    else {
        aux->ant->prox = aux->prox;
        if (aux->prox) {
            aux->prox->ant = aux->ant;
        }
    }
    free(aux->dadoDoPet);
    free(aux);
    return 1;
}


