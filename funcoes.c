#include "viva_segura.h"
#include "db.h"
#include <sqlite3.h>

extern sqlite3 *db;

struct Usuario user;
int logado = 0;
int cadastrado = 0;

/* ================= VALIDACOES ================= */

int validarEmail(char email[])
{
    return strchr(email, '@') != NULL;
}

int validarNome(char nome[])
{
    int i;

    for (i = 0; nome[i] != '\0'; i++)
    {
        if (isdigit((unsigned char)nome[i]))
            return 0;
    }

    return strlen(nome) > 0;
}

int validarData(char data[])
{
    if (strlen(data) != 10)
        return 0;

    if (data[2] != '/' || data[5] != '/')
        return 0;

    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
            continue;

        if (!isdigit((unsigned char)data[i]))
            return 0;
    }

    return 1;
}

int validarNumero3(char txt[])
{
    int i, t = strlen(txt);

    if (t < 1 || t > 3)
        return 0;

    for (i = 0; txt[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)txt[i]))
            return 0;
    }

    return 1;
}

static int is_all_digits(const char *s)
{
    if (!s || !*s)
        return 0;

    for (; *s; s++)
    {
        if (!isdigit((unsigned char)*s))
            return 0;
    }
    return 1;
}

void limparNumero(char *entrada, char *saida)
{
    int j = 0;
    int i;

    for (i = 0; entrada[i] != '\0'; i++)
    {
        if (isdigit((unsigned char)entrada[i]))
        {
            saida[j++] = entrada[i];
        }
    }

    saida[j] = '\0';
}

int validarCPF(const char *cpf)
{
    return strlen(cpf) == 11;
}

void lerSenha(char *senha, const char *label) {
    int i = 0;
    char ch;
    int mostrar = 0;

    // MOSTRA UMA VEZ ANTES DE DIGITAR
    printf("%s [OCULTO]: ", label);

    while (1) {
        ch = _getch();

        if (ch == 13) { // ENTER
            senha[i] = '\0';
            break;
        }

        else if (ch == 8) { // BACKSPACE
            if (i > 0) i--;
        }

        else if (ch == 'v' || ch == 'V') {
            mostrar = !mostrar;
        }

        else {
            if (i < 19) {
                senha[i++] = ch;
            }
        }

        
        printf("\r%-60s", ""); // limpa tudo

        // ATUALIZA A MESMA LINHA (sem sumir o texto)
        printf("\r%s [%s]: ", label, mostrar ? "VISIVEL" : "OCULTO");
        int j;

        for (j = 0; j < i; j++) {
            printf(mostrar ? "%c" : "*", senha[j]);
        }

        printf("\033[K"); // limpa só o restante da linha 
    }
}

int senhaForte(const char *senha)
{
    int temNumero = 0, temLetra = 0;

    if (strlen(senha) < 8)
        return 0;
        int i;
       
    for (i = 0; senha[i]; i++)
    {
        if (isdigit((unsigned char)senha[i]))
            temNumero = 1;

        if (isalpha((unsigned char)senha[i]))
            temLetra = 1;
    }

    return temNumero && temLetra;
}

static int validarCEP(const char *cep)
{
    return cep && strlen(cep) == 8 && is_all_digits(cep);
}

static int validarUF(char uf[])
{
    if (!uf)
        return 0;

    if (strlen(uf) != 2)
        return 0;

    uf[0] = toupper((unsigned char)uf[0]);
    uf[1] = toupper((unsigned char)uf[1]);

    return isalpha((unsigned char)uf[0]) &&
           isalpha((unsigned char)uf[1]);
}

static int validarTelefone(const char *tel)
{
    if (!tel)
        return 0;

    if (!(strlen(tel) == 10 || strlen(tel) == 11))
        return 0;

    return is_all_digits(tel);
}

/* ================= MENU ================= */

void menu()
{
    system("cls");

    printf("================================================\n");
    printf("              SISTEMA VIVA SEGURA               \n");
    printf("================================================\n");

    if (logado)
        printf("  Sessao ativa: %s\n", user.nome);
    else
        printf("  Status: Desconectado\n");

    printf("------------------------------------------------\n");
    printf("  [1] Cadastro        [5] Monitoramento\n");
    printf("  [2] Login           [6] Check-in\n");
    printf("  [3] Ver dados       [7] Denuncia\n");
    printf("  [4] Emergencia      [8] Telefones\n");
    printf("\n  [0] Sair\n");
    printf("================================================\n");
    printf("Escolha: ");
}

/* ================= CADASTRO ================= */

void cadastro()
{
    char confirmarSenha[20];
    char cpfDigitado[20];
    char cpfLimpo[20];
    int valido = 0;

    system("cls");

    printf(">>> NOVO CADASTRO <<<\n");

    printf("\n--- IDENTIFICACAO ---\n");

    do
    {
        printf("Nome Completo: ");
        scanf(" %49[^\n]", user.nome);

        if (!validarNome(user.nome))
            printf("Nome invalido! Nao use numeros.\n");

    } while (!validarNome(user.nome));

    do
    {
        printf("Data de Nascimento: ");
        scanf("%19s", user.nascimento);

        if (!validarData(user.nascimento))
            printf("Data invalida! Use dd/mm/aaaa\n");

    } while (!validarData(user.nascimento));

    do
    {
        printf("Digite o CPF: ");
        scanf("%19s", cpfDigitado);

        limparNumero(cpfDigitado, cpfLimpo);

        if (strlen(cpfLimpo) != 11)
        {
            printf("CPF deve conter exatamente 11 numeros!\n");
            continue;
        }

        if (!validarCPF(cpfLimpo))
        {
            printf("CPF invalido!\n");
            continue;
        }

        if (cpfExiste(db, cpfLimpo))
        {
            printf("CPF ja cadastrado!\n");
            continue;
        }

        valido = 1;

    } while (!valido);

    strcpy(user.cpf, cpfLimpo);

    printf("\n--- CREDENCIAIS ---\n");
    do
    {
        printf("E-mail: ");
        scanf("%49s", user.email);

        if (!validarEmail(user.email))
            printf("[!] E-mail invalido.\n");

    } while (!validarEmail(user.email));

    do
{
    lerSenha(user.senha, "Defina sua senha (V=mostrar/ocultar)");
    printf("\n");

    if (!senhaForte(user.senha))
    {
        printf("\n[Erro] A senha deve ter no minimo 8 caracteres, com letras e numeros.\n\n");
        continue;
    }

    lerSenha(confirmarSenha, "Confirme a senha (V=mostrar/ocultar)");
    printf("\n");

    // ===== NOVA PARTE AQUI =====
    int mostrar = 0;
    char ch;

    while (1) {
        system("cls");

        printf("=== CONFIRMACAO DE SENHA ===\n\n");

        printf("Senha:    ");

        //int i;
        for (int i = 0; user.senha[i]; i++)
            printf(mostrar ? "%c" : "*", user.senha[i]);

        printf("\nConfirmacao: ");

        //int i;
        for (int i = 0; confirmarSenha[i]; i++)
            printf(mostrar ? "%c" : "*", confirmarSenha[i]);

        printf("\n\n[V] Mostrar/Ocultar  |  [Enter] Continuar\n");

        ch = _getch();

        if (ch == 'v' || ch == 'V') {
            mostrar = !mostrar;
        } 
        else if (ch == 13) {
            break;
        }
    }
    // ===== FIM DA NOVA PARTE =====

    if (strcmp(user.senha, confirmarSenha) != 0)
        printf("\n[Erro] As senhas nao coincidem. Digite novamente!\n\n");

} while (!senhaForte(user.senha) || strcmp(user.senha, confirmarSenha) != 0);

    //  HASH (continua igual)
    int hash = 0;
    int i;

    for (i = 0; user.senha[i]; i++)
        hash += user.senha[i];

    sprintf(user.senha, "%d", hash);

    printf("\n--- CONTATO E LOCALIZACAO ---\n");

    do
    {
        printf("Telefone: ");
        scanf("%19s", user.telefone);

        if (!validarTelefone(user.telefone))
            printf("[!] Telefone invalido. Use 10 ou 11 numeros.\n");

    } while (!validarTelefone(user.telefone));

    printf("Endereco: ");
    scanf(" %99[^\n]", user.endereco);

    printf("Cidade: ");
    scanf(" %49[^\n]", user.cidade);

    do
    {
        printf("Estado (UF): ");
        scanf("%9s", user.estado);

        if (!validarUF(user.estado))
            printf("[!] UF invalida. Ex: SP, RJ.\n");

    } while (!validarUF(user.estado));

    do
    {
        printf("CEP: ");
        scanf("%19s", user.cep);

        if (!validarCEP(user.cep))
            printf("[!] CEP invalido. Use 8 numeros.\n");

    } while (!validarCEP(user.cep));

    printf("\n--- INFORMACOES MEDICAS ---\n");
    printf("Tipo Sanguineo: ");
    scanf("%4s", user.sangue);

    printf("Alergias: ");
    scanf(" %99[^\n]", user.alergias);

    printf("\n--- CONTATOS DE CONFIANCA ---\n");

    do
    {
        printf("Nome do Contato 1: ");
        scanf(" %49[^\n]", user.contatoNome1);

        if (!validarNome(user.contatoNome1))
            printf("Nome invalido!\n");

    } while (!validarNome(user.contatoNome1));

    do
    {
        printf("Telefone 1: ");
        scanf("%19s", user.contatoTelefone1);

        if (!validarTelefone(user.contatoTelefone1))
            printf("[!] Telefone invalido.\n");

    } while (!validarTelefone(user.contatoTelefone1));

    do
    {
        printf("\nNome do Contato 2: ");
        scanf(" %49[^\n]", user.contatoNome2);

        if (!validarNome(user.contatoNome2))
            printf("Nome invalido!\n");

    } while (!validarNome(user.contatoNome2));

    do
    {
        printf("Telefone 2: ");
        scanf("%19s", user.contatoTelefone2);

        if (!validarTelefone(user.contatoTelefone2))
            printf("[!] Telefone invalido.\n");

    } while (!validarTelefone(user.contatoTelefone2));

    printf("\n--- SEGURANCA ADICIONAL ---\n");
    printf("Deseja senha para desativar Check-in? (1-Sim/0-Nao): ");
    scanf("%d", &user.protecaoCheckin);

    if (user.protecaoCheckin == 1)
    {
        char senhaTemp[20];
        char confirmar[20];

       do
{
    lerSenha(senhaTemp, "Crie a senha do Check-in (V=mostrar/ocultar)");
    printf("\n");

    lerSenha(confirmar, "Confirme a senha (V=mostrar/ocultar)");
    printf("\n");

    // ===== VISUALIZAR SENHAS =====
    int mostrar = 0;
    char ch;

    while (1) {
        system("cls");

        printf("=== CONFIRMACAO DE SENHA DO CHECK-IN ===\n\n");

        printf("Senha:    ");

        //int i;
        for (int i = 0; senhaTemp[i]; i++)
            printf(mostrar ? "%c" : "*", senhaTemp[i]);

        printf("\nConfirmacao: ");

        //int i;
        for (int i = 0; confirmar[i]; i++)
            printf(mostrar ? "%c" : "*", confirmar[i]);

        printf("\n\n[V] Mostrar/Ocultar  |  [Enter] Continuar\n");

        ch = _getch();

        if (ch == 'v' || ch == 'V') {
            mostrar = !mostrar;
        } 
        else if (ch == 13) {
            break;
        }
    }
    // ===== FIM =====

    if (strcmp(senhaTemp, confirmar) != 0)
    {
        printf("\n[Erro] As senhas nao coincidem!\n\n");
    }

} while (strcmp(senhaTemp, confirmar) != 0);

        //  GERAR HASH
        int hash = 0;
        int i;
        for (i = 0; senhaTemp[i]; i++)
        {
            hash += senhaTemp[i];
        }

        sprintf(user.senhaCheckin, "%d", hash);
    }
    else
    {
        strcpy(user.senhaCheckin, "0");
    }

    {
        int rc = db_insert_usuario(&user);

        if (rc == 0)
        {
            printf("\n[Sucesso] Usuario cadastrado e salvo no banco!\n");
            cadastrado = 1;
        }
        else if (rc == 1)
        {
            printf("\n[Erro] Ja existe um usuario cadastrado com este e-mail.\n");
        }
        else if (rc == 2)
        {
            printf("\n[Erro] Ja existe um usuario cadastrado com este CPF.\n");
        }
        else
        {
            printf("\n[Erro] Falha ao salvar no banco (codigo %d).\n", rc);
        }
    }

    system("pause");
}
void login()
{
    char emailB[50], senhaB[20];
    struct Usuario temp;

    printf("\n--- ACESSO AO SISTEMA ---\n");
    printf("E-mail: ");
    scanf("%49s", emailB);

    lerSenha(senhaB, "Digite sua senha (V=mostrar/ocultar)");
    printf("\n");

    {
        int rc = db_get_usuario_by_email(emailB, &temp);

        if (rc == 0)
        {

            int hash = 0;
            char hashStr[20];
            int i;

            for (i = 0; senhaB[i]; i++)
                hash += senhaB[i];

            sprintf(hashStr, "%d", hash);

            if (strcmp(hashStr, temp.senha) == 0)
            {
                user = temp;
                logado = 1;

                printf("\n[OK] Login efetuado. Bem-vinda, %s!\n", user.nome);
            }
            else
            {
                printf("\n[Erro] Senha incorreta.\n");
            }
        }
        else if (rc == 1)
        {
            printf("\n[Erro] Usuario nao encontrado.\n");
        }
        else
        {
            printf("\n[Erro] Falha ao consultar o banco (codigo %d).\n", rc);
        }
    }

    system("pause");
}

void verDados()
{
    if (!logado)
    {
        printf("\n[!] Realize o login primeiro.\n");
        system("pause");
        return;
    }

    system("cls");

    printf("================================================\n");
    printf("              RELATORIO DE USUARIA              \n");
    printf("================================================\n");
    printf(" NOME:            %s\n", user.nome);
    printf(" CPF:             %s\n", user.cpf);
    printf(" DATA NASC.:      %s\n", user.nascimento);
    printf(" E-MAIL:          %s\n", user.email);
    printf(" TELEFONE:        %s\n", user.telefone);
    printf("------------------------------------------------\n");
    printf(" ENDERECO:        %s\n", user.endereco);
    printf(" CIDADE/UF:       %s - %s\n", user.cidade, user.estado);
    printf(" CEP:             %s\n", user.cep);
    printf("------------------------------------------------\n");
    printf(" TIPO SANGUINEO:  %s\n", user.sangue);
    printf(" ALERGIAS:        %s\n", user.alergias);
    printf("------------------------------------------------\n");
    printf(" CONTATO EMERG. 1: %s (%s)\n", user.contatoNome1, user.contatoTelefone1);
    printf(" CONTATO EMERG. 2: %s (%s)\n", user.contatoNome2, user.contatoTelefone2);
    printf("================================================\n");

    system("pause");
}

void emergencia()
{
    if (!logado)
    {
        printf("\n[!] Realize o login primeiro.\n");
        system("pause");
        return;
    }

    system("cls");

    printf("================================================\n");
    printf("           !!! ALERTA DE EMERGENCIA !!!         \n");
    printf("================================================\n");
    printf(" USUARIA EM RISCO: %s\n", user.nome);
    printf(" LOCALIZACAO:      %s, %s - %s\n",
           user.endereco, user.cidade, user.estado);
    printf("------------------------------------------------\n");
    printf(" NOTIFICANDO CONTATOS DE CONFIANCA:\n");
    printf(" 1. %s (%s) -> SMS ENVIADO\n",
           user.contatoNome1, user.contatoTelefone1);
    printf(" 2. %s (%s) -> SMS ENVIADO\n",
           user.contatoNome2, user.contatoTelefone2);
    printf("------------------------------------------------\n");
    printf(" [!] ACIONANDO AUTORIDADES (190)...\n");
    printf("================================================\n");

    {
        int rc = db_log_emergencia(&user);

        if (rc != 0)
            printf("\n[Erro] Falha ao registrar emergencia no banco.\n");
    }

    system("pause");
}

void monitoramento()
{
    if (!logado)
    {
        printf("\n[!] Realize o login primeiro.\n");
        system("pause");
        return;
    }

    system("cls");

    printf("================================================\n");
    printf("            MONITORAMENTO EM TEMPO REAL         \n");
    printf("================================================\n");
    printf(" RASTREANDO:    %s\n", user.nome);
    printf(" LOCAL ATUAL:   %s, %s - %s\n",
           user.endereco, user.cidade, user.estado);
    printf("------------------------------------------------\n");
    printf(" COMPARTILHANDO ROTA COM:\n");
    printf(" 1. %s (%s)\n", user.contatoNome1, user.contatoTelefone1);
    printf(" 2. %s (%s)\n", user.contatoNome2, user.contatoTelefone2);
    printf("------------------------------------------------\n");
    printf(" [ATIVO] Seu percurso esta sendo vigiado.\n");
    printf("================================================\n");

    {
        int rc = db_log_monitoramento(&user);

        if (rc != 0)
            printf("\n[Erro] Falha ao registrar monitoramento no banco.\n");
    }

    system("pause");
}
void checkin()
{
    if (!logado)
    {
        printf("\n[!] Realize o login primeiro.\n");
        system("pause");
        return;
    }

    int op;

    do
    {
        system("cls");

        printf("\n================================================\n");
        printf("            SISTEMA DE CHECK-IN\n");
        printf("================================================\n");
        printf(" USUARIA: %s\n", user.nome);
        printf("------------------------------------------------\n");
        printf(" [1] Ativar Check-in\n");
        printf(" [2] Desativar Check-in\n");
        printf(" [3] Ver Status Detalhado\n");
        printf(" [4] Configurar Intervalo\n");
        printf(" [0] Voltar ao menu\n");
        printf("------------------------------------------------\n");
        printf(" Escolha: ");

        if (scanf("%d", &op) != 1)
        {
            printf("\n[Erro] Entrada invalida!\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (op)
        {

        case 1:
            if (user.protecaoCheckin == 1)
            {
                printf("\n[INFO] Check-in ja esta ativo.\n");
            }
            else
            {
                user.protecaoCheckin = 1;

                printf("\n[OK] Check-in ativado com sucesso!\n");
                printf(" Seus contatos serao notificados.\n");
                printf(" Monitoramento iniciado em %s.\n", user.cidade);
                printf(" Intervalo de envio: %d minutos.\n",
                       user.intervaloMinutos);
            }
            break;

        case 2:
            if (user.protecaoCheckin == 1)
            {
                char senha[20];

                printf("\n[SEGURANCA]\n");
                lerSenha(senha, "Digite sua senha (V=mostrar/ocultar)");
                printf("\n"); //  senha escondida

                int hash = 0;
                char hashStr[20];
                int i;

                // gera hash
                for (i = 0; senha[i]; i++)
                {
                    hash += senha[i];
                }

                sprintf(hashStr, "%d", hash);

                if (strcmp(hashStr, user.senhaCheckin) == 0)
                {
                    user.protecaoCheckin = 0;

                    printf("\n[OK] Check-in desativado.\n");
                    printf(" Monitoramento encerrado.\n");
                    printf(" Seus contatos nao receberao mais alertas.\n");
                }
                else
                {
                    printf("\n[ALERTA] Senha incorreta!\n");
                    printf(" Possivel tentativa suspeita! Alerta de seguranca emitido.\n");
                }
            }
            else
            {
                printf("\n[INFO] Check-in ja esta desativado.\n");
            }
            break;

        case 3:
            printf("\n=========== STATUS DETALHADO ===========\n");
            printf(" Usuaria: %s\n", user.nome);
            printf(" Cidade atual: %s\n", user.cidade);
            printf(" Check-in: %s\n",
                   user.protecaoCheckin ? "ATIVO" : "INATIVO");
            printf(" Intervalo: %d minutos\n", user.intervaloMinutos);
            if (user.protecaoCheckin)
            {
                printf(" Proxima atualizacao em breve...\n");
                printf(" Sistema monitorando localizacao.\n");
            }
            else
            {
                printf(" Sistema inativo.\n");
            }
            printf("========================================\n");
            break;

        case 4:
        {
            char entrada[10];

            do
            {
                printf("\nDigite o intervalo (em minutos): ");
                scanf("%9s", entrada);

                if (!validarNumero3(entrada))
                    printf("[Erro] Digite apenas numeros ate 3 digitos!\n");

            } while (!validarNumero3(entrada));

            user.intervaloMinutos = atoi(entrada);

            printf("[OK] Intervalo atualizado para %d minutos.\n",
                   user.intervaloMinutos);

            // while(getchar() != '\n');
            break;
        }

        case 0:
            printf("\nRetornando ao menu inicial...\n");
            break;

        default:
            printf("\n[Erro] Opcao invalida!\n");
        }

        {
            int rc = db_log_checkin(&user, op);

            if (rc != 0)
                printf("\n[Erro] Falha ao registrar check-in no banco.\n");
        }

        printf("\nPressione ENTER para continuar...");
        while (getchar() != '\n')
            ;
        getchar();

    } while (op != 0);
}

void denuncia()
{
    char relato[200];
    int anon;
    int valido = 0;

    system("cls");

    printf("================================================\n");
    printf("               REGISTRO DE DENUNCIA             \n");
    printf("================================================\n");

    printf(" Descreva a situacao: ");
    scanf(" %199[^\n]", relato);

    // ===== VALIDAÇÃO 1 OU 0 =====
    do {
        printf(" Manter anonimato? (1-Sim / 0-Nao): ");

        if (scanf("%d", &anon) != 1) {
            printf("[Erro] Entrada invalida!\n");
            while (getchar() != '\n'); // limpa buffer
            continue;
        }

        if (anon != 0 && anon != 1) {
            printf("[Erro] Digite apenas 1 (Sim) ou 0 (Nao).\n");
            continue;
        }

        valido = 1;

    } while (!valido);
    // ===== FIM =====

    {
        int rc = db_insert_denuncia(logado ? &user : NULL, relato, anon);

        if (rc == 0) {
            if (anon == 1)
                printf("\n[Sucesso] Denuncia registrada ANONIMAMENTE.\n");
            else
                printf("\n[Sucesso] Denuncia registrada com seus dados.\n");
        }
        else {
            printf("\n[Erro] Falha ao registrar denuncia.\n");
        }
    }

    system("pause");
}
void telefones()
{
    system("cls");

    printf("================================================\n");
    printf("            TELEFONES DE EMERGENCIA             \n");
    printf("================================================\n");
    printf("  190 - Policia Militar\n");
    printf("  192 - SAMU\n");
    printf("  193 - Bombeiros\n");
    printf("  180 - Central de Atendimento a Mulher\n");
    printf("------------------------------------------------\n");

    if (logado)
    {
        printf("  SEUS CONTATOS SALVOS:\n");
        printf("  - %s: %s\n",
               user.contatoNome1, user.contatoTelefone1);

        printf("  - %s: %s\n",
               user.contatoNome2, user.contatoTelefone2);
    }

    printf("================================================\n");

    system("pause");
}