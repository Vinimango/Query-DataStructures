#ifndef PET_H
#define PET_H
#include "pessoa.h"
#include "tipo_pet.h"

struct pet {
    int codigo;
    int codigo_pessoa;
    int codigo_tipo_pet;
    char nome[100];
};

struct no_pet {
    struct pet *dadoDoPet;
    struct no_pet *prox;
    struct no_pet *ant;
};

struct no_pet *carregar_pet(const char *nome_arquivo,struct no_pessoa *listaPessoas, struct no_tipo_pet *listaTipoPet);

void salvar_pet(struct no_pet *listaPets, const char *nome_arquivo);

struct no_pet *buscar_pet(struct no_pet *listaPets, int codigo);

int inserir_pet(struct no_pet **listaPets, struct pet *dadosDoPet, struct no_pessoa *pessoa, struct no_tipo_pet *tipo_pet);

int alterar_pet(struct no_pet **listaPets, int codigo, struct pet *dadosDoPet);

int deletar_pet(struct no_pet **listaPets, int codigo );


#endif