#ifdef PET_H
#define PET_H
#endif

struct pet {
    int codigo;
    int codigo_pessoa;
    int codigo_tipo_pet;
    char nome[50];
};

struct no_pet {
    struct pet *dadoDoPet;
    struct no *prox;
    struct no *ant;
};

struct no_pet *carregar_pet(const char *nome_arquivo);


