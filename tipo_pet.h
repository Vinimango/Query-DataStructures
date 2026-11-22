#ifndef TIPO_PET_H
#define TIPO_PET_H
#endif


struct tipo_pet {
    int codigo;
    char nome[50];
};

struct no_tipo_pet {
    struct tipo_pet *dadosDoTipoPet;
    struct no_tipo_pet *prox;
    struct no_tipo_pet *ant;
};

struct no_tipo_pet *carregar_tipos_pet(const char *nome_arquivo);

void salvar_tipos_pet(struct no_tipo_pet *lista_tipos_pet, const char *nome_arquivo);

struct no_tipo_pet *buscar_tipo_pet(struct no_tipo_pet *lista, int codigo);

int inserir_tipo_pet(struct no_tipo_pet **lista_tipos_pet, struct tipo_pet *dados_tipo_pet);

int alterar_tipo_pet(struct no_tipo_pet **lista_tipos_pet, int codigo, const char *novo_nome);

int remover_tipo_pet(struct no_tipo_pet **lista_tipos_pet, int codigo);