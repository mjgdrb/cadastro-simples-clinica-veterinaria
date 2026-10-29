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
    char Raca[50];
    float Peso;
    float Altura;
    bool Castrado;              // [true] castrado [false] não castrado

/*

 -> vou adicionar essa parte depois

 //Dono
    char NomeDono[50];
    int IdadeDono;
    int TelefoneDono;

    //Opcionais
    char Porte[50];
    char Cor[50];
    char Pelagem[50];

*/
   

}Paciente;

void criarArquivo(){

    FILE *arquivo = fopen("dados.csv", "w");
    if(arquivo == NULL){
        printf("--- Erro ao criar arquivo. ---");
        parar = true;
        return;
    }

    fprintf(arquivo, "Espécie,Nome,Idade,Sexo,Raça,Peso,Altura,Castrado\n");

    fclose(arquivo);
    printf("Arquivo CSV criado com sucesso!\n");
}

Paciente paciente[MAX_PACIENTES];
int MAX_PACIENTES = 100;


void fazerCadastro(){
    
// ainda irei fazer



printf("\nDigite a espécie do paciente: ");
fgets(paciente[i].Especie, sizeof(paciente[i].Especie), stdin);
paciente[i].Especie[strcspn(paciente[i].Especie, "\n")] = '\0';

printf("\nDigite o nome do paciente: ");
fgets(paciente[i].Nome, sizeof(paciente[i].Nome), stdin);
paciente[i].Nome[strcspn(paciente[i].Nome, "\n")] = '\0';

printf("Digite a idade de %s: ", paciente[i].Nome);
scanf("%d", &paciente[i].Idade);
getchar(); // limpa o buffer

printf("Digite o sexo de %s: ", paciente[i].Nome);
scanf("%c", &paciente[i].Sexo);
getchar(); // limpa o buffer

printf("\nDigite a raça do paciente %d: ", i + 1);
fgets(paciente[i].Raca, sizeof(paciente[i].Raca), stdin);
paciente[i].Raca[strcspn(paciente[i].Raca, "\n")] = '\0';

printf("Digite o peso de %s(em Kg)): ", paciente[i].Nome);
scanf("%f", &paciente[i].Peso);
getchar(); // limpa o buffer

printf("Digite a altura de %s(em m)): ", paciente[i].Nome);
scanf("%f", &paciente[i].Altura);
getchar(); // limpa o buffer

printf("%s é castrado(a)? [1] para SIM [0] para NÃO): ", paciente[i].Nome);
scanf("%d", &paciente[i].Castrado);
getchar(); // limpa o buffer

(*MAX_PACIENTES)++;
}

void atualizarCadastro(){

// ainda irei fazer


}


void imprimirCadastros(){

    FILE *arquivo = fopen("dados.csv", "r");

    if(arquivo == NULL){
        printf("--- Erro ao ler arquivo. ---\n");
        parar = true;
        return;
    }

    char linha[700];
    char *token;

    fgets(linha, sizeof(linha), arquivo);

    printf("=== Dados lidos ===\n");

    while (fgets(linha, sizeof(linha), arquivo)) {

        token = strtok(linha, ","); // Espécie

        printf("Espécie: %s | ", token);

        token = strtok(NULL, ","); // Nome

        printf("Nome: %s | ", token);

        token = strtok(NULL, ","); // Idade

        printf("Idade: %s | ", token);

        token = strtok(NULL, ","); // Sexo

        printf("Sexo: %s | ", token);

        token = strtok(NULL, ","); // Raça

        printf("Raça: %s | ", token);

        token = strtok(NULL, ","); // Peso

        printf("Peso: %s | ", token);

        token = strtok(NULL, ","); // Altura

        printf("Altura: %s | ", token);

        token = strtok(NULL, "\n"); // Castrado

        printf("Castrado: %s\n", token);
        
    }

    fclose(arquivo);
    printf("Arquivo CSV lido com sucesso!\n");

}


void apagarCadastro(){

// ainda irei fazer


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

    if ((opcao < 1) || (opcao > 6)){
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

        switch (opcao){
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
    }


    return 0;
}
