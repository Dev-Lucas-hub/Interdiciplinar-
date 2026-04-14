#ifndef DB_H
#define DB_H

#include "viva_segura.h"

int db_init(void);

int db_insert_usuario(const struct Usuario *u);

int db_get_usuario_by_email(const char *email, struct Usuario *out);

int db_log_emergencia(const struct Usuario *u);
int db_log_monitoramento(const struct Usuario *u);
int db_log_checkin(const struct Usuario *u, int acao);
int db_insert_denuncia(const struct Usuario *u_or_null, const char *relato, int anonimo);

#endif