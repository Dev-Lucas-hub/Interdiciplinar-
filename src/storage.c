#include "../include/storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define DATA_DIR "data"
#define P_USUARIO DATA_DIR "/usuario.csv"
#define P_USUARIO_TMP DATA_DIR "/usuario.csv.tmp"
#define P_ENDERECO DATA_DIR "/endereco.csv"
#define P_TELEFONE DATA_DIR "/telefone.csv"
#define P_DENUNCIA DATA_DIR "/denuncia.csv"
#define P_EMERGENCIA DATA_DIR "/emergencia.csv"
#define P_CHECKIN DATA_DIR "/checkin_virtual.csv"
#define P_MEDIDA DATA_DIR "/medida_protetiva.csv"
#define P_ATENDIMENTO DATA_DIR "/atendimento.csv"
#define P_PROFISSIONAL DATA_DIR "/profissional.csv"
#define P_SERVICO DATA_DIR "/servico.csv"

#define HDR_USUARIO                                                           \
    "id_usuario;nome;cpf;data_nascimento;email;senha;telefone;cidade;sangue;" \
    "alergias;contatoNome1;contatoTelefone1;contatoNome2;contatoTelefone2;"    \
    "contatoCheckin;intervaloCheckin;mensagemCheckin;protecaoCheckin;"        \
    "senhaCheckin;endereco"

#define HDR_ENDERECO "ID_endereco;Rua;Numero;Bairro;Cep;Complemento;ende_do_usuario"
#define HDR_TELEFONE "ID_numero;chk_tipo_tel;telefone;ID_codigo_do;tel_do_usuario"
#define HDR_DENUNCIA "id_denuncia;descricao;data_denuncia;horario;id_usuario"
#define HDR_EMERGENCIA "id_emergencia;local_emergencia;horario;id_usuario"
#define HDR_CHECKIN "id_checkin;localizacao;data_hora;status_seguranca;checkin_usuario"
#define HDR_MEDIDA                                                            \
    "id_medida;numero_processo;data_inicio;data_fim;descricao;mp_usuario"
#define HDR_ATENDIMENTO                                                       \
    "id_atendimento;data_atendimento;protocolo;descricao;id_usuario;"         \
    "id_profissional"
#define HDR_PROFISSIONAL "id_profissional;nome;especialidade"
#define HDR_SERVICO "id_servico;nome;descricao"

static void ensure_data_dir(void) {
#ifdef _WIN32
    _mkdir(DATA_DIR);
#else
    mkdir(DATA_DIR, 0755);
#endif
}

static void ensure_file(const char *path, const char *header) {
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return;
    }
    f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "%s\n", header);
    fclose(f);
}

static void strip_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[n - 1] = '\0';
        --n;
    }
}

static void csv_sanitize(char *s) {
    for (; *s; ++s) {
        if (*s == ';' || *s == '\n' || *s == '\r') *s = ' ';
    }
}

static int first_id_from_line(const char *line) {
    char buf[32];
    int i = 0;
    while (line[i] && line[i] != ';' && i < (int)sizeof(buf) - 1) {
        buf[i] = line[i];
        ++i;
    }
    buf[i] = '\0';
    return atoi(buf);
}

static int next_id_in_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 1;
    char line[4096];
    int max_id = 0;
    if (!fgets(line, sizeof line, f)) {
        fclose(f);
        return 1;
    }
    while (fgets(line, sizeof line, f)) {
        strip_newline(line);
        if (!line[0]) continue;
        int id = first_id_from_line(line);
        if (id > max_id) max_id = id;
    }
    fclose(f);
    return max_id + 1;
}

static void now_date_time(char *date_buf, size_t date_sz, char *time_buf, size_t time_sz) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if (!tm) {
        date_buf[0] = '\0';
        time_buf[0] = '\0';
        return;
    }
    strftime(date_buf, date_sz, "%Y-%m-%d", tm);
    strftime(time_buf, time_sz, "%H:%M:%S", tm);
}

static void now_datetime(char *buf, size_t sz) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if (!tm) {
        buf[0] = '\0';
        return;
    }
    strftime(buf, sz, "%Y-%m-%d %H:%M:%S", tm);
}

/* strtok(";") junta delimitadores vizinhos e some campos vazios (";;"). */
static int split_semicolons(char *line, char *cols[], int maxcols) {
    strip_newline(line);
    if ((unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF) {
        memmove(line, line + 3, strlen(line + 3) + 1);
    }
    int n = 0;
    char *cur = line;
    for (;;) {
        if (n >= maxcols) break;
        cols[n++] = cur;
        char *semi = strchr(cur, ';');
        if (!semi) break;
        *semi = '\0';
        cur = semi + 1;
    }
    return n;
}

static int parse_usuario_line(char *line, Usuario *u) {
    char work[4096];
    if (strlen(line) >= sizeof work) return 0;
    strcpy(work, line);

    char *cols[32];
    int n = split_semicolons(work, cols, (int)(sizeof cols / sizeof cols[0]));
    if (n < 20) return 0;

    memset(u, 0, sizeof *u);
    u->id_usuario = atoi(cols[0]);
    strncpy(u->nome, cols[1], sizeof u->nome - 1);
    strncpy(u->cpf, cols[2], sizeof u->cpf - 1);
    strncpy(u->nascimento, cols[3], sizeof u->nascimento - 1);
    strncpy(u->email, cols[4], sizeof u->email - 1);
    strncpy(u->senha, cols[5], sizeof u->senha - 1);
    strncpy(u->telefone, cols[6], sizeof u->telefone - 1);
    strncpy(u->cidade, cols[7], sizeof u->cidade - 1);
    strncpy(u->sangue, cols[8], sizeof u->sangue - 1);
    strncpy(u->alergias, cols[9], sizeof u->alergias - 1);
    strncpy(u->contatoNome1, cols[10], sizeof u->contatoNome1 - 1);
    strncpy(u->contatoTelefone1, cols[11], sizeof u->contatoTelefone1 - 1);
    strncpy(u->contatoNome2, cols[12], sizeof u->contatoNome2 - 1);
    strncpy(u->contatoTelefone2, cols[13], sizeof u->contatoTelefone2 - 1);
    u->contatoCheckin = atoi(cols[14]);
    u->intervaloCheckin = atoi(cols[15]);
    strncpy(u->mensagemCheckin, cols[16], sizeof u->mensagemCheckin - 1);
    u->protecaoCheckin = atoi(cols[17]);
    strncpy(u->senhaCheckin, cols[18], sizeof u->senhaCheckin - 1);
    strncpy(u->endereco, cols[19], sizeof u->endereco - 1);
    return 1;
}

static void imprimir_campos_usuario_console(const Usuario *u) {
    printf("id_usuario:        %d\n", u->id_usuario);
    printf("nome:              %s\n", u->nome);
    printf("cpf:               %s\n", u->cpf);
    printf("data_nascimento:   %s\n", u->nascimento);
    printf("email:             %s\n", u->email);
    printf("senha:             %s\n", u->senha);
    printf("telefone:          %s\n", u->telefone);
    printf("cidade:            %s\n", u->cidade);
    printf("sangue:            %s\n", u->sangue);
    printf("alergias:          %s\n", u->alergias);
    printf("contatoNome1:      %s\n", u->contatoNome1);
    printf("contatoTelefone1:  %s\n", u->contatoTelefone1);
    printf("contatoNome2:      %s\n", u->contatoNome2);
    printf("contatoTelefone2:  %s\n", u->contatoTelefone2);
    printf("contatoCheckin:    %d\n", u->contatoCheckin);
    printf("intervaloCheckin:  %d\n", u->intervaloCheckin);
    printf("mensagemCheckin:   %s\n", u->mensagemCheckin);
    printf("protecaoCheckin:   %d\n", u->protecaoCheckin);
    printf("senhaCheckin:      %s\n", u->senhaCheckin);
    printf("endereco:          %s\n", u->endereco);
}

void storage_imprimir_todos_usuarios(int id_usuario_em_sessao) {
    FILE *f = fopen(P_USUARIO, "r");
    if (!f) {
        printf("\nNao foi possivel abrir usuario.csv.\n");
        return;
    }

    char line[4096];
    if (!fgets(line, sizeof line, f)) {
        fclose(f);
        printf("\nusuario.csv esta vazio.\n");
        return;
    }

    int total = 0;
    while (fgets(line, sizeof line, f)) {
        char work[4096];
        if (strlen(line) >= sizeof work) continue;
        strcpy(work, line);
        Usuario u;
        if (!parse_usuario_line(work, &u)) continue;

        ++total;
        printf("\n---------- Cadastro #%d (id_usuario=%d) ----------\n", total,
               u.id_usuario);
        if (id_usuario_em_sessao > 0 && u.id_usuario == id_usuario_em_sessao)
            printf("[ Sessao atual: voce esta logado neste usuario ]\n");
        imprimir_campos_usuario_console(&u);
    }

    fclose(f);

    if (total == 0)
        printf("\n(Nenhum usuario no arquivo alem do cabecalho.)\n");
    else
        printf("\n========== Total no usuario: %d ==========\n", total);
}

void storage_init(void) {
    ensure_data_dir();
    ensure_file(P_USUARIO, HDR_USUARIO);
    ensure_file(P_ENDERECO, HDR_ENDERECO);
    ensure_file(P_TELEFONE, HDR_TELEFONE);
    ensure_file(P_DENUNCIA, HDR_DENUNCIA);
    ensure_file(P_EMERGENCIA, HDR_EMERGENCIA);
    ensure_file(P_CHECKIN, HDR_CHECKIN);
    ensure_file(P_MEDIDA, HDR_MEDIDA);
    ensure_file(P_ATENDIMENTO, HDR_ATENDIMENTO);
    ensure_file(P_PROFISSIONAL, HDR_PROFISSIONAL);
    ensure_file(P_SERVICO, HDR_SERVICO);
}

int storage_usuario_count(void) {
    FILE *f = fopen(P_USUARIO, "r");
    if (!f) return 0;
    char line[4096];
    if (!fgets(line, sizeof line, f)) {
        fclose(f);
        return 0;
    }
    int c = 0;
    while (fgets(line, sizeof line, f)) {
        strip_newline(line);
        if (line[0]) ++c;
    }
    fclose(f);
    return c;
}

int storage_email_exists(const char *email) {
    FILE *f = fopen(P_USUARIO, "r");
    if (!f) return 0;
    char line[4096];
    if (!fgets(line, sizeof line, f)) {
        fclose(f);
        return 0;
    }
    Usuario tmp;
    while (fgets(line, sizeof line, f)) {
        char work[4096];
        if (strlen(line) >= sizeof work) continue;
        strcpy(work, line);
        if (!parse_usuario_line(work, &tmp)) continue;
        if (strcmp(tmp.email, email) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int storage_find_usuario_by_id(int id_usuario, Usuario *u) {
    if (id_usuario <= 0) return 0;
    FILE *f = fopen(P_USUARIO, "r");
    if (!f) return 0;
    char line[4096];
    if (!fgets(line, sizeof line, f)) {
        fclose(f);
        return 0;
    }
    while (fgets(line, sizeof line, f)) {
        char work[4096];
        if (strlen(line) >= sizeof work) continue;
        strcpy(work, line);
        Usuario tmp;
        if (!parse_usuario_line(work, &tmp)) continue;
        if (tmp.id_usuario == id_usuario) {
            *u = tmp;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int storage_find_usuario_by_email(const char *email, Usuario *u) {
    FILE *f = fopen(P_USUARIO, "r");
    if (!f) return 0;
    char line[4096];
    if (!fgets(line, sizeof line, f)) {
        fclose(f);
        return 0;
    }
    while (fgets(line, sizeof line, f)) {
        char work[4096];
        if (strlen(line) >= sizeof work) continue;
        strcpy(work, line);
        Usuario tmp;
        if (!parse_usuario_line(work, &tmp)) continue;
        if (strcmp(tmp.email, email) == 0) {
            *u = tmp;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int storage_refresh_usuario_sessao(Usuario *u) {
    char email_bak[50];
    strncpy(email_bak, u->email, sizeof email_bak - 1);
    email_bak[sizeof email_bak - 1] = '\0';

    if (u->id_usuario > 0) {
        if (storage_find_usuario_by_id(u->id_usuario, u)) return 1;
    }
    if (email_bak[0] != '\0')
        return storage_find_usuario_by_email(email_bak, u);
    return 0;
}

void storage_print_endereco_telefone_relacionados(int id_usuario) {
    if (id_usuario <= 0) return;

    printf("\n--- Registros em endereco (usuario %d) ---\n", id_usuario);
    FILE *f = fopen(P_ENDERECO, "r");
    if (f) {
        char line[4096];
        if (fgets(line, sizeof line, f)) {
            int any = 0;
            while (fgets(line, sizeof line, f)) {
                char work[4096];
                if (strlen(line) >= sizeof work) continue;
                strcpy(work, line);
                char *cols[12];
                int n = split_semicolons(work, cols, 12);
                if (n >= 7 && atoi(cols[6]) == id_usuario) {
                    printf("  id %s | %s, n %s | %s | CEP %s | %s\n", cols[0],
                           cols[1], cols[2], cols[3], cols[4], cols[5]);
                    any = 1;
                }
            }
            if (!any) printf("  (nenhum)\n");
        }
        fclose(f);
    } else
        printf("  (arquivo nao encontrado)\n");

    printf("\n--- Registros em telefone (usuario %d) ---\n", id_usuario);
    f = fopen(P_TELEFONE, "r");
    if (f) {
        char line[4096];
        if (fgets(line, sizeof line, f)) {
            int any = 0;
            while (fgets(line, sizeof line, f)) {
                char work[4096];
                if (strlen(line) >= sizeof work) continue;
                strcpy(work, line);
                char *cols[8];
                int nt = split_semicolons(work, cols, 8);
                if (nt >= 5 && atoi(cols[4]) == id_usuario) {
                    printf("  id %s | %s | %s | cod %s\n", cols[0], cols[1],
                           cols[2], cols[3][0] ? cols[3] : "-");
                    any = 1;
                }
            }
            if (!any) printf("  (nenhum)\n");
        }
        fclose(f);
    } else
        printf("  (arquivo nao encontrado)\n");
}

static void usuario_sanitize_for_write(Usuario *u) {
    csv_sanitize(u->nome);
    csv_sanitize(u->cpf);
    csv_sanitize(u->nascimento);
    csv_sanitize(u->email);
    csv_sanitize(u->senha);
    csv_sanitize(u->telefone);
    csv_sanitize(u->cidade);
    csv_sanitize(u->sangue);
    csv_sanitize(u->alergias);
    csv_sanitize(u->contatoNome1);
    csv_sanitize(u->contatoTelefone1);
    csv_sanitize(u->contatoNome2);
    csv_sanitize(u->contatoTelefone2);
    csv_sanitize(u->mensagemCheckin);
    csv_sanitize(u->senhaCheckin);
    csv_sanitize(u->endereco);
}

static int append_endereco_for_user(int id_usuario, const Usuario *u) {
    FILE *f = fopen(P_ENDERECO, "a");
    if (!f) return 0;
    int id = next_id_in_file(P_ENDERECO);
    char rua[120];
    strncpy(rua, u->endereco, sizeof rua - 1);
    rua[sizeof rua - 1] = '\0';
    csv_sanitize(rua);
    char bairro[60];
    strncpy(bairro, u->cidade, sizeof bairro - 1);
    bairro[sizeof bairro - 1] = '\0';
    csv_sanitize(bairro);
    fprintf(f, "%d;%s;%s;%s;%s;%s;%d\n", id, rua, "", bairro, "", "",
            id_usuario);
    fclose(f);
    return 1;
}

static int append_telefone_row(int id_usuario, const char *tipo,
                               const char *numero) {
    FILE *f = fopen(P_TELEFONE, "a");
    if (!f) return 0;
    int id = next_id_in_file(P_TELEFONE);
    char num[32];
    strncpy(num, numero, sizeof num - 1);
    num[sizeof num - 1] = '\0';
    csv_sanitize(num);
    fprintf(f, "%d;%s;%s;;%d\n", id, tipo, num, id_usuario);
    fclose(f);
    return 1;
}

int storage_insert_usuario(Usuario *u) {
    usuario_sanitize_for_write(u);
    u->id_usuario = next_id_in_file(P_USUARIO);

    FILE *f = fopen(P_USUARIO, "a");
    if (!f) return 0;
    fprintf(f, "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d;%d;%s;%d;%s;%s\n",
            u->id_usuario, u->nome, u->cpf, u->nascimento, u->email, u->senha,
            u->telefone, u->cidade, u->sangue, u->alergias, u->contatoNome1,
            u->contatoTelefone1, u->contatoNome2, u->contatoTelefone2,
            u->contatoCheckin, u->intervaloCheckin, u->mensagemCheckin,
            u->protecaoCheckin, u->senhaCheckin, u->endereco);
    fclose(f);

    append_endereco_for_user(u->id_usuario, u);
    append_telefone_row(u->id_usuario, "pessoal", u->telefone);
    append_telefone_row(u->id_usuario, "parente", u->contatoTelefone1);
    append_telefone_row(u->id_usuario, "parente", u->contatoTelefone2);
    return 1;
}

int storage_update_usuario(const Usuario *u) {
    if (u->id_usuario <= 0) return 0;

    Usuario tmp = *u;
    usuario_sanitize_for_write(&tmp);

    FILE *in = fopen(P_USUARIO, "r");
    if (!in) return 0;
    FILE *out = fopen(P_USUARIO_TMP, "w");
    if (!out) {
        fclose(in);
        return 0;
    }

    char line[4096];
    if (!fgets(line, sizeof line, in)) {
        fclose(in);
        fclose(out);
        remove(P_USUARIO_TMP);
        return 0;
    }
    strip_newline(line);
    fprintf(out, "%s\n", line);

    int ok = 0;
    while (fgets(line, sizeof line, in)) {
        char work[4096];
        if (strlen(line) >= sizeof work) continue;
        strcpy(work, line);
        strip_newline(work);
        int id = first_id_from_line(work);
        if (id == tmp.id_usuario) {
            fprintf(out,
                    "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d;%d;%s;%d;%s;%"
                    "s\n",
                    tmp.id_usuario, tmp.nome, tmp.cpf, tmp.nascimento,
                    tmp.email, tmp.senha, tmp.telefone, tmp.cidade, tmp.sangue,
                    tmp.alergias, tmp.contatoNome1, tmp.contatoTelefone1,
                    tmp.contatoNome2, tmp.contatoTelefone2, tmp.contatoCheckin,
                    tmp.intervaloCheckin, tmp.mensagemCheckin,
                    tmp.protecaoCheckin, tmp.senhaCheckin, tmp.endereco);
            ok = 1;
        } else
            fprintf(out, "%s\n", work);
    }

    fclose(in);
    fclose(out);

    if (!ok) {
        remove(P_USUARIO_TMP);
        return 0;
    }
    remove(P_USUARIO);
    if (rename(P_USUARIO_TMP, P_USUARIO) != 0) return 0;
    return 1;
}

void storage_append_denuncia(int id_usuario, const char *descricao) {
    char d[512];
    strncpy(d, descricao, sizeof d - 1);
    d[sizeof d - 1] = '\0';
    csv_sanitize(d);

    char date[16], hora[16];
    now_date_time(date, sizeof date, hora, sizeof hora);

    FILE *f = fopen(P_DENUNCIA, "a");
    if (!f) return;
    int id = next_id_in_file(P_DENUNCIA);
    fprintf(f, "%d;%s;%s;%s;%d\n", id, d, date, hora, id_usuario);
    fclose(f);
}

void storage_append_emergencia(int id_usuario, const char *local) {
    char loc[120];
    strncpy(loc, local, sizeof loc - 1);
    loc[sizeof loc - 1] = '\0';
    csv_sanitize(loc);

    char hora[16];
    memset(hora, 0, sizeof hora);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if (tm) strftime(hora, sizeof hora, "%H:%M:%S", tm);

    FILE *f = fopen(P_EMERGENCIA, "a");
    if (!f) return;
    int id = next_id_in_file(P_EMERGENCIA);
    fprintf(f, "%d;%s;%s;%d\n", id, loc, hora, id_usuario);
    fclose(f);
}

void storage_append_checkin_virtual(int id_usuario, const char *localizacao, const char *status_seguranca) {
    char loc[120];
    strncpy(loc, localizacao, sizeof loc - 1);
    loc[sizeof loc - 1] = '\0';
    csv_sanitize(loc);
    char st[24];
    strncpy(st, status_seguranca, sizeof st - 1);
    st[sizeof st - 1] = '\0';
    csv_sanitize(st);

    char dt[32];
    now_datetime(dt, sizeof dt);

    FILE *f = fopen(P_CHECKIN, "a");
    if (!f) return;
    int id = next_id_in_file(P_CHECKIN);
    fprintf(f, "%d;%s;%s;%s;%d\n", id, loc, dt, st, id_usuario);
    fclose(f);
}