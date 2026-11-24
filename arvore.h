#indef ARVORE_H
#define ARVORE_H

struct no_arvore {
    char chave[100];
    void *registro;
    struct no_arvore *esq;
    struct no_arvore *dir;
};

struct no_arvore *inserir_na_arvore(struct no_arvore *raiz, const char *chave, void *registro );

void percorrer_em_ordem(struct no_arvore *raiz, void (*imprimir)(void *registro));

void *buscar_na_arvore(struct no_arvore *raiz, const char *chave);

void liberar_arvore(struct no_arvore *raiz);

#endif
