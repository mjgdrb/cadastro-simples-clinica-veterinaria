/* ========================================================================== */
/*           SISTEMA DE CADASTRO DE PACIENTES PARA CLÍNICA VETERINÁRIA         */
/* ========================================================================== */
/* Descrição: Sistema em Linguagem C para gerenciamento de pacientes (CRUD),   */
/* com suporte a persistência de dados em arquivos texto (CSV), validação de  */
/* entradas, geração automática de códigos e interface colorida via terminal. */
/* ========================================================================== */

// BIBLIOTECAS UTILIZADAS
#include <stdio.h>     // Biblioteca padrão de entrada e saída (printf, fgets, fopen, etc.)
#include <stdbool.h>   // Suporte ao tipo de dado booleano (true/false)
#include <locale.h>    // Configuração de localização/idioma (acentuação em português)
#include <string.h>    // Manipulação de strings (strcpy, strcmp, strstr, strlen)
#include <stdlib.h>    // Funções utilitárias (system, etc.)
#include <ctype.h>     // Funções de manipulação de caracteres (tolower, toupper, isalpha)
#include <time.h>      // Manipulação de data/hora para geração automática de códigos

// Configuração de compatibilidade para cores no terminal dependendo do Sistema Operacional
#ifdef _WIN32
    #include <windows.h> 
#else
    #include <unistd.h>  
#endif

// DEFINIÇÃO DE CORES (ANSI Escape Sequences) PARA O TERMINAL
#define RESET    "\033[0m"
#define NEGRITO  "\033[1m"
#define VERMELHO "\033[1;31m"
#define VERDE    "\033[1;32m"
#define AMARELO  "\033[1;33m"
#define AZUL     "\033[1;34m"
#define MAGENTA  "\033[1;35m"
#define CIANO    "\033[1;36m"

// ESTRUTURA DE DADOS (STRUCT) DO PACIENTE
// Representa o domínio da aplicação com mais de 4 campos de tipos variados
typedef struct {
    char Codigo[20];       // Identificador único no formato AAMMNN (ex: 261101)
    char NomeDono[50];     // Nome do proprietário do animal
    char Telefone[30];     // Telefone de contato
    char Especie[50];      // Espécie do animal (Cão, Gato, etc.)
    char Nome[50];         // Nome do animal
    char Idade[30];        // Idade (ex: "2 anos", "8 meses")            
    char Sexo[10];         // Sexo ("Femea" ou "Macho")
    char Raca[50];         // Raça do animal
    char Peso[30];         // Peso (ex: "4.5 kg", "300 gramas")                  
    char Castrado[30];     // Status de castração ("Castrado" ou "Nao Castrado")
} Paciente;

// VARIÁVEIS GLOBAIS DE CONTROLE DO SISTEMA
int opcao;
bool parar = false;
const char *nomeArquivo = "pacientes.csv";          // Arquivo de persistência principal (CSV)
const char *nomeArquivoContador = "contador.txt";   // Arquivo auxiliar para controle de IDs sequenciais
FILE *arquivo;
Paciente paciente;

// ========================================================================== */
//                     FUNÇÕES AUXILIARES DE VALIDAÇÃO E FORMATAÇÃO            */
// ========================================================================== */

// Converte todos os caracteres de uma string para letras minúsculas (auxilia nas buscas)
void paraMinusculo(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Formata strings em Title Case (Ex: "joão silva" vira "João Silva") apenas se contiver letras
void formatarSeApenasLetras(char *str) {
    bool temLetra = false;

    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (isalpha(c)) {
            temLetra = true;
        } else if (!isspace(c)) {
            // Se contiver números ou símbolos (ex: "4.5 kg"), não aplica formatação de nomes
            return;
        }
    }

    if (!temLetra) return;

    bool inicioPalavra = true;
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (isspace(c)) {
            inicioPalavra = true;
        } else if (inicioPalavra) {
            str[i] = toupper(c);
            inicioPalavra = false;
        } else {
            str[i] = tolower(c);
        }
    }
}

// Habilita suporte a códigos de cores ANSI nativos no prompt de comando do Windows
void ativarCoresWindows() {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004; // Habilita ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, dwMode);
    #endif
}

// Verifica se o usuário digitou o comando de cancelamento ("cancel")
bool ehComandoCancelar(const char *entrada) {
    char copia[100];
    strncpy(copia, entrada, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    char *inicio = copia;
    while (isspace((unsigned char)*inicio)) inicio++;

    char *fim = inicio + strlen(inicio) - 1;
    while (fim > inicio && isspace((unsigned char)*fim)) *fim-- = '\0';

    paraMinusculo(inicio);
    return strcmp(inicio, "cancel") == 0;
}

// Lê e valida números inteiros dentro de um intervalo específico (evita loops infinitos)
int lerInteiroValido(const char *prompt, int min, int max) {
    char buffer[100];
    int valor;
    char extra;

    while (true) {
        printf(AMARELO "%s" RESET, prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }

        if (sscanf(buffer, " %d %c", &valor, &extra) == 1) {
            if (valor >= min && valor <= max) {
                return valor;
            }
            printf(VERMELHO "=== OPÇÃO INVÁLIDA! Escolha um número entre %d e %d. ===\n\n" RESET, min, max);
        } else {
            printf(VERMELHO "=== ENTRADA INVÁLIDA! Digite apenas números inteiros. ===\n\n" RESET);
        }
    }
}

// Lê um caractere válido restrito a um conjunto de opções permitidas
char lerCharValido(const char *prompt, const char *opcoesValidas, const char *mensagemErro) {
    char buffer[100];
    char c, extra;

    while (true) {
        printf(AMARELO "%s" RESET, prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }

        if (sscanf(buffer, " %c %c", &c, &extra) == 1) {
            if (strchr(opcoesValidas, c) != NULL) {
                return c;
            }
        }
        printf(VERMELHO "=== %s ===\n\n" RESET, mensagemErro);
    }
}

// Lê um caractere permitindo a interrupção/cancelamento da operação
char lerCharCancelavel(const char *prompt, const char *opcoesValidas, const char *mensagemErro) {
    char buffer[100];
    char c, extra;

    while (true) {
        printf(CIANO "%s" RESET, prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        buffer[strcspn(buffer, "\n")] = '\0';

        if (ehComandoCancelar(buffer)) {
            return '\0'; // Retorna nulo indicando cancelamento
        }

        if (sscanf(buffer, " %c %c", &c, &extra) == 1) {
            if (strchr(opcoesValidas, c) != NULL) {
                return c;
            }
        }
        printf(VERMELHO "=== %s ===\n\n" RESET, mensagemErro);
    }
}

// Lê texto seguro com validação para impedir campos vazios e substitui vírgulas por pontos (para o CSV)
void lerTextoSeguro(const char *prompt, char *destino, int tamanho) {
    while (true) {
        printf(CIANO "%s" RESET, prompt);
        if (fgets(destino, tamanho, stdin) == NULL) {
            continue;
        }
        destino[strcspn(destino, "\n")] = '\0';

        for (int i = 0; destino[i]; i++) {
            if (destino[i] == ',') {
                destino[i] = '.'; // Previne quebra do formato CSV
            }
        }

        if (strlen(destino) > 0) {
            return;
        }
        printf(VERMELHO "=== ESTE CAMPO NÃO PODE FICAR VAZIO! ===\n\n" RESET);
    }
}

// Lê texto permitindo comando de cancelamento
bool lerTextoCancelavel(const char *prompt, char *destino, int tamanho) {
    while (true) {
        printf(CIANO "%s" RESET, prompt);
        if (fgets(destino, tamanho, stdin) == NULL) {
            continue;
        }
        destino[strcspn(destino, "\n")] = '\0';

        if (ehComandoCancelar(destino)) {
            return false;
        }

        for (int i = 0; destino[i]; i++) {
            if (destino[i] == ',') {
                destino[i] = '.';
            }
        }

        if (strlen(destino) > 0) {
            return true;
        }
        printf(VERMELHO "=== ESTE CAMPO NÃO PODE FICAR VAZIO! ===\n\n" RESET);
    }
}

// ========================================================================== */
//                 GERENCIAMENTO DE ID E PERSISTÊNCIA EM ARQUIVOS             */
// ========================================================================== */

// Gera automaticamente o código no formato AAMMNN (ex: 261101 = ano 26, mes 11, sequencia 01)
void gerarProximoCodigo(char *codigoDestino) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int anoAtual = tm.tm_year % 100; // Últimos 2 dígitos do ano
    int mesAtual = tm.tm_mon + 1;    // Mês atual (1 a 12)

    int anoSalvo = 0;
    int mesSalvo = 0;
    int sequencia = 0;

    // Lê o estado atual do contador no arquivo auxiliar
    FILE *fCounter = fopen(nomeArquivoContador, "r");
    if (fCounter != NULL) {
        fscanf(fCounter, "%d %d %d", &anoSalvo, &mesSalvo, &sequencia);
        fclose(fCounter);
    }

    // Se mudou o mês ou o ano, reinicia a sequência para 1; caso contrário, incrementa
    if (anoSalvo == anoAtual && mesSalvo == mesAtual) {
        sequencia++;
    } else {
        sequencia = 1; 
    }

    // Salva o novo estado no arquivo auxiliar
    fCounter = fopen(nomeArquivoContador, "w");
    if (fCounter != NULL) {
        fprintf(fCounter, "%d %d %d", anoAtual, mesAtual, sequencia);
        fclose(fCounter);
    }

    // Formata a string final do código com zeros à esquerda (ex: 261101)
    sprintf(codigoDestino, "%02d%02d%02d", anoAtual, mesAtual, sequencia);
}

// CARREGAR DADOS DO ARQUIVO CSV PARA O VETOR EM MEMÓRIA (Read / Inicialização)
int carregarPacientes(Paciente lista[], int maxItens) {
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return -1; // Arquivo ainda não existe
    }

    int total = 0;
    char linha[800];
    fgets(linha, sizeof(linha), arquivo); // Ignora a linha de cabeçalho do CSV

    while (fgets(linha, sizeof(linha), arquivo) && total < maxItens) {
        linha[strcspn(linha, "\n")] = '\0';

        // Faz o parsing da linha separada por vírgulas
        char *codigo   = strtok(linha, ",");
        char *nomeDono = strtok(NULL, ",");
        char *telefone = strtok(NULL, ",");
        char *especie  = strtok(NULL, ",");
        char *nome     = strtok(NULL, ",");
        char *idade    = strtok(NULL, ",");
        char *sexo     = strtok(NULL, ",");
        char *raca     = strtok(NULL, ",");
        char *peso     = strtok(NULL, ",");
        char *castrado = strtok(NULL, ",");

        if (codigo && nomeDono && telefone && especie && nome && idade && sexo && raca && peso && castrado) {
            strcpy(lista[total].Codigo, codigo);
            strcpy(lista[total].NomeDono, nomeDono);
            strcpy(lista[total].Telefone, telefone);
            strcpy(lista[total].Especie, especie);
            strcpy(lista[total].Nome, nome);
            strcpy(lista[total].Idade, idade);
            strcpy(lista[total].Sexo, sexo);
            strcpy(lista[total].Raca, raca);
            strcpy(lista[total].Peso, peso);
            strcpy(lista[total].Castrado, castrado);
            total++;
        }
    }

    fclose(arquivo);
    return total; // Retorna o total de registros carregados com sucesso
}

// SALVA TODOS OS REGISTROS DO VETOR DE VOLTA PARA O ARQUIVO (Write / Persistência)
bool salvarTodosPacientes(Paciente lista[], int total, int indiceIgnorar) {
    arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        return false;
    }

    // Escreve o cabeçalho padrão do CSV
    fprintf(arquivo, "Codigo,NomeDono,Telefone,Especie,Nome,Idade,Sexo,Raca,Peso,Castrado\n");
    
    for (int i = 0; i < total; i++) {
        // Se indiceIgnorar for válido, pula aquele registro específico (usado na exclusão)
        if (i != indiceIgnorar) {
            fprintf(arquivo, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                    lista[i].Codigo,
                    lista[i].NomeDono,
                    lista[i].Telefone,
                    lista[i].Especie,
                    lista[i].Nome,
                    lista[i].Idade,
                    lista[i].Sexo,
                    lista[i].Raca,
                    lista[i].Peso,
                    lista[i].Castrado);
        }
    }

    fclose(arquivo);
    return true;
}

// ========================================================================== */
//                     FUNÇÕES DE INTERFACE E EXIBIÇÃO                         */
// ========================================================================== */

void exibirAvisoSemCadastro(const char *tituloSecao) {
    printf(MAGENTA "\n=== %s ===\n" RESET, tituloSecao);
    printf(AMARELO "\nAviso: Não há nenhum paciente cadastrado no sistema.\n" RESET);
    printf(CIANO "\nPressione ENTER para voltar ao menu..." RESET);
    getchar();
}

// Exibe os dados detalhados de um paciente formatados em formato de "Card"
void imprimirCardPaciente(int numero, Paciente p) {
    printf(AMARELO "\nPaciente #%d (Código: %s)\n" RESET, numero, p.Codigo);
    printf(CIANO "  Nome do Dono: " RESET "%s\n", p.NomeDono);
    printf(CIANO "  Telefone:     " RESET "%s\n", p.Telefone);
    printf(CIANO "  Espécie:      " RESET "%s\n", p.Especie);
    printf(CIANO "  Nome:         " RESET "%s\n", p.Nome);
    printf(CIANO "  Idade:        " RESET "%s\n", p.Idade);
    printf(CIANO "  Sexo:         " RESET "%s\n", p.Sexo);
    printf(CIANO "  Raça:         " RESET "%s\n", p.Raca);
    printf(CIANO "  Peso:         " RESET "%s\n", p.Peso);
    printf(CIANO "  Castrado?:    " RESET "%s\n", p.Castrado);
    printf(AZUL "-------------------------------------------------------------\n" RESET);
}

// Função de busca unificada (por Código, Nome do Paciente, Nome do Dono ou Espécie) com suporte a múltiplos resultados
int buscarIndicePaciente(Paciente lista[], int total, const char *acaoTexto) {
    printf("\t" CIANO "1" RESET " - Pesquisar por Código de Cadastro\n");
    printf("\t" CIANO "2" RESET " - Pesquisar por Nome do Paciente\n");
    printf("\t" CIANO "3" RESET " - Pesquisar por Nome do Dono\n");
    printf("\t" CIANO "4" RESET " - Pesquisar por Espécie\n");
    printf("\t" VERMELHO "0" RESET " - Cancelar e voltar ao menu\n\n");

    int tipoBusca = lerInteiroValido("Como deseja localizar o paciente? ", 0, 4);
    if (tipoBusca == 0) {
        printf(CIANO "\nOperação cancelada.\n" RESET);
        return -1;
    }

    char termoBusca[50];
    if (tipoBusca == 1) {
        lerTextoSeguro("\nDigite o Código de Cadastro (ex: 261101): ", termoBusca, sizeof(termoBusca));
    } else if (tipoBusca == 2) {
        lerTextoSeguro("\nDigite o Nome do paciente: ", termoBusca, sizeof(termoBusca));
    } else if (tipoBusca == 3) {
        lerTextoSeguro("\nDigite o Nome do dono: ", termoBusca, sizeof(termoBusca));
    } else {
        lerTextoSeguro("\nDigite a Espécie do paciente: ", termoBusca, sizeof(termoBusca));
    }

    char termoLower[50];
    strcpy(termoLower, termoBusca);
    paraMinusculo(termoLower);

    int indicesEncontrados[500];
    int qtdEncontrados = 0;

    // Varre o vetor procurando correspondências parciais ou totais (strstr)
    for (int i = 0; i < total; i++) {
        char campoComparar[50];
        if (tipoBusca == 1) {
            strcpy(campoComparar, lista[i].Codigo);
        } else if (tipoBusca == 2) {
            strcpy(campoComparar, lista[i].Nome);
        } else if (tipoBusca == 3) {
            strcpy(campoComparar, lista[i].NomeDono);
        } else {
            strcpy(campoComparar, lista[i].Especie);
        }
        paraMinusculo(campoComparar);

        if (strstr(campoComparar, termoLower) != NULL) {
            indicesEncontrados[qtdEncontrados++] = i;
        }
    }

    if (qtdEncontrados == 0) {
        printf(AMARELO "\nNenhum paciente encontrado com \"%s\".\n" RESET, termoBusca);
        return -1;
    }

    if (qtdEncontrados == 1) {
        return indicesEncontrados[0]; // Retorna diretamente se houver apenas um resultado
    }

    // Se houver mais de um resultado, exibe a lista para o usuário escolher o correto
    printf(MAGENTA "\n=== PACIENTES ENCONTRADOS (%d) ===\n" RESET, qtdEncontrados);
    for (int i = 0; i < qtdEncontrados; i++) {
        imprimirCardPaciente(i + 1, lista[indicesEncontrados[i]]);
    }
    printf(VERMELHO "[0] Cancelar e voltar ao menu\n\n" RESET);

    char promptEscolha[120];
    snprintf(promptEscolha, sizeof(promptEscolha), "Qual o número (#) do paciente acima deseja %s? ", acaoTexto);

    int escolha = lerInteiroValido(promptEscolha, 0, qtdEncontrados);
    if (escolha == 0) {
        printf(CIANO "\nOperação cancelada.\n" RESET);
        return -1;
    }

    return indicesEncontrados[escolha - 1];
}

// Menu principal interativo
void menu() {
    printf(CIANO "\n\t========================================\n");
    printf("\t            MENU DE OPÇÕES\n");
    printf("\t========================================\n" RESET);

    printf("\n"
    MAGENTA "\t1" RESET " - Cadastrar Paciente\n"
    MAGENTA "\t2" RESET " - Atualizar Cadastro\n"
    MAGENTA "\t3" RESET " - Imprimir Todos os Cadastros\n"
    MAGENTA "\t4" RESET " - Pesquisar Paciente\n"
    MAGENTA "\t5" RESET " - Apagar Cadastro\n"
    VERMELHO "\t0" RESET " - Sair\n"
    "\n");
}

// ========================================================================== */
//                     OPERAÇÕES PRINCIPAIS DO CRUD                           */
// ========================================================================== */

// 1. CREATE (Cadastrar novo paciente)
bool receberDadosPaciente(Paciente *p) {
    gerarProximoCodigo(p->Codigo);
    printf(AZUL "Gerando código de cadastro: %s\n" RESET, p->Codigo);
    printf(AMARELO "(Digite \"cancel\" em qualquer campo para cancelar)\n" RESET);

    if (!lerTextoCancelavel("\nInsira o Nome do Dono: ", p->NomeDono, sizeof(p->NomeDono))) return false;
    formatarSeApenasLetras(p->NomeDono);

    if (!lerTextoCancelavel("Insira o Telefone de Contato: ", p->Telefone, sizeof(p->Telefone))) return false;

    if (!lerTextoCancelavel("Insira a Espécie: ", p->Especie, sizeof(p->Especie))) return false;
    formatarSeApenasLetras(p->Especie);

    if (!lerTextoCancelavel("Insira o Nome do Paciente: ", p->Nome, sizeof(p->Nome))) return false;
    formatarSeApenasLetras(p->Nome);

    if (!lerTextoCancelavel("Insira a Idade e a Medida (ex: 10 meses, 2 anos): ", p->Idade, sizeof(p->Idade))) return false;
    formatarSeApenasLetras(p->Idade);

    char tempSexo = lerCharCancelavel(
        "Insira o sexo ([F] para fêmea ou [M] para macho): ",
        "FfMm",
        "OPÇÃO INVÁLIDA! Digite apenas F ou M (ou \"cancel\" para sair)."
    );
    if (tempSexo == '\0') return false;
    strcpy(p->Sexo, (tempSexo == 'F' || tempSexo == 'f') ? "Femea" : "Macho");

    if (!lerTextoCancelavel("Insira a Raça: ", p->Raca, sizeof(p->Raca))) return false;
    formatarSeApenasLetras(p->Raca);

    if (!lerTextoCancelavel("Insira o Peso e a Medida (ex: 200 gramas, 4 kg): ", p->Peso, sizeof(p->Peso))) return false;
    formatarSeApenasLetras(p->Peso);

    char tempCastrado = lerCharCancelavel(
        "É castrado? ([S] para Sim [N] para Não): ",
        "SsNn",
        "OPÇÃO INVÁLIDA! Digite apenas S ou N (ou \"cancel\" para sair)."
    );
    if (tempCastrado == '\0') return false;
    strcpy(p->Castrado, (tempCastrado == 'S' || tempCastrado == 's') ? "Castrado" : "Nao Castrado");

    return true;
}

void escreverCadastro() {
    arquivo = fopen(nomeArquivo, "r");
    int precisaCabecalho = 0;

    if (arquivo == NULL) {
        precisaCabecalho = 1;
    } else {
        int c = fgetc(arquivo);
        if (c == EOF) {
            precisaCabecalho = 1;
        }
        fclose(arquivo);
    }

    if (precisaCabecalho) {
        arquivo = fopen(nomeArquivo, "w");
        if (arquivo == NULL) {
            printf(VERMELHO "=== Erro ao criar o arquivo! ===\n" RESET);
            return;
        }
        fprintf(arquivo, "Codigo,NomeDono,Telefone,Especie,Nome,Idade,Sexo,Raca,Peso,Castrado\n");
        fclose(arquivo);
    }

    // Grava os dados no final do arquivo (modo append "a")
    arquivo = fopen(nomeArquivo, "a");
    if (arquivo == NULL) {
        printf(VERMELHO "=== Erro ao abrir o arquivo para adicionar dados! ===\n" RESET);
        return;
    }

    fprintf(arquivo, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
            paciente.Codigo,
            paciente.NomeDono,
            paciente.Telefone,
            paciente.Especie,
            paciente.Nome, 
            paciente.Idade, 
            paciente.Sexo, 
            paciente.Raca, 
            paciente.Peso, 
            paciente.Castrado);

    fclose(arquivo);
    printf(VERDE "\n=== Paciente cadastrado com sucesso! Código gerado: %s ===\n" RESET, paciente.Codigo);
    memset(&paciente, 0, sizeof(Paciente));
}

void cadastrarPaciente() {
    printf(MAGENTA "=== CADASTRO DE PACIENTE ===\n" RESET);
    if (receberDadosPaciente(&paciente)) {
        escreverCadastro();
    } else {
        memset(&paciente, 0, sizeof(Paciente));
        printf(VERMELHO "\n=== Cadastro cancelado pelo usuário. ===\n" RESET);
    }
}

// 2. UPDATE (Atualizar cadastro existente)
bool atualizarCadastro() {
    Paciente lista[500];
    int total = carregarPacientes(lista, 500);

    if (total <= 0) {
        exibirAvisoSemCadastro("ATUALIZAR CADASTRO");
        return false;
    }

    printf(MAGENTA "\n=== ATUALIZAR CADASTRO ===\n\n" RESET);
    int idx = buscarIndicePaciente(lista, total, "atualizar");

    if (idx < 0) return true;

    Paciente backupOriginal = lista[idx]; // Mantém backup caso o usuário desfaça as alterações
    bool editando = true;
    bool houveAlteracao = false;
    bool salvarAlteracoes = false;

    while (editando) {
        printf(AMARELO "\n=== EDITANDO PACIENTE: %s (Código: %s) ===\n" RESET, lista[idx].Nome, lista[idx].Codigo);
        imprimirCardPaciente(1, lista[idx]);

        printf(MAGENTA "\t1" RESET " - Nome do Dono\n");
        printf(MAGENTA "\t2" RESET " - Telefone\n");
        printf(MAGENTA "\t3" RESET " - Espécie\n");
        printf(MAGENTA "\t4" RESET " - Nome do Paciente\n");
        printf(MAGENTA "\t5" RESET " - Idade\n");
        printf(MAGENTA "\t6" RESET " - Sexo\n");
        printf(MAGENTA "\t7" RESET " - Raça\n");
        printf(MAGENTA "\t8" RESET " - Peso\n");
        printf(MAGENTA "\t9" RESET " - Castrado?\n");
        printf(MAGENTA "\t10" RESET " - Atualizar TODOS os campos\n");
        printf(VERDE "\t11" RESET " - Concluir e salvar alterações\n");
        printf(VERMELHO "\t0" RESET " - Sair e desfazer todas as alterações\n\n");

        int campo = lerInteiroValido("Qual informação deseja alterar? ", 0, 11);

        switch (campo) {
            case 1:
                lerTextoSeguro("\nNovo Nome do Dono: ", lista[idx].NomeDono, sizeof(lista[idx].NomeDono));
                formatarSeApenasLetras(lista[idx].NomeDono);
                houveAlteracao = true;
                break;
            case 2:
                lerTextoSeguro("\nNovo Telefone: ", lista[idx].Telefone, sizeof(lista[idx].Telefone));
                houveAlteracao = true;
                break;
            case 3:
                lerTextoSeguro("\nNova Espécie: ", lista[idx].Especie, sizeof(lista[idx].Especie));
                formatarSeApenasLetras(lista[idx].Especie);
                houveAlteracao = true;
                break;
            case 4:
                lerTextoSeguro("\nNovo Nome do Paciente: ", lista[idx].Nome, sizeof(lista[idx].Nome));
                formatarSeApenasLetras(lista[idx].Nome);
                houveAlteracao = true;
                break;
            case 5:
                lerTextoSeguro("\nNova Idade (ex: 10 meses, 2 anos): ", lista[idx].Idade, sizeof(lista[idx].Idade));
                formatarSeApenasLetras(lista[idx].Idade);
                houveAlteracao = true;
                break;
            case 6: {
                char s = lerCharValido("\nNovo Sexo ([F] Fêmea / [M] Macho): ", "FfMm", "OPÇÃO INVÁLIDA! Digite apenas F ou M.");
                strcpy(lista[idx].Sexo, (s == 'F' || s == 'f') ? "Femea" : "Macho");
                houveAlteracao = true;
                break;
            }
            case 7:
                lerTextoSeguro("\nNova Raça: ", lista[idx].Raca, sizeof(lista[idx].Raca));
                formatarSeApenasLetras(lista[idx].Raca);
                houveAlteracao = true;
                break;
            case 8:
                lerTextoSeguro("\nNovo Peso (ex: 200 gramas, 4 kg): ", lista[idx].Peso, sizeof(lista[idx].Peso));
                formatarSeApenasLetras(lista[idx].Peso);
                houveAlteracao = true;
                break;
            case 9: {
                char c = lerCharValido("\nÉ castrado? ([S] Sim / [N] Não): ", "SsNn", "OPÇÃO INVÁLIDA! Digite apenas S ou N.");
                strcpy(lista[idx].Castrado, (c == 'S' || c == 's') ? "Castrado" : "Nao Castrado");
                houveAlteracao = true;
                break;
            }
            case 10: {
                char codigoAtual[20];
                strcpy(codigoAtual, lista[idx].Codigo);
                Paciente backupParcial = lista[idx];
                if (receberDadosPaciente(&lista[idx])) {
                    strcpy(lista[idx].Codigo, codigoAtual); // Preserva o código original
                    houveAlteracao = true;
                } else {
                    lista[idx] = backupParcial;
                }
                break;
            }
            case 11: {
                if (!houveAlteracao) {
                    editando = false;
                    salvarAlteracoes = false;
                    break;
                }
                char confirmaSalvar = lerCharValido("\nDeseja SALVAR as alterações? ([S] Sim / [N] Não): ", "SsNn", "Digite S ou N.");
                if (confirmaSalvar == 'S' || confirmaSalvar == 's') {
                    salvarAlteracoes = true;
                    editando = false;
                }
                break;
            }
            case 0: {
                if (!houveAlteracao) {
                    editando = false;
                    salvarAlteracoes = false;
                    break;
                }
                char confirmaDesfazer = lerCharValido("\nDeseja sair e DESFAZER as alterações? ([S] Sim / [N] Não): ", "SsNn", "Digite S ou N.");
                if (confirmaDesfazer == 'S' || confirmaDesfazer == 's') {
                    lista[idx] = backupOriginal;
                    salvarAlteracoes = false;
                    editando = false;
                }
                break;
            }
        }
    }

    if (salvarAlteracoes) {
        salvarTodosPacientes(lista, total, -1);
        printf(VERDE "\n=== Cadastro atualizado com sucesso! ===\n" RESET);
    }

    return true;
}

// 3. READ (Listar todos os cadastros)
void imprimirCadastros() {
    Paciente lista[500];
    int total = carregarPacientes(lista, 500);

    if (total <= 0) {
        exibirAvisoSemCadastro("PACIENTES CADASTRADOS");
        return;
    }

    printf(MAGENTA "\n=== PACIENTES CADASTRADOS ===\n" RESET);
    printf(AZUL "Total de registros: %d\n" RESET, total);

    for (int i = 0; i < total; i++) {
        imprimirCardPaciente(i + 1, lista[i]);
    }

    printf(VERDE "\nLeitura concluída! Pressione ENTER para voltar ao menu..." RESET);
    getchar();
}

// Pesquisa geral exibindo todos os resultados correspondentes
void pesquisarPaciente() {
    Paciente lista[500];
    int total = carregarPacientes(lista, 500);

    if (total <= 0) {
        exibirAvisoSemCadastro("PESQUISAR PACIENTE");
        return;
    }

    char termoBusca[50];
    printf(MAGENTA "\n=== PESQUISAR PACIENTE ===\n" RESET);
    printf("\t" AZUL "1" RESET " - Pesquisar por Código de Cadastro\n");
    printf("\t" AZUL "2" RESET " - Pesquisar por Nome do Paciente\n");
    printf("\t" AZUL "3" RESET " - Pesquisar por Nome do Dono\n");
    printf("\t" AZUL "4" RESET " - Pesquisar por Espécie\n");
    printf("\t" VERMELHO "0" RESET " - Voltar ao menu\n\n");

    int tipoBusca = lerInteiroValido("Escolha o tipo de busca: ", 0, 4);
    if (tipoBusca == 0) return;

    lerTextoSeguro("Digite o termo para pesquisa: ", termoBusca, sizeof(termoBusca));

    char termoLower[50];
    strcpy(termoLower, termoBusca);
    paraMinusculo(termoLower);

    int encontrados = 0;
    printf(MAGENTA "\n=== RESULTADOS DA BUSCA ===\n" RESET);

    for (int i = 0; i < total; i++) {
        char campoComparar[50];
        if (tipoBusca == 1) strcpy(campoComparar, lista[i].Codigo);
        else if (tipoBusca == 2) strcpy(campoComparar, lista[i].Nome);
        else if (tipoBusca == 3) strcpy(campoComparar, lista[i].NomeDono);
        else strcpy(campoComparar, lista[i].Especie);

        paraMinusculo(campoComparar);
        if (strstr(campoComparar, termoLower) != NULL) {
            imprimirCardPaciente(++encontrados, lista[i]);
        }
    }

    if (encontrados == 0) {
        printf(AMARELO "\nNenhum paciente encontrado com \"%s\".\n" RESET, termoBusca);
    } else {
        printf(VERDE "\nTotal de pacientes encontrados: %d\n" RESET, encontrados);
    }

    printf(CIANO "\nPressione ENTER para voltar ao menu..." RESET);
    getchar();
}

// 4. DELETE (Apagar cadastro)
bool apagarCadastro() {
    Paciente lista[500];
    int total = carregarPacientes(lista, 500);

    if (total <= 0) {
        exibirAvisoSemCadastro("APAGAR CADASTRO");
        return false;
    }

    printf(MAGENTA "\n=== APAGAR CADASTRO ===\n" RESET);
    int indiceAlvo = buscarIndicePaciente(lista, total, "apagar");

    if (indiceAlvo < 0) return true;

    printf(MAGENTA "\n=== PACIENTE SELECIONADO ===\n" RESET);
    imprimirCardPaciente(1, lista[indiceAlvo]);

    char promptConfirma[150];
    snprintf(promptConfirma, sizeof(promptConfirma), 
             "\nDeseja apagar o cadastro de \"%s\" (Código: %s)? ([S] Sim / [N] Não): ", 
             lista[indiceAlvo].Nome, lista[indiceAlvo].Codigo);

    char confirma = lerCharValido(promptConfirma, "SsNn", "Digite S ou N.");

    if (confirma != 'S' && confirma != 's') {
        printf(VERMELHO "\nOperação cancelada.\n" RESET);
        return true;
    }

    // Reescreve o arquivo ignorando o índice selecionado
    if (salvarTodosPacientes(lista, total, indiceAlvo)) {
        printf(VERDE "\n=== Cadastro apagado com sucesso! ===\n" RESET);
    } else {
        printf(VERMELHO "\n=== Erro ao atualizar o arquivo! ===\n" RESET);
    }

    return true;
}

// Funções utilitárias de terminal
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    #ifdef _WIN32
        Sleep(1500); 
    #else
        usleep(1500 * 1000); 
    #endif
}

void transicao() {
    pausar();
    limparTela();
}

// ========================================================================== */
//                            FUNÇÃO PRINCIPAL (MAIN)                         */
// ========================================================================== */
int main() {
    setlocale(LC_ALL, "portuguese"); // Garante acentuação correta
    ativarCoresWindows();

    printf(VERDE "\n\t========================================\n");
    printf("\t   BEM-VINDO AO SISTEMA VETERINÁRIO!\n");
    printf("\t========================================\n" RESET);

    // Loop principal do sistema controlado por menu
    while (!parar) {
        menu();
        opcao = lerInteiroValido("Insira a opção desejada: ", 0, 5);

        switch (opcao) {
            case 1:
                limparTela();
                cadastrarPaciente();
                transicao();
                break;
            case 2:
                limparTela();
                if (atualizarCadastro()) transicao();
                else limparTela();
                break;
            case 3:
                limparTela();
                imprimirCadastros();
                limparTela();
                break;
            case 4:
                limparTela();
                pesquisarPaciente();
                limparTela();
                break;
            case 5:
                limparTela();
                if (apagarCadastro()) transicao();
                else limparTela();
                break;
            case 0:
                printf(VERDE "\nSaindo do sistema... Até logo!\n" RESET);
                parar = true;
                break;
        }
    }

    return 0;
}