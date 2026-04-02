#include <stdio.h>

#include "../include/app_state.h"
#include "../include/console.h"
#include "../include/emergencia.h"

void emergencia(void) {
    if (!logado) {
        printf("\nFaca login primeiro!\n\n");
        console_pause();
        return;
    }

    console_clear();

    printf("========================================\n");
    printf("        EMERGENCIA ACIONADA\n");
    printf("========================================\n");

    printf("\n--- POLICIA [190] ---\n");
    printf("URGENTE: A mulher %s corre risco de vida!\n", user.nome);
    printf("Favor se dirigir imediatamente para a localizacao enviada.\n");

    printf("\n--- CONTATOS DE EMERGENCIA ---\n");
    printf("%s pode estar em perigo de vida!\n", user.nome);
    printf("Acompanhe a localizacao e tente contato imediato.\n");

    printf("\nContatos notificados:\n");
    printf("- %s (%s)\n", user.contatoNome1, user.contatoTelefone1);
    printf("- %s (%s)\n", user.contatoNome2, user.contatoTelefone2);

    printf("\nLocalizacao enviada para a policia [190] e contatos de emergencia.\n");
    printf("Ajuda a caminho!\n\n");

    console_pause();
}

