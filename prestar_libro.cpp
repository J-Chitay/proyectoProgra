#include "prestar_libro.h"
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <sstream>
#include <ctime>
#include "conexion.h"

extern PGconn* conexionGlobal;
extern std::string usuarioActual;
extern int idUsuarioActual;

HWND hwndPrestarLibro;
HWND txtIdLibro;
HWND btnPrestar;



void GuardarPrestamo(PGconn* conn, int isbn, int idUsuario) {
    if (!conn) {
        MessageBox(NULL, "Conexión no válida", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Verificar si el libro existe y si está disponible
    std::stringstream consulta;
    consulta << "SELECT titulo, disponibilidad FROM libros WHERE isbn = " << isbn << ";";

    PGresult* res = PQexec(conn, consulta.str().c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string errorMsg = "Error al consultar el libro:\n" + std::string(PQerrorMessage(conn));
        MessageBox(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
        PQclear(res);
        return;
    }

    int filas = PQntuples(res);
    if (filas == 0) {
        MessageBox(NULL, "El ISBN ingresado no existe.", "Error", MB_OK | MB_ICONERROR);
        PQclear(res);
        return;
    }

    // Obtener título y disponibilidad
    char* titulo = PQgetvalue(res, 0, 0);
    char* disp_str = PQgetvalue(res, 0, 1);
    int disponibilidad = atoi(disp_str);

    if (disponibilidad == 0) {
        // Libro ya está prestado
        MessageBox(NULL, "El libro ya esta prestado y no se puede realizar la operacion", "Información", MB_OK | MB_ICONINFORMATION);
        PQclear(res);
        return;
    }

    PQclear(res);

    // Actualizar disponibilidad a 0
    std::stringstream actualizar;
    actualizar << "UPDATE libros SET disponibilidad = 0 WHERE isbn = " << isbn << ";";

    PGresult* res2 = PQexec(conn, actualizar.str().c_str());

    if (PQresultStatus(res2) != PGRES_COMMAND_OK) {
        std::string errorMsg = "Error al actualizar la disponibilidad:\n" + std::string(PQerrorMessage(conn));
        MessageBox(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
        PQclear(res2);
        return;
    }
    PQclear(res2);

    // Obtener fecha actual
    time_t ahora = time(0);
    struct tm tstruct;
    char fechaHora[80];
    tstruct = *localtime(&ahora);
    strftime(fechaHora, sizeof(fechaHora), "%Y-%m-%d", &tstruct);  // solo fecha

    // Insertar préstamo
    std::stringstream insertar;
    insertar << "INSERT INTO prestamos (isbn, titulo, id_usuario, fecha_prestamo) VALUES ("
             << isbn << ", '" << titulo << "', " << idUsuario << ", '" << fechaHora << "');";

    PGresult* res3 = PQexec(conn, insertar.str().c_str());

    if (PQresultStatus(res3) != PGRES_COMMAND_OK) {
        std::string errorMsg = "Error al registrar el préstamo:\n" + std::string(PQerrorMessage(conn));
        MessageBox(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
        PQclear(res3);
        return;
    }
    PQclear(res3);

    MessageBox(NULL, "El libro ha sido prestado con exito", "Éxito", MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK PrestarLibroProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        if (LOWORD(wp) == 1) { // Botón "Prestar"
            char buffer[256];
            GetWindowTextA(txtIdLibro, buffer, 256);
            int isbn = atoi(buffer);

            if (isbn <= 0) {
                MessageBox(hwnd, "No se encontro el libro con el ISBN ingresado", "Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            GuardarPrestamo(conexionGlobal, isbn, idUsuarioActual);
            SetWindowText(txtIdLibro, "");  // Limpia el campo ISBN
            return 0;
        }
        else if (LOWORD(wp) == 2) {  // Botón "Cerrar"
            DestroyWindow(hwndPrestarLibro);
            hwndPrestarLibro = NULL;
        return 0;
        }
        break;

    case WM_DESTROY:
        DestroyWindow(hwndPrestarLibro);
        hwndPrestarLibro = NULL;
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void CentrarVentana(HWND hwnd) {
    RECT rc = { 0 };
    GetWindowRect(hwnd, &rc);

    int ancho = rc.right - rc.left;
    int alto = rc.bottom - rc.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - ancho) / 2;
    int y = (screenHeight - alto) / 2;

    SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}




void AbrirVentanaPrestarLibro(PGconn* conn, const std::string& usuario) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = PrestarLibroProcedure;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "PrestarLibroWindowClass";

    RegisterClass(&wc);

    hwndPrestarLibro = CreateWindowEx(
        0, "PrestarLibroWindowClass", "Prestar Libro",
        WS_OVERLAPPEDWINDOW,  // Quitar max y min
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!hwndPrestarLibro) {
        MessageBox(NULL, "No se pudo crear la ventana de préstamo", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    CentrarVentana(hwndPrestarLibro);

    CreateWindow("STATIC", "ID del Libro:", WS_VISIBLE | WS_CHILD,
                 50, 40, 100, 20, hwndPrestarLibro, NULL, NULL, NULL);

    txtIdLibro = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                              160, 40, 150, 20, hwndPrestarLibro, NULL, NULL, NULL);

    btnPrestar = CreateWindow("BUTTON", "Prestar", WS_VISIBLE | WS_CHILD,
                              150, 80, 100, 30, hwndPrestarLibro, (HMENU)1, NULL, NULL);

    HWND btnCerrar = CreateWindow("BUTTON", "Cerrar", WS_VISIBLE | WS_CHILD,
                                 150, 120, 100, 30, hwndPrestarLibro, (HMENU)2, NULL, NULL);

    ShowWindow(hwndPrestarLibro, SW_SHOW);
    UpdateWindow(hwndPrestarLibro);
}
