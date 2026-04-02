#include <stdio.h>
#include <string.h>

#include "../include/app_state.h"
#include "../include/console.h"
#include "../include/denuncia.h"
#include "../include/storage.h"

void denuncia(void) {
    int anonimo;
    char texto[200];

    console_clear();

    printf("========================================\n");
    printf("            DENUNCIA\n");
    printf("========================================\n");

    printf("\nDescreva a situacao:\n");
    scanf(" %[^\n]", texto);

    printf("\nDeseja enviar anonimamente?\n");
    printf("1 - Sim\n");
    printf("0 - Nao\n");
    printf("Escolha: ");
    scanf("%d", &anonimo);

    printf("\nEnviando denuncia...\n\n");
    console_pause();

    console_clear();

    printf("\n========================================\n");
    printf("        CONFIRMACAO DA DENUNCIA\n");
    printf("========================================\n");

    if (anonimo == 1) {
        printf("Tipo: Anonima\n");
        printf("Sua identidade foi protegida.\n");
    } else {
        printf("Tipo: Identificada\n");
        printf("Responsavel: %s\n", user.nome);
    }

    printf("\n--- DESCRICAO DA DENUNCIA ---\n");
    printf("%s\n", texto);

    storage_append_denuncia(anonimo ? 0 : user.id_usuario, texto);

    printf("\nSua denuncia foi enviada com sucesso!\n\n");
    printf("As autoridades competentes foram notificadas.\n\n");

    strcpy(texto, "");
    console_pause();
}