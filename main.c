#include "viva_segura.h"
#include "db.h"

int main() {
    int op;

    if (db_init() != 0) {
        printf("[Erro] Falha ao inicializar o banco SQLite.\n");
        system("pause");
        return 1;
    }

    do {
        menu();
        if (scanf("%d", &op) != 1) {
            while(getchar() != '\n');
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
            default: printf("\nInvalido!\n"); system("pause");
        }
    } while(op != 0);

    return 0;
}