#ifndef MENU_H
#define MENU_H

#include <windows.h>
#include <libpq-fe.h>
#include <string>


// Declarar MenuProcedure
LRESULT CALLBACK MenuProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// Declarar la función para abrir la ventana del menú
void AbrirVentanaMenu(PGconn* conn, const std::string& usuario);



#endif // MENU_H
