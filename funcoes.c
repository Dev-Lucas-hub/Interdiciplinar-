#include "viva_segura.h"
#include "db.h"

struct Usuario user;
int logado = 0;
int cadastrado = 0;

int validarEmail(char email[]) {
    return strchr(email, '@') != NULL;
}

static int is_all_digits(const char *s) {
    if (!s || !*s) return 0;
    for (; *s; s++) {
        if (!isdigit((unsigned char)*s)) return 0;
    }
    return 1;
}

static int validarCPF(const char *cpf) {
    return cpf && strlen(cpf) == 11 && is_all_digits(cpf);
}

static int validarCEP(const char *cep) {
    return cep && strlen(cep) == 8 && is_all_digits(cep);
}

static int validarUF(char uf[]) {
    if (!uf) return 0;
    size_t n = strlen(uf);
    if (n != 2) return 0;
    uf[0] = (char)toupper((unsigned char)uf[0]);
    uf[1] = (char)toupper((unsigned char)uf[1]);
    return isalpha((unsigned char)uf[0]) && isalpha((unsigned char)uf[1]);
}

static int validarTelefone(const char *tel) {
    if (!tel) return 0;
    size_t n = strlen(tel);
    if (!(n == 10 || n == 11)) return 0;
    return is_all_digits(tel);
}

void menu() {
    system("cls");
    printf("================================================\n");
    printf("              SISTEMA VIVA SEGURA               \n");
    printf("================================================\n");
    if(logado) printf("  Sessao ativa: %s\n", user.nome);
    else       printf("  Status: Desconectado\n");
    printf("------------------------------------------------\n");
    printf("  [1] Cadastro        [5] Monitoramento\n");
    printf("  [2] Login           [6] Check-in\n");
    printf("  [3] Ver dados       [7] Denuncia\n");
    printf("  [4] Emergencia      [8] Telefones\n");
    printf("\n  [0] Sair\n");
    printf("================================================\n");
    printf("Escolha: ");
}

void cadastro() {
    char confirmarSenha[20]; 
    system("cls"); 

    printf(">>> NOVO CADASTRO <<<\n");

    printf("\n--- IDENTIFICACAO ---\n");
    printf("Nome Completo: "); scanf(" %49[^\n]", user.nome);
    printf("Data de Nascimento: "); scanf("%19s", user.nascimento);
    do {
        printf("CPF: "); scanf("%19s", user.cpf);
        if (!validarCPF(user.cpf)) printf("[!] CPF invalido. Use apenas 11 numeros.\n");
    } while (!validarCPF(user.cpf));

    printf("\n--- CREDENCIAIS ---\n");
    do {
        printf("E-mail: "); scanf("%49s", user.email);
        if (!validarEmail(user.email)) printf("[!] E-mail invalido.\n");
    } while (!validarEmail(user.email));

    printf("Defina sua Senha: "); scanf("%19s", user.senha);
    printf("Confirme a Senha: "); scanf("%19s", confirmarSenha);

    if (strcmp(user.senha, confirmarSenha) != 0) {
        printf("\n[Erro] As senhas nao coincidem!\n");
        system("pause"); return; 
    }

    printf("\n--- CONTATO E LOCALIZACAO ---\n");
    do {
        printf("Telefone: "); scanf("%19s", user.telefone);
        if (!validarTelefone(user.telefone)) printf("[!] Telefone invalido. Use 10 ou 11 numeros.\n");
    } while (!validarTelefone(user.telefone));
    printf("Endereco: "); scanf(" %99[^\n]", user.endereco);
    printf("Cidade: "); scanf(" %49[^\n]", user.cidade);
    do {
        printf("Estado (UF): "); scanf("%9s", user.estado);
        if (!validarUF(user.estado)) printf("[!] UF invalida. Ex: SP, RJ.\n");
    } while (!validarUF(user.estado));
    do {
        printf("CEP: "); scanf("%19s", user.cep);
        if (!validarCEP(user.cep)) printf("[!] CEP invalido. Use 8 numeros.\n");
    } while (!validarCEP(user.cep));

    printf("\n--- INFORMACOES MEDICAS ---\n");
    printf("Tipo Sanguineo: "); scanf("%4s", user.sangue);
    printf("Alergias: "); scanf(" %99[^\n]", user.alergias);

    printf("\n--- CONTATOS DE CONFIANCA ---\n");
    printf("Nome do Contato 1: "); scanf(" %49[^\n]", user.contatoNome1);
    do {
        printf("Telefone 1: "); scanf("%19s", user.contatoTelefone1);
        if (!validarTelefone(user.contatoTelefone1)) printf("[!] Telefone invalido.\n");
    } while (!validarTelefone(user.contatoTelefone1));
    printf("\nNome do Contato 2: "); scanf(" %49[^\n]", user.contatoNome2);
    do {
        printf("Telefone 2: "); scanf("%19s", user.contatoTelefone2);
        if (!validarTelefone(user.contatoTelefone2)) printf("[!] Telefone invalido.\n");
    } while (!validarTelefone(user.contatoTelefone2));

    printf("\n--- SEGURANCA ADICIONAL ---\n");
    printf("Deseja senha para desativar Check-in? (1-Sim/0-Nao): ");
    scanf("%d", &user.protecaoCheckin);

    if (user.protecaoCheckin == 1) {
        printf("Crie a senha do Check-in: "); scanf("%19s", user.senhaCheckin);
    } else {
        strcpy(user.senhaCheckin, "0");
    }

    {
        int rc = db_insert_usuario(&user);
        if (rc == 0) {
            printf("\n[Sucesso] Usuario cadastrado e salvo no banco!\n");
            cadastrado = 1;
        } else if (rc == 1) {
            printf("\n[Erro] Ja existe uma usuaria cadastrada com este e-mail.\n");
        } else {
            printf("\n[Erro] Falha ao salvar no banco (codigo %d).\n", rc);
        }
    }
    system("pause");
}

void login() {
    char emailB[50], senhaB[20];
    struct Usuario temp;

    printf("\n--- ACESSO AO SISTEMA ---\n");
    printf("E-mail: "); scanf("%49s", emailB);
    printf("Senha: "); scanf("%19s", senhaB);

    {
        int rc = db_get_usuario_by_email(emailB, &temp);
        if (rc == 0) {
            if (strcmp(senhaB, temp.senha) == 0) {
                user = temp;
                logado = 1;
                printf("\n[OK] Login efetuado. Bem-vinda, %s!\n", user.nome);
            } else {
                printf("\n[Erro] Senha incorreta.\n");
            }
        } else if (rc == 1) {
            printf("\n[Erro] Usuario nao encontrado.\n");
        } else {
            printf("\n[Erro] Falha ao consultar o banco (codigo %d).\n", rc);
        }
    }
    system("pause");
}

void verDados() {
    if (!logado) { printf("\n[!] Realize o login primeiro.\n"); system("pause"); return; }

    system("cls");
    printf("================================================\n");
    printf("              RELATORIO DE USUARIA               \n");
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

void emergencia() {
    if (!logado) { printf("\n[!] Realize o login primeiro.\n"); system("pause"); return; }

    system("cls");
    printf("================================================\n");
    printf("           !!! ALERTA DE EMERGENCIA !!!         \n");
    printf("================================================\n");
    printf(" USUARIA EM RISCO: %s\n", user.nome);
    printf(" LOCALIZACAO:      %s, %s - %s\n", user.endereco, user.cidade, user.estado);
    printf("------------------------------------------------\n");
    printf(" NOTIFICANDO CONTATOS DE CONFIANCA:\n");
    printf(" 1. %s (%s) -> SMS ENVIADO\n", user.contatoNome1, user.contatoTelefone1);
    printf(" 2. %s (%s) -> SMS ENVIADO\n", user.contatoNome2, user.contatoTelefone2);
    printf("------------------------------------------------\n");
    printf(" [!] ACIONANDO AUTORIDADES (190)...\n");
    printf("================================================\n");

    {
        int rc = db_log_emergencia(&user);
        if (rc != 0) {
            printf("\n[Erro] Falha ao registrar emergencia no banco (codigo %d).\n", rc);
        }
    }
    system("pause");
}

void monitoramento() {
    if (!logado) { printf("\n[!] Realize o login primeiro.\n"); system("pause"); return; }

    system("cls");
    printf("================================================\n");
    printf("            MONITORAMENTO EM TEMPO REAL         \n");
    printf("================================================\n");
    printf(" RASTREANDO:    %s\n", user.nome);
    printf(" LOCAL ATUAL:   %s, %s - %s\n", user.endereco, user.cidade, user.estado);
    printf("------------------------------------------------\n");
    printf(" COMPARTILHANDO ROTA COM:\n");
    printf(" 1. %s (%s)\n", user.contatoNome1, user.contatoTelefone1);
    printf(" 2. %s (%s)\n", user.contatoNome2, user.contatoTelefone2);
    printf("------------------------------------------------\n");
    printf(" [ATIVO] Seu percurso esta sendo vigiado.\n");
    printf("================================================\n");

    {
        int rc = db_log_monitoramento(&user);
        if (rc != 0) {
            printf("\n[Erro] Falha ao registrar monitoramento no banco (codigo %d).\n", rc);
        }
    }
    system("pause");
}

void checkin() {
    if (!logado) { printf("\n[!] Realize o login primeiro.\n"); system("pause"); return; }
    
    int op;
    system("cls");
    printf("================================================\n");
    printf("                SISTEMA DE CHECK-IN             \n");
    printf("================================================\n");
    printf(" USUARIA: %s\n", user.nome);
    printf("------------------------------------------------\n");
    printf(" [1] Ativar Check-in\n");
    printf(" [2] Desativar Check-in\n");
    printf(" [3] Ver Status Atual\n");
    printf(" [0] Voltar ao Menu\n");
    printf("------------------------------------------------\n");
    printf(" Escolha: ");
    scanf("%d", &op);

    switch(op) {
        case 1:
            printf("\n [OK] Check-in ativado. Notificaremos seus contatos.\n");
            break;
        
        case 2:
            if (user.protecaoCheckin == 1) {
                char s[20];
                printf("\n [!] SEGURANCA: Digite sua senha de Check-in: "); 
                scanf("%19s", s);
                if(strcmp(s, user.senhaCheckin) == 0) {
                    printf(" [OK] Check-in desativado com sucesso.\n");
                } else {
                    printf(" [!] Senha incorreta! Alerta de seguranca emitido.\n");
                }
            } else {
                printf(" [OK] Check-in desativado.\n");
            }
            break;

        case 3:
            printf("\n STATUS: Monitorando confirmacao em %s.\n", user.cidade);
            printf(" Proxima verificacao em breve.\n");
            break;

        case 0:
            return;

        default:
            printf("\n [!] Opcao invalida.\n");
            break;
    }
    
    {
        int rc = db_log_checkin(&user, op);
        if (rc != 0) {
            printf("\n[Erro] Falha ao registrar check-in no banco (codigo %d).\n", rc);
        }
    }
    system("pause");
}

void denuncia() {
    char relato[200]; int anon;
    system("cls");
    printf("================================================\n");
    printf("               REGISTRO DE DENUNCIA             \n");
    printf("================================================\n");
    printf(" Descreva a situacao: "); scanf(" %199[^\n]", relato);
    printf(" Manter anonimato? (1-Sim / 0-Nao): "); scanf("%d", &anon);

    {
        int rc = db_insert_denuncia(logado ? &user : NULL, relato, anon);
        if (rc == 0) {
            printf("\n [Sucesso] Sua denuncia foi registrada com seguranca.\n");
        } else {
            printf("\n [Erro] Falha ao registrar denuncia no banco (codigo %d).\n", rc);
        }
    }
    system("pause");
}

void telefones() {
    system("cls");
    printf("================================================\n");
    printf("            TELEFONES DE EMERGENCIA             \n");
    printf("================================================\n");
    printf("  190 - Policia Militar\n");
    printf("  192 - SAMU\n");
    printf("  193 - Bombeiros\n");
    printf("  180 - Central de Atendimento a Mulher\n");
    printf("------------------------------------------------\n");
    if(logado) {
        printf("  SEUS CONTATOS SALVOS:\n");
        printf("  - %s: %s\n", user.contatoNome1, user.contatoTelefone1);
        printf("  - %s: %s\n", user.contatoNome2, user.contatoTelefone2);
    }
    printf("================================================\n");
    system("pause");
}
