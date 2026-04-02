#include <stdio.h>
#include <string.h>

#include "../include/app_state.h"
#include "../include/checkin.h"
#include "../include/console.h"
#include "../include/storage.h"

void checkin(void) {
    if (!logado) {
        printf("\nFaca login primeiro!\n\n");
        console_pause();
        return;
    }

    int op;

    printf("========================================\n");
    printf("             CHECK-IN\n");
    printf("========================================\n");

    printf("\n1 - Ativar\n");
    printf("2 - Desativar\n");
    printf("3 - Configurar\n");
    printf("4 - Status\n");

    printf("\nEscolha: ");
    scanf("%d", &op);

    switch (op) {
        case 1:
            printf("\nCheck-in ativado!\n");
            printf("A localizacao sera enviada automaticamente.\n");
            storage_append_checkin_virtual(user.id_usuario, "Check-in ativado",
                                           "seguro");
            break;

        case 2:
            if (user.protecaoCheckin == 1) {
                char senha[20];
                printf("\nDigite a senha: ");
                scanf("%s", senha);

                if (strcmp(senha, user.senhaCheckin) == 0) {
                    printf("\nCheck-in desativado com sucesso!\n");
                    storage_append_checkin_virtual(user.id_usuario,
                                                   "Check-in desativado",
                                                   "seguro");
                } else
                    printf("\nSenha incorreta! Nao foi possivel desativar.\n");
            } else {
                printf("\nCheck-in desativado!\n");
                storage_append_checkin_virtual(user.id_usuario,
                                               "Check-in desativado", "seguro");
            }
            break;

        case 3:
            printf("\n===== CONFIGURACAO =====\n");

            printf("Escolha o contato (1 ou 2): ");
            scanf("%d", &user.contatoCheckin);

            printf("Intervalo de envio (em minutos): ");
            scanf("%d", &user.intervaloCheckin);

            printf("Mensagem que sera enviada:\n");
            scanf(" %[^\n]", user.mensagemCheckin);

            printf("\nConfiguracao salva com sucesso!\n");
            if (!storage_update_usuario(&user))
                printf("(Aviso: nao foi possivel gravar no arquivo.)\n");
            break;

        case 4:
            printf("\n===== STATUS DO CHECK-IN =====\n");

            printf("Contato selecionado: %d\n", user.contatoCheckin);
            printf("Intervalo: %d minutos\n", user.intervaloCheckin);
            printf("Mensagem: %s\n", user.mensagemCheckin);

            if (user.protecaoCheckin == 1)
                printf("Protecao: Ativada\n");
            else
                printf("Protecao: Desativada\n");
            break;

        default:
            printf("\nOpcao invalida!\n");
    }

    console_pause();
}