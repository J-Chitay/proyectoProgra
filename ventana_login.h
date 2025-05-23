#ifndef VENTANALOGIN_H
#define VENTANALOGIN_H

#include <windows.h>

class VentanaLogin {
public:
    VentanaLogin();
    void CrearVentana();
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK WindowProcedureMenu(HWND, UINT, WPARAM, LPARAM);

private:
    HWND hButtonLogin, hButtonClear;
    void AbrirVentanaMenu();
    void AbrirVentanaAgregarLibro();
};

#endif // VENTANALOGIN_H
