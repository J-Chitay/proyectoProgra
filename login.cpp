
#include "menu.h"
#include "menu2.h"
#include <windows.h>
#include <tchar.h>
#include "conexion.h"
#include <string>
#include <iostream>

using namespace std;

// Prototipo del procedimiento de ventana
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Identificadores de botón
enum BotonID { BTN_LOGIN = 1, BTN_CLEAR, BTN_CLOSE };

HWND hEditUser, hEditPass, hButtonLogin, hButtonClear, hButtonClose, hwnd;
ConexionDB* conexionPtr = nullptr;  // Puntero a la conexión para usarla en la ventana

string TCHARToString(TCHAR* tcharStr) {
#ifdef UNICODE
    wstring wStr(tcharStr);
    return string(wStr.begin(), wStr.end());
#else
    return string(tcharStr);
#endif
}

int main() {
    // Crear instancia de conexión
    ConexionDB conexion;
    conexionPtr = &conexion;
    
    PGconn* conn = conexion.obtenerConexion();
    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(NULL, _T("No se pudo conectar a la base de datos."), _T("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Registrar la clase de ventana
    WNDCLASS wndClass = {};
    wndClass.lpfnWndProc = WindowProcedure;
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.lpszClassName = _T("LoginWindowClass");
    
    if (!RegisterClass(&wndClass)) {
        return 0;
    }

    hwnd = CreateWindowEx(
        0, _T("LoginWindowClass"), _T("Login"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!hwnd) {
        return 0;
    }

    // Centrar la ventana
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    MoveWindow(hwnd, x, y, windowWidth, windowHeight, FALSE);

    // Configuración de la ventana sin botones de minimizar, maximizar y cerrar
    SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SYSMENU);

    // Crear controles
    CreateWindow(_T("STATIC"), _T("Usuario:"), WS_VISIBLE | WS_CHILD, 50, 50, 100, 20, hwnd, NULL, NULL, NULL);
    hEditUser = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 50, 200, 20, hwnd, NULL, NULL, NULL);

    CreateWindow(_T("STATIC"), _T("Contrasenna:"), WS_VISIBLE | WS_CHILD, 50, 100, 100, 20, hwnd, NULL, NULL, NULL);
    hEditPass = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 150, 100, 200, 20, hwnd, NULL, NULL, NULL);

    hButtonLogin = CreateWindow(_T("BUTTON"), _T("Iniciar sesion"), WS_VISIBLE | WS_CHILD, 150, 150, 100, 30, hwnd, (HMENU)BTN_LOGIN, NULL, NULL);
    hButtonClear = CreateWindow(_T("BUTTON"), _T("Limpiar"), WS_VISIBLE | WS_CHILD, 260, 150, 100, 30, hwnd, (HMENU)BTN_CLEAR, NULL, NULL);
    hButtonClose = CreateWindow(_T("BUTTON"), _T("Cerrar"), WS_VISIBLE | WS_CHILD, 370, 150, 100, 30, hwnd, (HMENU)BTN_CLOSE, NULL, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Bucle de mensajes único
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // La conexión se cerrará al salir del objeto conexión
    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_DESTROY:
        // En este punto, la conexión se cierra en el destructor de ConexionDB.
        PostQuitMessage(0);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case BTN_LOGIN: {
            int lenUser = GetWindowTextLength(hEditUser) + 1;
            int lenPass = GetWindowTextLength(hEditPass) + 1;
            TCHAR *username = new TCHAR[lenUser];
            TCHAR *password = new TCHAR[lenPass];

            GetWindowText(hEditUser, username, lenUser);
            GetWindowText(hEditPass, password, lenPass);

            string usuario = TCHARToString(username);
            string contrasena = TCHARToString(password);
            cout << "Usuario: " << usuario << ", Contrasenia: " << contrasena << endl;
            delete[] username;
            delete[] password;

            PGconn* conn = conexionPtr->obtenerConexion();
            if (PQstatus(conn) != CONNECTION_OK) {
                MessageBox(hwnd, _T("No se pudo conectar a la base de datos."), _T("Error"), MB_OK | MB_ICONERROR);
                return 0;
            }

            // Cambiar la consulta para devolver primero el ID:
            string query = "SELECT id, tipo FROM usuarios WHERE usuario = '" + usuario + "' AND contrasena = '" + contrasena + "'";

            PGresult* res = PQexec(conn, query.c_str());

            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                MessageBox(hwnd, _T("Error al ejecutar la consulta."), _T("Error"), MB_OK | MB_ICONERROR);
            } else if (PQntuples(res) == 0) {
                MessageBox(hwnd, _T("Credenciales inválidas."), _T("Error de Login"), MB_OK | MB_ICONERROR);
                SetWindowText(hEditUser, _T("")); SetWindowText(hEditPass, _T("")); SetFocus(hEditUser);
            } else {
                int idUsuario = atoi(PQgetvalue(res, 0, 0)); // ← Aquí obtienes el ID
                string tipo_usuario = PQgetvalue(res, 0, 1);

                if (tipo_usuario == "admin") {
                    DestroyWindow(hwnd);
                    AbrirVentanaMenu(conn, usuario);
                } else if (tipo_usuario == "normal") {
                    DestroyWindow(hwnd);
                    AbrirVentanaMenu2(conn, usuario, idUsuario); // ← Ya puedes usarlo
                }
            }
            PQclear(res);
            break;
        }
        case BTN_CLOSE:
            PostQuitMessage(0);
            break;
        case BTN_CLEAR:
            SetWindowText(hEditUser, _T(""));
            SetWindowText(hEditPass, _T(""));
            SetFocus(hEditUser);
            break;
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}