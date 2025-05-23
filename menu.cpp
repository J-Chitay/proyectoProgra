#include "menu.h"
#include "conexion.h"
#include "agregar_libro.h"
#include <windows.h>
#include <libpq-fe.h>
#include "ver_libros.h"

// Variable global para la conexión (si deseas usarla)
PGconn* conexionGlobal = nullptr;

LRESULT CALLBACK MenuProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
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
        if (LOWORD(wp) == 4) {  // Botón de Agregar Libro
            // Abrir la ventana para agregar libro sin destruir el menú.
            // Si lo deseas, puedes ocultar el menú o deshabilitarlo temporalmente.
            AbrirVentanaAgregarLibro(hwnd);
            return 0;
        }
        if (LOWORD(wp) == 5) {  // Botón de Ver Libros
            AbrirVentanaVerLibros(hwnd);
            return 0;
        }

        
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void AbrirVentanaMenu(PGconn* conn, const std::string& usuario)
{
    // Asigna la conexión a la variable global para su uso posterior.
    conexionGlobal = conn;
    
    WNDCLASS wndClass = {};
    wndClass.lpfnWndProc = MenuProcedure;
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.lpszClassName = "MenuWindowClass";
    
    if (!RegisterClass(&wndClass)) {
        // Si la clase ya está registrada, puedes omitir este error.
    }

    HWND hwnd = CreateWindowEx(
        0, "MenuWindowClass", "Menú de Admin",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!hwnd) {
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
    
    // Crear los botones.
    CreateWindow("BUTTON", "Ver Libros", WS_VISIBLE | WS_CHILD, 150, 100, 150, 30, hwnd, (HMENU)5, NULL, NULL);
    CreateWindow("BUTTON", "Agregar Libro", WS_VISIBLE | WS_CHILD, 150, 150, 150, 30, hwnd, (HMENU)4, NULL, NULL);
    CreateWindow("BUTTON", "Cerrar", WS_VISIBLE | WS_CHILD, 200, 200, 100, 30, hwnd, (HMENU)3, NULL, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
