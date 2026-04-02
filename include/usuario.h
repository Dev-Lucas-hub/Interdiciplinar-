#pragma once

typedef struct Usuario {
    int id_usuario;
    char nome[50];
    char nascimento[20];
    char cpf[20];

    char email[50];
    char senha[20];
    char telefone[20];

    char endereco[100];
    char cidade[50];

    char sangue[5];
    char alergias[100];

    char contatoNome1[50];
    char contatoTelefone1[20];

    char contatoNome2[50];
    char contatoTelefone2[20];

    int contatoCheckin;
    int intervaloCheckin;
    char mensagemCheckin[100];

    int protecaoCheckin;
    char senhaCheckin[20];
} Usuario;