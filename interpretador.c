#include "interpretador.h"
#include "arvore.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CAMPOS 8
#define TAM 128

enum op { OP_INVALIDO, OP_INSERT, OP_UPDATE, OP_DELETE, OP_SELECT };
enum tabela { T_TIPO_PET, T_PESSOA, T_PET, T_INVALIDA };

static const char *nomes_tabela[] = {"tipo_pet", "pessoa", "pet"};
static const char *colunas_padrao[3][MAX_CAMPOS] = {
    {"codigo", "nome"},
    {"codigo", "nome", "endereco", "telefone", "data_nascimento"},
    {"codigo", "codigo_pessoa", "codigo_tipo_pet", "nome"},
};
static const int qtd_colunas[] = {2, 5, 4};

struct bancos {
    struct no_tipo_pet **tipos;
    struct no_pessoa **pessoas;
    struct no_pet **pets;
};


static void minusculo(char *dst, const char *src, size_t n) {
    size_t i;
    for (i = 0; i + 1 < n && src[i]; i++) dst[i] = (char) tolower((unsigned char) src[i]);
    dst[i] = '\0';
}

static void normalizar_aspas(char *s) {
    char *w = s;
    for (char *r = s; *r; r++) {
        unsigned char *u = (unsigned char *) r;
        if (u[0] == 0xE2 && u[1] == 0x80 && (u[2] == 0x98 || u[2] == 0x99 || u[2] == 0x9C || u[2] == 0x9D)) {
            *w++ = '\'';
            r += 2;
        } else {
            *w++ = *r;
        }
    }
    *w = '\0';
}


static void aparar(char *s) {
    char *ini = s;
    while (isspace((unsigned char) *ini)) ini++;
    size_t n = strlen(ini);
    while (n && isspace((unsigned char) ini[n - 1])) n--;
    if (n >= 2 && (ini[0] == '\'' || ini[0] == '"') && ini[n - 1] == ini[0]) {
        ini++;
        n -= 2;
    }
    memmove(s, ini, n);
    s[n] = '\0';
}

/* divide s[0..len) por virgulas fora de aspas */
static int separar_lista(const char *s, size_t len, char out[][TAM], int max) {
    int k = 0;
    size_t j = 0;
    char aspa = 0;
    for (size_t i = 0; i <= len && k < max; i++) {
        char ch = i < len ? s[i] : ',';
        if (aspa) {
            if (ch == aspa) aspa = 0;
        } else if (ch == '\'' || ch == '"') {
            aspa = ch;
        } else if (ch == ',') {
            out[k][j] = '\0';
            aparar(out[k]);
            k++;
            j = 0;
            continue;
        }
        if (j + 1 < TAM) out[k][j++] = ch;
    }
    return k;
}


static enum op obter_op(const char *l) {
    while (isspace((unsigned char) *l)) l++;
    if (!strncmp(l, "insert ", 7)) return OP_INSERT;
    if (!strncmp(l, "update ", 7)) return OP_UPDATE;
    if (!strncmp(l, "delete ", 7)) return OP_DELETE;
    if (!strncmp(l, "select ", 7)) return OP_SELECT;
    return OP_INVALIDO;
}

static enum tabela obter_tabela(const char *l, enum op op) {
    const char *chave = op == OP_INSERT ? "into " : op == OP_UPDATE ? "update " : "from ";
    const char *p = strstr(l, chave);
    if (!p) return T_INVALIDA;
    p += strlen(chave);
    while (isspace((unsigned char) *p)) p++;
    for (int t = 0; t < T_INVALIDA; t++) {
        size_t n = strlen(nomes_tabela[t]);
        if (!strncmp(p, nomes_tabela[t], n) && !isalnum((unsigned char) p[n]) && p[n] != '_')
            return (enum tabela) t;
    }
    return T_INVALIDA;
}

static int obter_codigo_where(const char *l, int *codigo) {
    const char *w = strstr(l, " where ");
    if (!w) return 0;
    w += 7;
    while (isspace((unsigned char) *w)) w++;
    if (strncmp(w, "codigo", 6) != 0) return 0;
    w += 6;
    while (isspace((unsigned char) *w)) w++;
    if (*w != '=') return 0;
    *codigo = atoi(w + 1);
    return 1;
}

int validar_comando(const char *descricao_comando) {
    char l[sizeof(((struct comando *) 0)->descricao)];
    minusculo(l, descricao_comando, sizeof l);
    normalizar_aspas(l);
    enum op op = obter_op(l);
    int codigo;
    if (op == OP_INVALIDO || obter_tabela(l, op) == T_INVALIDA) return 0;
    switch (op) {
        case OP_INSERT: return strstr(l, "values") && strchr(l, '(') && strchr(l, ')');
        case OP_UPDATE: return strstr(l, " set ") && obter_codigo_where(l, &codigo);
        case OP_DELETE: return obter_codigo_where(l, &codigo);
        default: return !strstr(l, " where ") || obter_codigo_where(l, &codigo); /* select so filtra por codigo */
    }
}


static int aplicar_campo(enum tabela t, void *reg, const char *campo, const char *v) {
    char c[TAM];
    minusculo(c, campo, sizeof c);
    if (t == T_TIPO_PET) {
        struct tipo_pet *r = reg;
        if (!strcmp(c, "codigo")) r->codigo = atoi(v);
        else if (!strcmp(c, "nome") || !strcmp(c, "descricao")) snprintf(r->nome, sizeof r->nome, "%s", v);
        else return 0;
    } else if (t == T_PESSOA) {
        struct pessoa *r = reg;
        if (!strcmp(c, "codigo")) r->codigo = atoi(v);
        else if (!strcmp(c, "nome")) snprintf(r->nome, sizeof r->nome, "%s", v);
        else if (!strcmp(c, "endereco")) snprintf(r->endereco, sizeof r->endereco, "%s", v);
        else if (!strcmp(c, "telefone") || !strcmp(c, "fone")) snprintf(r->telefone, sizeof r->telefone, "%s", v);
        else if (!strcmp(c, "data_nascimento")) snprintf(r->data_nascimento, sizeof r->data_nascimento, "%s", v);
        else return 0;
    } else {
        struct pet *r = reg;
        if (!strcmp(c, "codigo")) r->codigo = atoi(v);
        else if (!strcmp(c, "codigo_pessoa") || !strcmp(c, "codigo_pes") || !strcmp(c, "codigo_cli") || !strcmp(c, "codigo_pet"))
            r->codigo_pessoa = atoi(v);
        else if (!strcmp(c, "codigo_tipo_pet") || !strcmp(c, "codigo_tipo")) r->codigo_tipo_pet = atoi(v);
        else if (!strcmp(c, "nome")) snprintf(r->nome, sizeof r->nome, "%s", v);
        else return 0;
    }
    return 1;
}


struct coluna { const char *titulo; int largura; };

static const struct coluna colunas_saida[3][5] = {
    {{"codigo", 6}, {"nome", 20}},
    {{"codigo", 6}, {"nome", 20}, {"fone", 14}, {"nascimento", 10}, {"endereco", 25}},
    {{"codigo", 6}, {"nome", 20}, {"dono", 20}, {"tipo", 15}},
};
static const int qtd_colunas_saida[] = {2, 5, 4};

static struct bancos *bancos_atual;
static int linhas_impressas;

static void celula(const char *s, int largura) {
    int extra = 0;
    for (const unsigned char *p = (const unsigned char *) s; *p; p++)
        if ((*p & 0xC0) == 0x80) extra++;
    printf("%-*s  ", largura + extra, s);
}

static void celula_int(int v, int largura) {
    char buf[16];
    snprintf(buf, sizeof buf, "%d", v);
    celula(buf, largura);
}

static void imprimir_cabecalho(enum tabela t) {
    linhas_impressas = 0;
    for (int i = 0; i < qtd_colunas_saida[t]; i++) celula(colunas_saida[t][i].titulo, colunas_saida[t][i].largura);
    printf("\n");
    for (int i = 0; i < qtd_colunas_saida[t]; i++) {
        for (int j = 0; j < colunas_saida[t][i].largura; j++) putchar('-');
        printf("  ");
    }
    printf("\n");
}

static void imprimir_rodape(void) {
    if (linhas_impressas == 0) printf("(nenhum registro)\n");
    else printf("(%d registro%s)\n", linhas_impressas, linhas_impressas > 1 ? "s" : "");
}

static void imprimir_tipo_pet(void *r) {
    struct tipo_pet *t = r;
    celula_int(t->codigo, 6);
    celula(t->nome, 20);
    printf("\n");
    linhas_impressas++;
}

static void imprimir_pessoa(void *r) {
    struct pessoa *p = r;
    celula_int(p->codigo, 6);
    celula(p->nome, 20);
    celula(p->telefone, 14);
    celula(p->data_nascimento, 10);
    celula(p->endereco, 25);
    printf("\n");
    linhas_impressas++;
}

static void imprimir_pet(void *r) {
    struct pet *p = r;
    struct no_pessoa *dono = buscar_pessoa(*bancos_atual->pessoas, p->codigo_pessoa);
    struct no_tipo_pet *tipo = buscar_tipo_pet(*bancos_atual->tipos, p->codigo_tipo_pet);
    celula_int(p->codigo, 6);
    celula(p->nome, 20);
    celula(dono ? dono->dadosDaPessoa->nome : "?", 20);
    celula(tipo ? tipo->dadosDoTipoPet->nome : "?", 15);
    printf("\n");
    linhas_impressas++;
}


static int existe(enum tabela t, int codigo, struct bancos *b) {
    if (t == T_TIPO_PET) return buscar_tipo_pet(*b->tipos, codigo) != NULL;
    if (t == T_PESSOA) return buscar_pessoa(*b->pessoas, codigo) != NULL;
    return buscar_pet(*b->pets, codigo) != NULL;
}

static int executar_insert(const char *orig, const char *l, enum tabela t, struct bancos *b) {
    char colunas[MAX_CAMPOS][TAM], valores[MAX_CAMPOS][TAM];
    size_t pos_values = (size_t) (strstr(l, "values") - l);
    const char *a1 = strchr(orig, '(');
    const char *a2 = strchr(orig + pos_values, '(');
    const char *f2 = strrchr(orig, ')');
    if (!a2 || !f2 || f2 < a2) {
        printf("Erro: VALUES mal formado.\n");
        return 0;
    }
    int nv = separar_lista(a2 + 1, (size_t) (f2 - a2 - 1), valores, MAX_CAMPOS);
    int nc;
    if (a1 && a1 < orig + pos_values) {
        const char *f1 = strchr(a1, ')');
        nc = separar_lista(a1 + 1, (size_t) (f1 - a1 - 1), colunas, MAX_CAMPOS);
    } else {
        nc = qtd_colunas[t];
        for (int i = 0; i < nc; i++) snprintf(colunas[i], TAM, "%s", colunas_padrao[t][i]);
    }
    if (nc != nv) {
        printf("Erro: %d colunas e %d valores.\n", nc, nv);
        return 0;
    }

    struct tipo_pet tp = {0};
    struct pessoa pe = {0};
    struct pet pt = {0};
    void *reg = t == T_TIPO_PET ? (void *) &tp : t == T_PESSOA ? (void *) &pe : (void *) &pt;
    int tem_codigo = 0;
    for (int i = 0; i < nc; i++) {
        if (!aplicar_campo(t, reg, colunas[i], valores[i])) {
            printf("Erro: coluna '%s' nao existe em %s.\n", colunas[i], nomes_tabela[t]);
            return 0;
        }
        char c[TAM];
        minusculo(c, colunas[i], sizeof c);
        tem_codigo |= !strcmp(c, "codigo");
    }
    const char *nome = t == T_TIPO_PET ? tp.nome : t == T_PESSOA ? pe.nome : pt.nome;
    int codigo = *(int *) reg;
    if (!tem_codigo || !nome[0]) {
        printf("Erro: codigo e nome sao obrigatorios.\n");
        return 0;
    }
    if (existe(t, codigo, b)) {
        printf("Erro: ja existe %s com codigo %d.\n", nomes_tabela[t], codigo);
        return 0;
    }
    if (t == T_PET && !buscar_pessoa(*b->pessoas, pt.codigo_pessoa)) {
        printf("Erro: dono (pessoa %d) nao cadastrado.\n", pt.codigo_pessoa);
        return 0;
    }
    if (t == T_PET && !buscar_tipo_pet(*b->tipos, pt.codigo_tipo_pet)) {
        printf("Erro: tipo de pet %d nao cadastrado.\n", pt.codigo_tipo_pet);
        return 0;
    }
    int ok = t == T_TIPO_PET ? inserir_tipo_pet(b->tipos, &tp)
           : t == T_PESSOA   ? inserir_pessoas(b->pessoas, &pe)
                             : inserir_pet(b->pets, &pt, *b->pessoas, *b->tipos);
    if (ok) printf("OK: %s %d inserido.\n", nomes_tabela[t], codigo);
    return ok;
}

static int executar_update(const char *orig, const char *l, enum tabela t, int codigo, struct bancos *b) {
    const char *ini = strstr(l, " set ") + 5;
    const char *fim = strstr(l, " where ");
    char pares[MAX_CAMPOS][TAM];
    int n = separar_lista(orig + (ini - l), (size_t) (fim - ini), pares, MAX_CAMPOS);

    if (!existe(t, codigo, b)) {
        printf("Erro: %s com codigo %d nao encontrado.\n", nomes_tabela[t], codigo);
        return 0;
    }
    struct tipo_pet tp;
    struct pessoa pe;
    struct pet pt;
    void *reg;
    if (t == T_TIPO_PET) {
        tp = *buscar_tipo_pet(*b->tipos, codigo)->dadosDoTipoPet;
        reg = &tp;
    } else if (t == T_PESSOA) {
        pe = *buscar_pessoa(*b->pessoas, codigo)->dadosDaPessoa;
        reg = &pe;
    } else {
        pt = *buscar_pet(*b->pets, codigo)->dadoDoPet;
        reg = &pt;
    }

    for (int i = 0; i < n; i++) {
        char *igual = strchr(pares[i], '=');
        if (!igual) {
            printf("Erro: esperado campo = valor em '%s'.\n", pares[i]);
            return 0;
        }
        *igual = '\0';
        aparar(pares[i]);
        aparar(igual + 1);
        if (!aplicar_campo(t, reg, pares[i], igual + 1)) {
            printf("Erro: coluna '%s' nao existe em %s.\n", pares[i], nomes_tabela[t]);
            return 0;
        }
    }
    if (*(int *) reg != codigo) {
        printf("Erro: o codigo nao pode ser alterado.\n");
        return 0;
    }
    if (t == T_PET && !buscar_pessoa(*b->pessoas, pt.codigo_pessoa)) {
        printf("Erro: dono (pessoa %d) nao cadastrado.\n", pt.codigo_pessoa);
        return 0;
    }
    if (t == T_PET && !buscar_tipo_pet(*b->tipos, pt.codigo_tipo_pet)) {
        printf("Erro: tipo de pet %d nao cadastrado.\n", pt.codigo_tipo_pet);
        return 0;
    }
    int ok = t == T_TIPO_PET ? alterar_tipo_pet(b->tipos, codigo, tp.nome)
           : t == T_PESSOA   ? alterar_pessoa(b->pessoas, codigo, &pe)
                             : alterar_pet(b->pets, codigo, &pt);
    if (ok) printf("OK: %s %d alterado.\n", nomes_tabela[t], codigo);
    return ok;
}

static int executar_delete(enum tabela t, int codigo, struct bancos *b) {
    if (!existe(t, codigo, b)) {
        printf("Erro: %s com codigo %d nao encontrado.\n", nomes_tabela[t], codigo);
        return 0;
    }
    if (t != T_PET) {
        for (struct no_pet *p = *b->pets; p; p = p->prox) {
            int ref = t == T_PESSOA ? p->dadoDoPet->codigo_pessoa : p->dadoDoPet->codigo_tipo_pet;
            if (ref == codigo) {
                printf("Erro: %s %d ainda tem pets cadastrados.\n", nomes_tabela[t], codigo);
                return 0;
            }
        }
    }
    int ok = t == T_TIPO_PET ? remover_tipo_pet(b->tipos, codigo)
           : t == T_PESSOA   ? remover_pessoa(b->pessoas, codigo)
                             : deletar_pet(b->pets, codigo);
    if (ok) printf("OK: %s %d removido.\n", nomes_tabela[t], codigo);
    return ok;
}

static int executar_select(const char *l, enum tabela t, struct bancos *b) {
    void (*imprimir)(void *) = t == T_TIPO_PET ? imprimir_tipo_pet : t == T_PESSOA ? imprimir_pessoa : imprimir_pet;
    int codigo;
    int filtrar = obter_codigo_where(l, &codigo);
    int por_nome = strstr(l, "order by nome") != NULL;

    bancos_atual = b;
    imprimir_cabecalho(t);

    struct no_arvore *raiz = NULL;
    char chave[100];
#define VISITAR(reg, cod, nome)                                          \
    do {                                                                 \
        if (filtrar && (cod) != codigo) break;                           \
        if (por_nome) snprintf(chave, sizeof chave, "%s", nome);         \
        else snprintf(chave, sizeof chave, "%011d", (cod) + 1000000000); \
        raiz = inserir_na_arvore(raiz, chave, reg);                      \
    } while (0)

    if (t == T_TIPO_PET)
        for (struct no_tipo_pet *n = *b->tipos; n; n = n->prox) VISITAR(n->dadosDoTipoPet, n->dadosDoTipoPet->codigo, n->dadosDoTipoPet->nome);
    else if (t == T_PESSOA)
        for (struct no_pessoa *n = *b->pessoas; n; n = n->prox) VISITAR(n->dadosDaPessoa, n->dadosDaPessoa->codigo, n->dadosDaPessoa->nome);
    else
        for (struct no_pet *n = *b->pets; n; n = n->prox) VISITAR(n->dadoDoPet, n->dadoDoPet->codigo, n->dadoDoPet->nome);
#undef VISITAR

    percorrer_em_ordem(raiz, imprimir);
    liberar_arvore(raiz);
    imprimir_rodape();
    return 1;
}

static int executar_comando(const char *cmd, struct bancos *b) {
    char orig[sizeof(((struct comando *) 0)->descricao)], l[sizeof orig];
    snprintf(orig, sizeof orig, "%s", cmd);
    normalizar_aspas(orig);
    minusculo(l, orig, sizeof l);
    enum op op = obter_op(l);
    enum tabela t = obter_tabela(l, op);
    int codigo = 0;
    obter_codigo_where(l, &codigo);

    printf("\n> %s\n", orig);
    switch (op) {
        case OP_INSERT: return executar_insert(orig, l, t, b);
        case OP_UPDATE: return executar_update(orig, l, t, codigo, b);
        case OP_DELETE: return executar_delete(t, codigo, b);
        case OP_SELECT: return executar_select(l, t, b);
        default: printf("Erro: comando invalido.\n"); return 0;
    }
}

void separar_comando_por_tabela(
    struct cabeca *filaComandos,
    struct cabeca *filaComandosTipoPet,
    struct cabeca *filaComandosPessoa,
    struct cabeca *filaComandosPet) {
    struct cabeca *destinos[] = {filaComandosTipoPet, filaComandosPessoa, filaComandosPet};
    struct comando cmd;
    while (desenfileirar(filaComandos, &cmd)) {
        char l[sizeof cmd.descricao];
        minusculo(l, cmd.descricao, sizeof l);
        enum tabela t = obter_tabela(l, obter_op(l));
        if (t == T_INVALIDA) printf("Erro: tabela invalida em '%s'.\n", cmd.descricao);
        else enfileirar(destinos[t], &cmd);
    }
}

int processar_filas_operacoes_especificas(
        struct cabeca *filaTipoPet,
        struct cabeca *filaPessoa,
        struct cabeca *filaPet,
        struct no_tipo_pet **listaTiposPet,
        struct no_pessoa **listaPessoas,
        struct no_pet **listaPets) {
    struct bancos b = {listaTiposPet, listaPessoas, listaPets};
    struct cabeca *filas[] = {filaTipoPet, filaPessoa, filaPet};
    struct comando cmd;
    int erros = 0;
    for (int i = 0; i < 3; i++)
        while (desenfileirar(filas[i], &cmd)) erros += !executar_comando(cmd.descricao, &b);
    return erros;
}
