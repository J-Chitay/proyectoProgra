#include "menu2.h"
#include "prestar_libro.h"
#include "conexion.h"
#include "buscar_libro.h"
#include <windows.h>
#include <libpq-fe.h>

// Variable global para la conexión (si deseas usarla)

// Declaración para usar la variable global definida en otro archivo
extern PGconn* conexionGlobal;
std::string usuarioActual = "";  // Aquí guardamos el usuario que inició sesión
int idUsuarioActual = -1;  // Aquí guardamos el ID del usuario que inició sesión


LRESULT CALLBACK MenuProcedure2(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Preparar texto con el usuario actual
        std::string textoUsuario = "Usuario: " + usuarioActual + " (ID: " + std::to_string(idUsuarioActual) + ")";


        // Obtener el tamaño de la ventana para posicionar texto a la derecha
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Medir tamaño del texto para alinearlo a la derecha
        SIZE textSize;
        GetTextExtentPoint32A(hdc, textoUsuario.c_str(), textoUsuario.length(), &textSize);

        // Posicionar texto en la esquina superior derecha, con un margen de 10 px
        int x = rect.right - textSize.cx - 10;
        int y = 10;

        // Dibujar el texto
        TextOutA(hdc, x, y, textoUsuario.c_str(), textoUsuario.length());

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        if (conexionGlobal != nullptr) {
            PQfinish(conexionGlobal);
            conexionGlobal = nullptr;
        }
        PostQuitMessage(0);
        return 0;

    case WM_COMMAND:
        if (LOWORD(wp) == 3) {  // Botón de Cerrar
            if (conexionGlobal != nullptr) {
                PQfinish(conexionGlobal);
                conexionGlobal = nullptr;
            }
            PostQuitMessage(0);
            return 0;
        }
        if (LOWORD(wp) == 4) {
            AbrirVentanaBuscarLibro(conexionGlobal, usuarioActual); // Pasa el usuario real
            return 0;
        }
        if (LOWORD(wp) == 5) {
            AbrirVentanaPrestarLibro(conexionGlobal, usuarioActual);
            return 0;
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void AbrirVentanaMenu2(PGconn* conn, const std::string& usuario, int idUsuario)
{
    conexionGlobal = conn;
    usuarioActual = usuario;
    idUsuarioActual = idUsuario;

    // Registrar clase
    WNDCLASS wc = {};
    wc.lpfnWndProc = MenuProcedure2;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "MenuWindowClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Error al registrar clase MenuWindowClass", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    HWND hwnd = CreateWindowEx(
        0, "MenuWindowClass", "Menú de Usuario",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, GetModuleHandle(NULL), NULL);


    if (!hwnd) {
        MessageBox(NULL, "Error al crear ventana Menu2", "Error", MB_OK | MB_ICONERROR);
        return;
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

    SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SYSMENU);

    CreateWindow("BUTTON", "Prestar Libro", WS_VISIBLE | WS_CHILD, 150, 100, 150, 30, hwnd, (HMENU)5, NULL, NULL);
    CreateWindow("BUTTON", "Buscar Libro", WS_VISIBLE | WS_CHILD, 150, 150, 150, 30, hwnd, (HMENU)4, NULL, NULL);
    CreateWindow("BUTTON", "Cerrar", WS_VISIBLE | WS_CHILD, 200, 200, 100, 30, hwnd, (HMENU)3, NULL, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
