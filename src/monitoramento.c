#include <stdio.h>

#include "../include/app_state.h"
#include "../include/console.h"
#include "../include/monitoramento.h"

void monitoramento(void) {
    if (!logado) {
        printf("\nFaca login primeiro!\n\n");
        console_pause();
        return;
    }

    printf("========================================\n");
    printf("        MONITORAMENTO ATIVO\n");
    printf("========================================\n");

    printf("\nSistema vibrando...\n");
    printf("Enviando localizacao em tempo real...\n");

    printf("\n--- POLICIA [190] ---\n");
    printf("Atencao: A mulher %s se sente insegura neste local.\n", user.nome);
    printf("Favor realizar patrulhamento e verificar a situacao.\n");

    printf("\n--- CONTATOS DE EMERGENCIA ---\n");
    printf("%s nao se sente bem neste local.\n", user.nome);
    printf("Por favor, entre em contato ou va ate a localizacao enviada.\n");

    printf("\nContatos notificados:\n");
    printf("- %s (%s)\n", user.contatoNome1, user.contatoTelefone1);
    printf("- %s (%s)\n", user.contatoNome2, user.contatoTelefone2);

    printf("\nLocalizacao enviada para a policia [190] e contatos de emergencia.\n\n");
    console_pause();
}