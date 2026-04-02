#include <stdio.h>
#include <string.h>

#include "../include/app_state.h"
#include "../include/auth.h"
#include "../include/console.h"
#include "../include/storage.h"

int validarEmail(char email[]) {
    return strchr(email, '@') != NULL;
}

void cadastro(void) {
    char confirmarSenha[20];

    console_clear();

    printf("========================================\n");
    printf("        CADASTRO DE USUARIO\n");
    printf("========================================\n");

    printf("\n--- DADOS PESSOAIS ---\n");

    printf("Nome: ");
    scanf(" %[^\n]", user.nome);

    printf("Nascimento: ");
    scanf(" %[^\n]", user.nascimento);

    printf("CPF: ");
    scanf("%s", user.cpf);

    printf("\n--- CONTA ---\n");

    do {
        printf("Email: ");
        scanf("%s", user.email);

        if (!validarEmail(user.email))
            printf("Email invalido!\n");

    } while (!validarEmail(user.email));

    printf("Senha: ");
    scanf("%s", user.senha);

    printf("Confirmar senha: ");
    scanf("%s", confirmarSenha);

    if (strcmp(user.senha, confirmarSenha) != 0) {
        printf("\nErro: senhas nao coincidem!\n\n");
        console_pause();
        return;
    }

    if (storage_email_exists(user.email)) {
        printf("\nErro: ja existe cadastro com este email!\n\n");
        console_pause();
        return;
    }

    printf("Telefone: ");
    scanf("%s", user.telefone);

    printf("\n--- ENDERECO ---\n");

    printf("Endereco: ");
    scanf(" %[^\n]", user.endereco);

    printf("Cidade: ");
    scanf(" %[^\n]", user.cidade);

    printf("\n--- SAUDE ---\n");

    printf("Tipo sanguineo: ");
    scanf("%s", user.sangue);

    printf("Alergias: ");
    scanf(" %[^\n]", user.alergias);

    printf("\n--- CONTATOS DE EMERGENCIA ---\n");

    printf("Nome do Contato 1: ");
    scanf(" %[^\n]", user.contatoNome1);

    printf("Telefone: ");
    scanf("%s", user.contatoTelefone1);

    printf("Nome do Contato 2: ");
    scanf(" %[^\n]", user.contatoNome2);

    printf("Telefone: ");
    scanf("%s", user.contatoTelefone2);

    printf("\n--- CONFIGURACAO DE SEGURANCA ---\n");
    printf("Proteger check-in com senha?\n1 - Sim\n0 - Nao\n");
    scanf("%d", &user.protecaoCheckin);

    if (user.protecaoCheckin == 1) {
        printf("Crie uma senha: ");
        scanf("%s", user.senhaCheckin);
    } else {
        user.senhaCheckin[0] = '\0';
    }

    user.intervaloCheckin = 0;
    user.contatoCheckin = 0;
    user.mensagemCheckin[0] = '\0';

    if (!storage_insert_usuario(&user)) {
        printf("\nErro: nao foi possivel salvar o cadastro (pasta data/). \n\n");
        console_pause();
        return;
    }

    printf("\nCadastro realizado com sucesso!\n\n");
    cadastrado = 1;

    console_pause();
}

void login(void) {
    if (!cadastrado) {
        printf("\nNenhum usuario cadastrado!\n\n");
        console_pause();
        return;
    }

    char email[50], senha[20];

    printf("\n===== LOGIN =====\n");

    printf("Email: ");
    scanf("%s", email);

    printf("Senha: ");
    scanf("%s", senha);

    Usuario tmp;
    if (storage_find_usuario_by_email(email, &tmp) &&
        strcmp(senha, tmp.senha) == 0) {
        user = tmp;
        logado = 1;
        printf("\nLogin realizado com sucesso!\n\n");
    } else {
        printf("\nErro: email ou senha incorretos!\n\n");
    }

    console_pause();
}