#ifndef PRESTAR_LIBRO_H
#define PRESTAR_LIBRO_H

#include <libpq-fe.h>
#include <string>

void AbrirVentanaPrestarLibro(PGconn* conn, const std::string& usuario);

#endif
