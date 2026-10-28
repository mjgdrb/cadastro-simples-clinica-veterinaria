#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

int opcao;
bool parar;

typedef struct{
    
    //Animal
    char Especie[50];
    char Nome[50];
    int Idade;
    char Sexo;                  // [F] fêmea [M] macho
    char Raça[50];
    float Peso;
    float Altura;
    bool castrado;              // [true] castrado [false] não castrado


    //Dono
    char NomeDono[50];
    int IdadeDono;
    int TelefoneDono;

    //Opcionais
    char Porte[50];
    char Cor[50];
    char Pelagem[50];

}Paciente;

int criarArquivo(){

    FILE *arquivo = fopen("dados.csv", "w");
    if(arquivo == NULL){
        printf("--- Erro ao criar arquivo. ---");
        return 1;
    }

    fclose(arquivo);
    printf("Arquivo CSV criado com sucesso!\n");

}

int fazerCadastro(){
    


}

int atualizarCadastro(){



}


int imprimirCadastros(){

    FILE *arquivo = fopen("dados.csv", "r");
    if(arquivo == NULL){
        printf("--- Erro ao ler arquivo. ---");
        return 1;
    }

    char linha[700];
    char *token;

    fgets(linha, sizeof(linha), arquivo);

    printf("=== Dados lidos ===\n");

    while (fgets(linha, sizeof(linha), arquivo)) {

        token = strtok(linha, ","); // Espécie

        printf("Espécie: %30s | ", token);

        token = strtok(NULL, ","); // Nome

        printf("Nome: %50s | ", token);

        token = strtok(NULL, ","); // Idade

        printf("Idade: %3d | ", token);

        token = strtok(NULL, ","); // Sexo

        printf("Sexo: %3s | ", token);

        token = strtok(NULL, ","); // Raça

        printf("Raça: %30s | ", token);

        token = strtok(NULL, ","); // Peso

        printf("Peso: %.2f | ", token);

        token = strtok(NULL, ","); // Altura

        printf("Altura: %.2f | ", token);

        token = strtok(NULL, "\n"); 

        if(token == 0){
            printf("Castrado: SIM\n");  // Castrado
        }
        else{
            printf("Castrado: NÃO\n");  // Não Castrado
        }
        
    }

    fclose(arquivo);
    printf("Arquivo CSV lido com sucesso!\n");

}


int apagarCadastro(){



}

void menu(){

    printf("BEM VINDO AO SISTEMA DE CADASTRO DE PACIENTES!\n");

    printf("MENU DE OPÇÕES\n");

    printf("1 - Criar Arquivo\n");
    printf("2 - Cadastrar Paciente\n");
    printf("3 - Atualizar Cadastro\n");
    printf("4 - Imprimir Cadastros\n");
    printf("5 - Apagar Cadastro\n");
    printf("6 - Sair\n");

}

bool pegarOpcao(){

    printf("Insira a opção desejada: ");

    if (scanf("%d", &opcao) != 1) { // Verifica se a entrada não é um número
        printf("\n=== ENTRADA INVALIDA! DIGITE APENAS NUMEROS. ===\n");
        while (getchar() != '\n'); // Limpa o buffer
        return false;
    }

    if (opcao != 1 || opcao != 2 || opcao != 3 || opcao != 4 || opcao != 5 ||opcao != 6){
        printf("=== ENTRADA INVALIDA! ===\n");
        return false;
    }

    return true;
}


int main(){
    SetConsoleOutputCP(CP_UTF8);

    while(!parar){
        menu();

        if(!pegarOpcao()){
            continue;
        }
    }

    switch (opcao)
    {
    case 1:             // Criar Arquivo
        criarArquivo();
        break;
    case 2:             // Cadastrar Paciente
        fazerCadastro();
        break;
    case 3:             // Atualizar Cadastro
        atualizarCadastro();
        break;
    case 4:             // Imprimir Cadastros
        imprimirCadastros();
        break;
    case 5:             // Apagar Cadastro
        apagarCadastro();
        break;
    case 6:             // Sair
        parar = true;
        break;
    default:            // Erro
        printf("=== OPÇÃO INVÁLIDA ===");
        break;
    }


    return 0;
}