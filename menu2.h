#ifndef MENU2_H
#define MENU2_H

#include <windows.h>
#include <libpq-fe.h>
#include <string>

// Variable global para el usuario
extern std::string usuarioActual;
extern int idUsuarioActual;



// Declarar MenuProcedure2
LRESULT CALLBACK MenuProcedure2(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// Declarar la función para abrir la ventana del menú
void AbrirVentanaMenu2(PGconn* conn, const std::string& usuario, int idUsuario);



#endif // MENU2_H
