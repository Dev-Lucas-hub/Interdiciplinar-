#ifndef VIVA_SEGURA_H
#define VIVA_SEGURA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <sqlite3.h>
#include <conio.h>

struct Usuario {
    char nome[50]; char nascimento[20]; char cpf[20];
    char email[50]; char senha[20]; char telefone[20];
    char endereco[100]; char cidade[50]; char estado[10]; char cep[20];
    char sangue[5]; char alergias[100];
    char contatoNome1[50]; char contatoTelefone1[20];
    char contatoNome2[50]; char contatoTelefone2[20];
    int contatoCheckin; int intervaloCheckin;
    char mensagemCheckin[100]; int protecaoCheckin;
    char senhaCheckin[20]; int intervaloMinutos;
};

extern struct Usuario user;
extern int logado;
extern int cadastrado;

void limparNumero(char *entrada, char *saida);
int validarCPF(const char *cpf);

int cpfExiste(sqlite3 *db, const char *cpf);

int validarEmail(char email[]);
void cadastro();
void login();
void verDados();
void emergencia();
void monitoramento();
void checkin();
void denuncia();
void telefones();
void menu();

#endif
