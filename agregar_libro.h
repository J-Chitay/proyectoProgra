#ifndef AGREGAR_LIBRO_H
#define AGREGAR_LIBRO_H

#include <windows.h>
#include "conexion.h"

void agregarLibro(PGconn* conn, const char* titulo, const char* autor, const char* isbn, const char* editorial, const char* anio, const char* area,const char* disponibilidad);
void AbrirVentanaAgregarLibro(HWND hwndPadre);

#endif // AGREGAR_LIBRO_H
