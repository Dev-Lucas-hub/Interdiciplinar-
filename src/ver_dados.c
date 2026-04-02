#include <stdio.h>

#include "../include/app_state.h"
#include "../include/console.h"
#include "../include/ver_dados.h"

void verDados(void) {
    if (!logado) {
        printf("\nFaca login primeiro!\n\n");
        console_pause();
        return;
    }

    printf("\n========== DADOS DO USUARIO ==========\n");

    printf("Nome: %s\n", user.nome);
    printf("Nascimento: %s\n", user.nascimento);
    printf("CPF: %s\n", user.cpf);
    printf("Email: %s\n", user.email);
    printf("Telefone: %s\n", user.telefone);
    printf("Endereco: %s\n", user.endereco);
    printf("Cidade: %s\n", user.cidade);
    printf("Tipo sanguineo: %s\n", user.sangue);
    printf("Alergias: %s\n", user.alergias);

    printf("\nContatos:\n");
    printf("- %s (%s)\n", user.contatoNome1, user.contatoTelefone1);
    printf("- %s (%s)\n", user.contatoNome2, user.contatoTelefone2);

    console_pause();
}

