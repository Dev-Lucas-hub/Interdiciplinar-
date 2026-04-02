#include <stdio.h>

#include "../include/auth.h"
#include "../include/checkin.h"
#include "../include/denuncia.h"
#include "../include/emergencia.h"
#include "../include/menu.h"
#include "../include/monitoramento.h"
#include "../include/telefones.h"
#include "../include/ver_dados.h"

int main(void) {
    int op;

    do {
        menu();
        scanf("%d", &op);

        switch (op) {
            case 1: cadastro(); break;
            case 2: login(); break;
            case 3: verDados(); break;
            case 4: emergencia(); break;
            case 5: monitoramento(); break;
            case 6: checkin(); break;
            case 7: denuncia(); break;
            case 8: telefones(); break;
        }
    } while (op != 0);

    return 0;
}
