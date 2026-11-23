#ifndef PESSOA_H
#define PESSOA_H


struct pessoa {
    int codigo;
    char nome[100];
    char endereco[100];
    char telefone[15];
    char data_nascimento[15];
};

struct no_pessoa {
    struct pessoa *dadosDaPessoa;
    struct no_pessoa *prox;
    struct no_pessoa *ant;
};


struct no_pessoa *carregar_pessoas(const char *nome_arquivo);

void salvar_pessoas(struct no_pessoa *lista_pessoas, const char *nome_arquivo);

struct no_pessoa *buscar_pessoa(struct no_pessoa *lista_pessoa, int codigo);

int inserir_pessoas(struct no_pessoa **lista_pessoas, struct pessoa *dadosPessoa);

int alterar_pessoa(struct no_pessoa **lista_pessoa, int codigo, struct pessoa *dadosPessoa);

int remover_pessoa(struct no_pessoa **lista_pessoa, int codigo);

#endif