#include "db.h"
#include "sqlite3.h"

#define DB_PATH "viva_segura.db"

static int exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        if (errmsg) sqlite3_free(errmsg);
        return rc;
    }
    return 0;
}

int db_init(void) {
    sqlite3 *db = NULL;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return rc;
    }

    rc = exec_sql(db,
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nome TEXT NOT NULL,"
        "  nascimento TEXT,"
        "  cpf TEXT,"
        "  email TEXT NOT NULL UNIQUE,"
        "  senha TEXT NOT NULL,"
        "  telefone TEXT,"
        "  endereco TEXT,"
        "  cidade TEXT,"
        "  estado TEXT,"
        "  cep TEXT,"
        "  sangue TEXT,"
        "  alergias TEXT,"
        "  contatoNome1 TEXT,"
        "  contatoTelefone1 TEXT,"
        "  contatoNome2 TEXT,"
        "  contatoTelefone2 TEXT,"
        "  protecaoCheckin INTEGER NOT NULL DEFAULT 0 CHECK (protecaoCheckin IN (0,1)),"
        "  senhaCheckin TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS emergencias ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  usuario_email TEXT REFERENCES usuarios(email) ON DELETE SET NULL,"
        "  usuario_nome TEXT,"
        "  usuario_cpf TEXT,"
        "  cidade TEXT,"
        "  estado TEXT,"
        "  endereco TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS monitoramentos ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  usuario_email TEXT REFERENCES usuarios(email) ON DELETE SET NULL,"
        "  usuario_nome TEXT,"
        "  cidade TEXT,"
        "  estado TEXT,"
        "  endereco TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS checkins ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  usuario_email TEXT REFERENCES usuarios(email) ON DELETE SET NULL,"
        "  usuario_nome TEXT,"
        "  acao INTEGER NOT NULL,"
        "  cep TEXT"
        ");"
        "CREATE TABLE IF NOT EXISTS denuncias ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  anonimo INTEGER NOT NULL CHECK (anonimo IN (0,1)),"
        "  autor_email TEXT REFERENCES usuarios(email) ON DELETE SET NULL,"
        "  autor_nome TEXT,"
        "  relato TEXT NOT NULL"
        ");"
        "CREATE UNIQUE INDEX IF NOT EXISTS idx_usuarios_cpf ON usuarios(cpf);"
        "CREATE INDEX IF NOT EXISTS idx_emergencias_created_at ON emergencias(created_at);"
        "CREATE INDEX IF NOT EXISTS idx_monitoramentos_created_at ON monitoramentos(created_at);"
        "CREATE INDEX IF NOT EXISTS idx_checkins_created_at ON checkins(created_at);"
        "CREATE INDEX IF NOT EXISTS idx_denuncias_created_at ON denuncias(created_at);"
    );

    sqlite3_close(db);
    return rc;
}

int db_insert_usuario(const struct Usuario *u) {
    sqlite3 *db = NULL;
    sqlite3_stmt *st = NULL;

    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) { if (db) sqlite3_close(db); return rc; }

    const char *sql =
        "INSERT INTO usuarios ("
        " nome, nascimento, cpf, email, senha, telefone, endereco, cidade, estado, cep, sangue, alergias,"
        " contatoNome1, contatoTelefone1, contatoNome2, contatoTelefone2, protecaoCheckin, senhaCheckin"
        ") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
    if (rc != SQLITE_OK) { sqlite3_close(db); return rc; }

    sqlite3_bind_text(st,  1, u->nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  2, u->nascimento, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  3, u->cpf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  4, u->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  5, u->senha, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  6, u->telefone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  7, u->endereco, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  8, u->cidade, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st,  9, u->estado, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 10, u->cep, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 11, u->sangue, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 12, u->alergias, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 13, u->contatoNome1, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 14, u->contatoTelefone1, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 15, u->contatoNome2, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 16, u->contatoTelefone2, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (st, 17, u->protecaoCheckin);
    sqlite3_bind_text(st, 18, u->senhaCheckin, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    sqlite3_finalize(st);
    sqlite3_close(db);

    if (rc == SQLITE_DONE) return 0;
    if (rc == SQLITE_CONSTRAINT || rc == SQLITE_CONSTRAINT_UNIQUE) return 1;
    return rc;
}

static void safe_copy(char *dst, size_t dst_sz, const unsigned char *src) {
    if (!dst || dst_sz == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    strncpy(dst, (const char*)src, dst_sz - 1);
    dst[dst_sz - 1] = '\0';
}

int db_get_usuario_by_email(const char *email, struct Usuario *out) {
    if (!email || !out) return SQLITE_MISUSE;

    sqlite3 *db = NULL;
    sqlite3_stmt *st = NULL;

    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) { if (db) sqlite3_close(db); return rc; }

    const char *sql =
        "SELECT "
        " nome, nascimento, cpf, email, senha, telefone, endereco, cidade, estado, cep, sangue, alergias,"
        " contatoNome1, contatoTelefone1, contatoNome2, contatoTelefone2, protecaoCheckin, senhaCheckin "
        "FROM usuarios WHERE email = ? LIMIT 1;";

    rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
    if (rc != SQLITE_OK) { sqlite3_close(db); return rc; }

    sqlite3_bind_text(st, 1, email, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    if (rc == SQLITE_ROW) {
        memset(out, 0, sizeof(*out));

        safe_copy(out->nome, sizeof(out->nome), sqlite3_column_text(st, 0));
        safe_copy(out->nascimento, sizeof(out->nascimento), sqlite3_column_text(st, 1));
        safe_copy(out->cpf, sizeof(out->cpf), sqlite3_column_text(st, 2));
        safe_copy(out->email, sizeof(out->email), sqlite3_column_text(st, 3));
        safe_copy(out->senha, sizeof(out->senha), sqlite3_column_text(st, 4));
        safe_copy(out->telefone, sizeof(out->telefone), sqlite3_column_text(st, 5));
        safe_copy(out->endereco, sizeof(out->endereco), sqlite3_column_text(st, 6));
        safe_copy(out->cidade, sizeof(out->cidade), sqlite3_column_text(st, 7));
        safe_copy(out->estado, sizeof(out->estado), sqlite3_column_text(st, 8));
        safe_copy(out->cep, sizeof(out->cep), sqlite3_column_text(st, 9));
        safe_copy(out->sangue, sizeof(out->sangue), sqlite3_column_text(st, 10));
        safe_copy(out->alergias, sizeof(out->alergias), sqlite3_column_text(st, 11));
        safe_copy(out->contatoNome1, sizeof(out->contatoNome1), sqlite3_column_text(st, 12));
        safe_copy(out->contatoTelefone1, sizeof(out->contatoTelefone1), sqlite3_column_text(st, 13));
        safe_copy(out->contatoNome2, sizeof(out->contatoNome2), sqlite3_column_text(st, 14));
        safe_copy(out->contatoTelefone2, sizeof(out->contatoTelefone2), sqlite3_column_text(st, 15));
        out->protecaoCheckin = sqlite3_column_int(st, 16);
        safe_copy(out->senhaCheckin, sizeof(out->senhaCheckin), sqlite3_column_text(st, 17));

        sqlite3_finalize(st);
        sqlite3_close(db);
        return 0;
    }

    sqlite3_finalize(st);
    sqlite3_close(db);
    if (rc == SQLITE_DONE) return 1; // não achou
    return rc;
}

static int open_db(sqlite3 **out_db) {
    if (!out_db) return SQLITE_MISUSE;
    *out_db = NULL;
    int rc = sqlite3_open(DB_PATH, out_db);
    if (rc != SQLITE_OK) {
        if (*out_db) sqlite3_close(*out_db);
        *out_db = NULL;
    }
    return rc;
}

int db_log_emergencia(const struct Usuario *u) {
    if (!u) return SQLITE_MISUSE;
    sqlite3 *db = NULL;
    sqlite3_stmt *st = NULL;

    int rc = open_db(&db);
    if (rc != SQLITE_OK) return rc;

    const char *sql =
        "INSERT INTO emergencias (usuario_email, usuario_nome, usuario_cpf, cidade, estado, endereco) "
        "VALUES (?,?,?,?,?,?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
    if (rc != SQLITE_OK) { sqlite3_close(db); return rc; }

    sqlite3_bind_text(st, 1, u->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, u->nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, u->cpf, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, u->cidade, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, u->estado, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 6, u->endereco, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    sqlite3_finalize(st);
    sqlite3_close(db);
    return (rc == SQLITE_DONE) ? 0 : rc;
}

int db_log_monitoramento(const struct Usuario *u) {
    if (!u) return SQLITE_MISUSE;
    sqlite3 *db = NULL;
    sqlite3_stmt *st = NULL;

    int rc = open_db(&db);
    if (rc != SQLITE_OK) return rc;

    const char *sql =
        "INSERT INTO monitoramentos (usuario_email, usuario_nome, cidade, estado, endereco) "
        "VALUES (?,?,?,?,?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
    if (rc != SQLITE_OK) { sqlite3_close(db); return rc; }

    sqlite3_bind_text(st, 1, u->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, u->nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, u->cidade, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, u->estado, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, u->endereco, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    sqlite3_finalize(st);
    sqlite3_close(db);
    return (rc == SQLITE_DONE) ? 0 : rc;
}

int db_log_checkin(const struct Usuario *u, int acao) {
    if (!u) return SQLITE_MISUSE;
    sqlite3 *db = NULL;
    sqlite3_stmt *st = NULL;

    int rc = open_db(&db);
    if (rc != SQLITE_OK) return rc;

    const char *sql =
        "INSERT INTO checkins (usuario_email, usuario_nome, acao, cep) VALUES (?,?,?,?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
    if (rc != SQLITE_OK) { sqlite3_close(db); return rc; }

    sqlite3_bind_text(st, 1, u->email, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, u->nome, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (st, 3, acao);
    sqlite3_bind_text(st, 4, u->cep, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    sqlite3_finalize(st);
    sqlite3_close(db);
    return (rc == SQLITE_DONE) ? 0 : rc;
}

int db_insert_denuncia(const struct Usuario *u_or_null, const char *relato, int anonimo) {
    if (!relato || relato[0] == '\0') return SQLITE_MISUSE;
    sqlite3 *db = NULL;
    sqlite3_stmt *st = NULL;

    int rc = open_db(&db);
    if (rc != SQLITE_OK) return rc;

    const char *sql =
        "INSERT INTO denuncias (anonimo, autor_email, autor_nome, relato) VALUES (?,?,?,?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &st, NULL);
    if (rc != SQLITE_OK) { sqlite3_close(db); return rc; }

    sqlite3_bind_int(st, 1, anonimo ? 1 : 0);
    if (anonimo || !u_or_null) {
        sqlite3_bind_null(st, 2);
        sqlite3_bind_null(st, 3);
    } else {
        sqlite3_bind_text(st, 2, u_or_null->email, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(st, 3, u_or_null->nome, -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(st, 4, relato, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(st);
    sqlite3_finalize(st);
    sqlite3_close(db);
    return (rc == SQLITE_DONE) ? 0 : rc;
}