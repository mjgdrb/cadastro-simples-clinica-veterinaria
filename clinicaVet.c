/* SISTEMA DE CADASTRO DE PACIENTES PARA CLINICA VETERINÁRIA */

// BIBLIOTECAS
#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h> 
#else
    #include <unistd.h>  
#endif



//STRUCTS
typedef struct{
    
    //Animal
    char Especie[50];
    char Nome[50];
    char Idade[30];            
    char Sexo[10];                  // Femea / Macho
    char Raca[50];
    char Peso[30];                  
    char Altura[30];
    char Castrado[30];              // Castrado / Nao castrado

}Paciente;

// VARIÁVEIS
char tempCastrado;
char tempSexo;
int opcao;
bool parar = false;
const char *nomeArquivo = "pacientes.csv";
FILE *arquivo;
Paciente paciente;

// FUNÇÕES

void menu(){

    printf("\n\tMENU DE OPÇÕES\n");


    printf("\n"
    "\t1 - Cadastrar Paciente\n"
    "\t2 - Atualizar Cadastro\n"
    "\t3 - Imprimir Cadastros\n"
    "\t4 - Apagar Cadastro\n"
    "\t0 - Sair\n"
    "\n");

}

bool pegarOpcao(){

    printf("\nInsira a opção desejada: ");

    if (scanf("%d", &opcao) != 1) { 
        printf("\n=== ENTRADA INVALIDA! DIGITE APENAS NUMEROS. ===\n");
        while (getchar() != '\n'); 
        return false;
    }

    while (getchar() != '\n'); 

    return true;
}

void receberCadastro(){

    printf("\nInsira a Espécie: ");
    if(fgets(paciente.Especie, sizeof(paciente.Especie), stdin)){
        paciente.Especie[strcspn(paciente.Especie, "\n")] = '\0';
    }

    printf("Insira o Nome: ");
    if(fgets(paciente.Nome, sizeof(paciente.Nome), stdin)){
        paciente.Nome[strcspn(paciente.Nome, "\n")] = '\0';
    }

    printf("Insira a Idade e a Medida (ex: 10 meses, 2 anos, 15 dias): ");
    if(fgets(paciente.Idade, sizeof(paciente.Idade), stdin)){
        paciente.Idade[strcspn(paciente.Idade, "\n")] = '\0';
    }


    do{
        printf("Insira o sexo ([F] para fêmea ou [M] para macho): ");
        scanf(" %c", &tempSexo);
        while(getchar() != '\n');
    }while(tempSexo != 'F' && tempSexo != 'f' && tempSexo != 'M' && tempSexo != 'm');


    strcpy(paciente.Sexo, (tempSexo == 'F' || tempSexo == 'f') ? "Femea" : "Macho");

    tempSexo = '\0';

    printf("Insira a Raça: ");
    if(fgets(paciente.Raca, sizeof(paciente.Raca), stdin)){
        paciente.Raca[strcspn(paciente.Raca, "\n")] = '\0';
    }

    printf("Insira o Peso e a Medida (ex: 200 gramas, 4 kg): ");
    if(fgets(paciente.Peso, sizeof(paciente.Peso), stdin)){
        paciente.Peso[strcspn(paciente.Peso, "\n")] = '\0';
    }

    printf("Insira a Altura e Medida (ex: 50 cm, 1,05 m): ");
    if(fgets(paciente.Altura, sizeof(paciente.Altura), stdin)){
        paciente.Altura[strcspn(paciente.Altura, "\n")] = '\0';
    }

    do{
        printf("É castrado? ([S] para Sim [N] para Não): ");
        scanf(" %c", &tempCastrado);
        while(getchar() != '\n');
    }while(tempCastrado != 'S' && tempCastrado != 's' && tempCastrado != 'N' && tempCastrado != 'n');


    strcpy(paciente.Castrado, (tempCastrado == 'S' || tempCastrado == 's') ? "Castrado" : "Nao Castrado");

    tempCastrado = '\0';

}

void escreverCadastro(){

    arquivo = fopen("pacientes.csv", "r");

    int precisaCabecalho = 0;

    if (arquivo == NULL){
        precisaCabecalho = 1;
        
    }
    else{
        int c = fgetc(arquivo);
        if (c == EOF){
            precisaCabecalho = 1;
        }
        fclose(arquivo);
    }

    if (precisaCabecalho) {
        arquivo = fopen(nomeArquivo, "w");
        if (arquivo == NULL){
            printf("=== Erro ao criar o arquivo! ===\n");
            return;
        }

        fprintf(arquivo, "Especie,Nome,Idade,Sexo,Raca,Peso,Altura,Castrado\n");
        fclose(arquivo);
    }

    arquivo = fopen(nomeArquivo, "a");
        if (arquivo == NULL){
            printf("=== Erro ao abrir o arquivo para adicionar dados! ===\n");
            return;
        }


    fprintf(arquivo,"%s,%s,%s,%s,%s,%s,%s,%s\n",
         paciente.Especie,
         paciente.Nome, 
         paciente.Idade, 
         paciente.Sexo, 
         paciente.Raca, 
         paciente.Peso, 
         paciente.Altura, 
         paciente.Castrado);

    fclose(arquivo);

    printf("\n=== Cliente cadastrado com sucesso! ===\n");

    memset(&paciente, 0, sizeof(Paciente));

}


void cadastrarPaciente(){

    printf("=== CADASTRO DE PACIENTE ===\n");

    receberCadastro();
    escreverCadastro();

}

void atualizarCadastro(){ 
    
    }

void imprimirCadastros(){

    arquivo = fopen(nomeArquivo, "r");

    if(arquivo == NULL){
        printf("\n--- Erro ao ler arquivo. ---\n");
        parar = true;
        return;
    }

    char linha[700];
    int contador = 1;

    fgets(linha, sizeof(linha), arquivo);

    printf("\n=== IMPRIMIR CADASTROS ===\n");

    printf("\n=== Pacientes Cadastrados ===\n");

    while (fgets(linha, sizeof(linha), arquivo)) {

        linha[strcspn(linha, "\n")] = '\0';

        char *especie  = strtok(linha, ",");
        char *nome     = strtok(NULL, ",");
        char *idade    = strtok(NULL, ",");
        char *sexo     = strtok(NULL, ",");
        char *raca     = strtok(NULL, ",");
        char *peso     = strtok(NULL, ",");
        char *altura   = strtok(NULL, ",");
        char *castrado = strtok(NULL, ",");


        if (especie && nome && idade && sexo && raca && peso && altura && castrado) {
            printf("\nPaciente #%d\n\n", contador++);
            printf("  Espécie:  %s\n", especie);
            printf("  Nome:     %s\n", nome);
            printf("  Idade:    %s\n", idade);
            printf("  Sexo:     %s\n", sexo);
            printf("  Raça:     %s\n", raca);
            printf("  Peso:     %s\n", peso);
            printf("  Altura:   %s\n", altura);
            printf("  Castrado?: %s\n", castrado);
            printf("-------------------------------------------------------------\n");
        }
        
    }

    fclose(arquivo);

    printf("\nArquivo CSV lido com sucesso!\n");

}

void apagarCadastro(){
    printf("\n[FAZER: Apagar Cadastro]\n"); 
    }

void limparTela(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    #ifdef _WIN32
        Sleep(2000); 
    #else
        usleep(2000 * 1000); 
    #endif
}

void transicao(){
    pausar();
    limparTela();
}


// FUNÇÃO PRINCIPAL
int main(){
    setlocale(LC_ALL, "Portuguese_Brazil.UTF-8");

    printf("\n\tBEM VINDO AO \nSISTEMA DE CADASTRO DE PACIENTES!\n");

    while(!parar){
        menu();

        if(!pegarOpcao()){
            continue;
        }

        switch (opcao){
            case 1:             // Cadastrar Paciente
                transicao();
                cadastrarPaciente();
                transicao();
                break;
            case 2:             // Atualizar Cadastro
                transicao();
                atualizarCadastro();
                transicao();
                break;
            case 3:             // Imprimir Cadastros
                transicao();
                imprimirCadastros();
                break;
            case 4:             // Apagar Cadastro
                transicao();
                apagarCadastro();
                transicao();
                break;
            case 0:             // Sair
                printf("\nSaindo ...");
                transicao();
                parar = true;
                break;
            default:            // Erro
                printf("=== OPÇÃO INVÁLIDA ===");
                transicao();
                break;
                }
    }

    return 0;
}