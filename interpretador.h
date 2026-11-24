

#ifndef QUERY_DATASTRUCTURES_INTERPRETADOR_H
#define QUERY_DATASTRUCTURES_INTERPRETADOR_H

#include "fila.h"
#include "pessoa.h"
#include "pet.h"
#include "tipo_pet.h"

int validar_comando(const char *descricao_comando);

void separar_comando_por_tabela(
    struct cabeca *filaComandos,
    struct cabeca *filaComandosTipoPet,
    struct cabeca *filaComandosPessoa,
    struct cabeca *filaComandosPet);

void processar_filas_operacoes_especificas(
        struct cabeca *filaTipoPet,
        struct cabeca *filaPessoa,
        struct cabeca *filaPet,
        struct no_tipo_pet **listaTiposPet,
        struct no_pessoa **listaPessoas,
        struct no_pet **listaPets);


#endif //QUERY_DATASTRUCTURES_INTERPRETADOR_H
