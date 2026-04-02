#include <stdio.h>

#include "../include/app_state.h"
#include "../include/console.h"
#include "../include/storage.h"
#include "../include/ver_dados.h"

void verDados(void) {
    if (!logado) {
        printf("\nFaca login primeiro!\n\n");
        console_pause();
        return;
    }

    if (!storage_refresh_usuario_sessao(&user)) {
        printf("\nNao foi possivel recarregar a sessao a partir do usuario.csv.\n\n");
        console_pause();
        return;
    }

    printf("\n========== TODOS OS USUARIOS ==========\n");

    storage_imprimir_todos_usuarios(user.id_usuario);

    console_pause();
}