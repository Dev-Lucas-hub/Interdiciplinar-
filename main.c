#include "viva_segura.h"
#include "db.h"
#include <sqlite3.h>

sqlite3 *db;

int main() {
    int op;

    if (db_init() != 0) {
        printf("[Erro] Falha ao inicializar o banco SQLite.\n");
        system("pause");
        return 1;
    }

   

do {
    menu();
    int op;
    char entrada[10];

    scanf("%9s", entrada);

    // valida se é número
    int valido = 1;
    int i;
    for (i = 0; entrada[i]; i++) {
        if (!isdigit(entrada[i])) {
            valido = 0;
            break;
        }
    }

    if (!valido) {
        printf("\n[Erro] Digite apenas numeros!\n");
        system("pause");
        continue;
    }

    op = atoi(entrada);

    // valida intervalo
    if (op < 0 || op > 8) {
        printf("\n[Erro] Opcao invalida!\n");
        system("pause");
        continue;
    }

    switch(op) {
        case 1: cadastro(); break;
        case 2: login(); break;
        case 3: verDados(); break;
        case 4: emergencia(); break;
        case 5: monitoramento(); break;
        case 6: checkin(); break;
        case 7: denuncia(); break;
        case 8: telefones(); break;
        case 0: printf("\nSaindo...\n"); break;
    }

} while(op != 0);

    return 0;
}
