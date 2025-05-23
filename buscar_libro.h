#ifndef BUSCAR_LIBRO_H
#define BUSCAR_LIBRO_H

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <libpq-fe.h>
#include "conexion.h"  // Asegúrate que define ConexionDB

#define IDC_EDIT_TITULO 101
#define IDC_BTN_BUSCAR 102
#define IDC_LISTVIEW_RESULTADOS 103
#define IDC_BTN_CERRAR 104  // Botón para cerrar ventana

void AbrirVentanaBuscarLibro(PGconn* conn, const std::string& usuario);

#endif
