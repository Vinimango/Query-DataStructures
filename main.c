#include "interpretador.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char **argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    struct no_tipo_pet *tipos = carregar_tipos_pet("tipos_pet.dat");
    struct no_pessoa *pessoas = carregar_pessoas("pessoas.dat");
    struct no_pet *pets = carregar_pet("pets.dat", pessoas, tipos);

    FILE *in = argc > 1 ? fopen(argv[1], "r") : stdin;
    if (!in) {
        printf("Nao foi possivel abrir %s\n", argv[1]);
        return 1;
    }

    struct cabeca fila, filaTipo, filaPessoa, filaPet;
    inicializar(&fila);
    inicializar(&filaTipo);
    inicializar(&filaPessoa);
    inicializar(&filaPet);

    struct comando cmd;
    size_t n = 0;
    char aspa = 0;
    int ch, validos = 0, invalidos = 0;
    do {
        ch = fgetc(in);
        if (ch == EOF || (ch == ';' && !aspa)) {
            cmd.descricao[n] = '\0';
            n = 0;
            if (strspn(cmd.descricao, " ") == strlen(cmd.descricao)) continue;
            if (validar_comando(cmd.descricao)) {
                enfileirar(&fila, &cmd);
                validos++;
            } else {
                printf("Comando invalido: %s\n", cmd.descricao);
                invalidos++;
            }
            continue;
        }
        if (aspa ? ch == aspa : (ch == '\'' || ch == '"')) aspa = aspa ? 0 : (char) ch;
        if (n + 1 < sizeof cmd.descricao) cmd.descricao[n++] = (char) (ch == '\n' || ch == '\r' || ch == '\t' ? ' ' : ch);
    } while (ch != EOF);
    if (in != stdin) fclose(in);

    separar_comando_por_tabela(&fila, &filaTipo, &filaPessoa, &filaPet);
    int erros = processar_filas_operacoes_especificas(&filaTipo, &filaPessoa, &filaPet, &tipos, &pessoas, &pets);

    printf("\n========================================\n");
    printf("%d comandos: %d executados, %d com erro, %d invalidos\n",
           validos + invalidos, validos - erros, erros, invalidos);

    salvar_tipos_pet(tipos, "tipos_pet.dat");
    salvar_pessoas(pessoas, "pessoas.dat");
    salvar_pet(pets, "pets.dat");
    return 0;
}
