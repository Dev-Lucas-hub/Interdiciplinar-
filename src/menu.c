#include <stdio.h>

#include "../include/console.h"
#include "../include/menu.h"

void menu(void) {
    console_clear();

    printf("========================================\n");
    printf("        SISTEMA VIVA SEGURA\n");
    printf("========================================\n");

    printf("\n[1] Cadastro\n");
    printf("[2] Login\n");
    printf("[3] Ver dados\n");
    printf("[4] Emergencia\n");
    printf("[5] Monitoramento\n");
    printf("[6] Check-in\n");
    printf("[7] Denuncia\n");
    printf("[8] Telefones\n");

    printf("\n[0] Sair\n");
    printf("========================================\n");

    printf("Escolha: ");
}