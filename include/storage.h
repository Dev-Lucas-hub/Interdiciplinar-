#pragma once

#include "usuario.h"

void storage_init(void);
int  storage_usuario_count(void);
int  storage_email_exists(const char *email);
int  storage_find_usuario_by_id(int id_usuario, Usuario *u);
int  storage_find_usuario_by_email(const char *email, Usuario *u);
int  storage_refresh_usuario_sessao(Usuario *u);
void storage_print_endereco_telefone_relacionados(int id_usuario);
void storage_imprimir_todos_usuarios(int id_usuario_em_sessao);
int  storage_insert_usuario(Usuario *u);
int  storage_update_usuario(const Usuario *u);
void storage_append_denuncia(int id_usuario, const char *descricao);
void storage_append_emergencia(int id_usuario, const char *local);
void storage_append_checkin_virtual(int id_usuario, const char *localizacao, const char *status_seguranca);
