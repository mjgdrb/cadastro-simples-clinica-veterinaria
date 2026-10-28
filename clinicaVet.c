#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

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

void abrirArquivo(){



}

void fecharArquivo(){




}

void fazerCadastro(){



}

void imprimirCadastros(){



}

void atualizarCadastro(){



}

void apagarCadastro(){



}

int main(){
    SetConsoleOutputCP(CP_UTF8);




    return 0;
}