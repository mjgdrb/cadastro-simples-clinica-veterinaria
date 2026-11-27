/* ========================================================================== */
/*           SISTEMA DE CADASTRO DE PACIENTES PARA CLÍNICA VETERINÁRIA         */
/* ========================================================================== */

// BIBLIOTECAS UTILIZADAS
#include <stdio.h>     
#include <stdbool.h>   
#include <locale.h>    
#include <string.h>    
#include <stdlib.h>    
#include <ctype.h>     

#ifdef _WIN32
    #include <windows.h> 
#else
    #include <unistd.h>  
#endif

// DEFINIÇÃO DE CORES (ANSI Escape Sequences)
#define RESET    "\033[0m"
#define NEGRITO  "\033[1m"
#define VERMELHO "\033[1;31m"
#define VERDE    "\033[1;32m"
#define AMARELO  "\033[1;33m"
#define AZUL     "\033[1;34m"
#define MAGENTA  "\033[1;35m"
#define CIANO    "\033[1;36m"

// ESTRUTURA DE DADOS (STRUCT) DO PACIENTE
typedef struct {
    char Codigo[20];       
    char NomeDono[50];     
    char Telefone[30];     
    char Especie[50];      
    char Nome[50];         
    char Idade[30];            
    char Sexo[10];         
    char Raca[50];         
    char Peso[30];                  
    char Castrado[30];     
} Paciente;

// CONSTANTES GLOBAIS DE CONFIGURAÇÃO DE ARQUIVOS
const char *nomeArquivo = "pacientes.csv";          
const char *nomeArquivoContador = "contador.txt";   

// ========================================================================== */
//                     FUNÇÕES AUXILIARES DE VALIDAÇÃO E FORMATAÇÃO           */
// ========================================================================== */

void paraMinusculo(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void formatarSeApenasLetras(char *str) {
    bool temLetra = false;

    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (isalpha(c)) {
            temLetra = true;
        } else if (!isspace(c)) {
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

void ativarCoresWindows() {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004; 
        SetConsoleMode(hOut, dwMode);
    #endif
}

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
            return '\0';
        }

        if (sscanf(buffer, " %c %c", &c, &extra) == 1) {
            if (strchr(opcoesValidas, c) != NULL) {
                return c;
            }
        }
        printf(VERMELHO "=== %s ===\n\n" RESET, mensagemErro);
    }
}

void lerTextoSeguro(const char *prompt, char *destino, int tamanho) {
    while (true) {
        printf(CIANO "%s" RESET, prompt);
        if (fgets(destino, tamanho, stdin) == NULL) {
            continue;
        }
        destino[strcspn(destino, "\n")] = '\0';

        for (int i = 0; destino[i]; i++) {
            if (destino[i] == ',') {
                destino[i] = '.';
            }
        }

        if (strlen(destino) > 0) {
            return;
        }
        printf(VERMELHO "=== ESTE CAMPO NÃO PODE FICAR VAZIO! ===\n\n" RESET);
    }
}

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

void gerarProximoCodigo(char *codigoDestino) {
    int proximoId = 1;

    FILE *fCounter = fopen(nomeArquivoContador, "r");
    if (fCounter != NULL) {
        fscanf(fCounter, "%d", &proximoId);
        fclose(fCounter);
    }

    fCounter = fopen(nomeArquivoContador, "w");
    if (fCounter != NULL) {
        fprintf(fCounter, "%d", proximoId + 1);
        fclose(fCounter);
    }

    sprintf(codigoDestino, "%d", proximoId);
}

int carregarPacientes(Paciente lista[], int maxItens) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return -1;
    }

    int total = 0;
    char linha[800];
    fgets(linha, sizeof(linha), arquivo); // Pula o cabeçalho

    while (fgets(linha, sizeof(linha), arquivo) && total < maxItens) {
        linha[strcspn(linha, "\n")] = '\0';

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
    return total;
}

bool salvarTodosPacientes(Paciente lista[], int total, int indiceIgnorar) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        return false;
    }

    fprintf(arquivo, "Codigo,NomeDono,Telefone,Especie,Nome,Idade,Sexo,Raca,Peso,Castrado\n");
    for (int i = 0; i < total; i++) {
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
//                     FUNÇÕES DE INTERFACE E EXIBIÇÃO                        */
// ========================================================================== */

void exibirAvisoSemCadastro(const char *tituloSecao) {
    printf(MAGENTA "\n=== %s ===\n" RESET, tituloSecao);
    printf(AMARELO "\nAviso: Não há nenhum paciente cadastrado no sistema.\n" RESET);
    printf(CIANO "\nPressione ENTER para voltar ao menu..." RESET);
    getchar();
}

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
        lerTextoSeguro("\nDigite o Código de Cadastro (ex: 1, 2...): ", termoBusca, sizeof(termoBusca));
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
        return indicesEncontrados[0];
    }

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

void escreverCadastro(Paciente *p) {
    FILE *arquivo = fopen(nomeArquivo, "r");
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

    arquivo = fopen(nomeArquivo, "a");
    if (arquivo == NULL) {
        printf(VERMELHO "=== Erro ao abrir o arquivo para adicionar dados! ===\n" RESET);
        return;
    }

    fprintf(arquivo, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
            p->Codigo,
            p->NomeDono,
            p->Telefone,
            p->Especie,
            p->Nome, 
            p->Idade, 
            p->Sexo, 
            p->Raca, 
            p->Peso, 
            p->Castrado);

    fclose(arquivo);
    printf(VERDE "\n=== Paciente cadastrado com sucesso! Código gerado: %s ===\n" RESET, p->Codigo);
}

void cadastrarPaciente() {
    Paciente p;
    memset(&p, 0, sizeof(Paciente));
    
    printf(MAGENTA "=== CADASTRO DE PACIENTE ===\n" RESET);
    if (receberDadosPaciente(&p)) {
        escreverCadastro(&p);
    } else {
        printf(VERMELHO "\n=== Cadastro cancelado pelo usuário. ===\n" RESET);
    }
}

bool atualizarCadastro() {
    Paciente lista[500];
    int total = carregarPacientes(lista, 500);

    if (total <= 0) {
        exibirAvisoSemCadastro("ATUALIZAR CADASTRO");
        return false;
    }

    printf(MAGENTA "\n=== ATUALIZAR CADASTRO ===\n\n" RESET);
    int idx = buscarIndicePaciente(lista, total, "atualizar");

    if (idx < 0) {
        return true;
    }

    Paciente backupOriginal = lista[idx];
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
                
                // Cria paciente temporário para receber dados atualizados
                Paciente tempP;
                memset(&tempP, 0, sizeof(Paciente));
                
                if (receberDadosPaciente(&tempP)) {
                    strcpy(tempP.Codigo, codigoAtual); // Restaura o código original
                    lista[idx] = tempP;
                    houveAlteracao = true;
                } else {
                    lista[idx] = backupParcial;
                    printf(AMARELO "\nEdição completa cancelada. Os dados anteriores desta etapa foram mantidos.\n" RESET);
                }
                break;
            }
            case 11: {
                if (!houveAlteracao) {
                    printf(CIANO "\nNenhuma alteração foi feita até o momento.\n" RESET);
                    editando = false;
                    salvarAlteracoes = false;
                    break;
                }

                char confirmaSalvar = lerCharValido(
                    "\nTem certeza que deseja SALVAR as alterações realizadas? ([S] Sim / [N] Não): ",
                    "SsNn",
                    "OPÇÃO INVÁLIDA! Digite apenas S para Sim ou N para Não."
                );

                if (confirmaSalvar == 'S' || confirmaSalvar == 's') {
                    salvarAlteracoes = true;
                    editando = false;
                } else {
                    printf(AMARELO "\nSalvamento cancelado. Você voltou para a edição do paciente.\n" RESET);
                }
                break;
            }
            case 0: {
                if (!houveAlteracao) {
                    printf(CIANO "\nSaindo da edição sem alterações.\n" RESET);
                    editando = false;
                    salvarAlteracoes = false;
                    break;
                }

                char confirmaDesfazer = lerCharValido(
                    "\nATENÇÃO: Todas as alterações feitas serão perdidas! Deseja realmente sair e desfazer? ([S] Sim / [N] Não): ",
                    "SsNn",
                    "OPÇÃO INVÁLIDA! Digite apenas S para Sim ou N para Não."
                );

                if (confirmaDesfazer == 'S' || confirmaDesfazer == 's') {
                    lista[idx] = backupOriginal;
                    salvarAlteracoes = false;
                    editando = false;
                    printf(VERMELHO "\n=== Todas as alterações foram desfeitas. ===\n" RESET);
                } else {
                    printf(AMARELO "\nSaída cancelada. Suas alterações continuam na tela de edição.\n" RESET);
                }
                break;
            }
        }

        if (editando && houveAlteracao && campo >= 1 && campo <= 9) {
            printf(VERDE "\nCampo atualizado! Edite outro campo, digite 11 para salvar ou 0 para desfazer.\n" RESET);
        }
    }

    if (salvarAlteracoes) {
        if (salvarTodosPacientes(lista, total, -1)) {
            printf(VERDE "\n=== Cadastro de \"%s\" atualizado e salvo com sucesso! ===\n" RESET, lista[idx].Nome);
        } else {
            printf(VERMELHO "\n=== Erro ao salvar alterações no arquivo! ===\n" RESET);
        }
    }

    return true;
}

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

    if (tipoBusca == 0) {
        printf(VERMELHO "\nBusca cancelada.\n" RESET);
        return;
    }

    lerTextoSeguro("Digite o termo para pesquisa: ", termoBusca, sizeof(termoBusca));

    char termoLower[50];
    strcpy(termoLower, termoBusca);
    paraMinusculo(termoLower);

    int encontrados = 0;

    printf(MAGENTA "\n=== RESULTADOS DA BUSCA ===\n" RESET);

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

bool apagarCadastro() {
    Paciente lista[500];
    int total = carregarPacientes(lista, 500);

    if (total <= 0) {
        exibirAvisoSemCadastro("APAGAR CADASTRO");
        return false;
    }

    printf(MAGENTA "\n=== APAGAR CADASTRO ===\n" RESET);
    int indiceAlvo = buscarIndicePaciente(lista, total, "apagar");

    if (indiceAlvo < 0) {
        return true;
    }

    printf(MAGENTA "\n=== PACIENTE SELECIONADO ===\n" RESET);
    imprimirCardPaciente(1, lista[indiceAlvo]);

    char promptConfirma[150];
    snprintf(promptConfirma, sizeof(promptConfirma), 
             "\nTem certeza que deseja apagar o cadastro de \"%s\" (Código: %s)? ([S] Sim / [N] Não): ", 
             lista[indiceAlvo].Nome, lista[indiceAlvo].Codigo);

    char confirma = lerCharValido(
        promptConfirma,
        "SsNn",
        "OPÇÃO INVÁLIDA! Digite apenas S para Sim ou N para Não."
    );

    if (confirma != 'S' && confirma != 's') {
        printf(VERMELHO "\nOperação cancelada. Nenhum dado foi apagado.\n" RESET);
        return true;
    }

    if (salvarTodosPacientes(lista, total, indiceAlvo)) {
        printf(VERDE "\n=== Cadastro de \"%s\" apagado com sucesso! ===\n" RESET, lista[indiceAlvo].Nome);
    } else {
        printf(VERMELHO "\n=== Erro ao atualizar o arquivo! ===\n" RESET);
    }

    return true;
}

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

// FUNÇÃO PRINCIPAL
int main() {
    setlocale(LC_ALL, "portuguese");
    ativarCoresWindows();

    int opcao; // Variável local controlando o menu
    bool parar = false;

    printf(VERDE "\n\t========================================\n");
    printf("\t   BEM-VINDO AO SISTEMA VETERINÁRIO!\n");
    printf("\t========================================\n" RESET);

    while (!parar) {
        menu();
        opcao = lerInteiroValido("Insira a opção desejada: ", 0, 5);

        switch (opcao) {
            case 1: // Cadastrar Paciente
                limparTela();
                cadastrarPaciente();
                transicao();
                break;
            case 2: // Atualizar Cadastro
                limparTela();
                if (atualizarCadastro()) {
                    transicao();
                } else {
                    limparTela();
                }
                break;
            case 3: // Imprimir Cadastros
                limparTela();
                imprimirCadastros();
                limparTela();
                break;
            case 4: // Pesquisar Paciente
                limparTela();
                pesquisarPaciente();
                limparTela();
                break;
            case 5: // Apagar Cadastro
                limparTela();
                if (apagarCadastro()) {
                    transicao();
                } else {
                    limparTela();
                }
                break;
            case 0: // Sair
                printf(VERDE "\nSaindo do sistema... Até logo!\n" RESET);
                parar = true;
                break;
        }
    }

    return 0;
}