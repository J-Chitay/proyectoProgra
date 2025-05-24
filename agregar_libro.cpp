#include <windows.h>
#include <tchar.h>
#include <string>
#include <libpq-fe.h>
#include "conexion.h"
#include "agregar_libro.h"

using namespace std;

#define ID_BTN_GUARDAR 101
#define ID_BTN_LIMPIAR 102
#define ID_BTN_CERRAR  103

HWND hTitulo, hAutor, hISBN, hEditorial, hAnio, hArea, hDisponibilidad;
PGconn* connAgregar = nullptr;
ConexionDB* conexionLibro = nullptr;

void limpiarCampos() {
    SetWindowTextA(hTitulo, "");
    SetWindowTextA(hAutor, "");
    SetWindowTextA(hISBN, "");
    SetWindowTextA(hEditorial, "");
    SetWindowTextA(hAnio, "");
    SetWindowTextA(hArea, "");
    SetWindowTextA(hDisponibilidad, "");
}

void agregarLibro(PGconn* conn, const char* titulo, const char* autor, const char* isbn, const char* editorial, const char* anio, const char* area) {
    if (!conn) {
        MessageBox(NULL, _T("No se pudo conectar a la base de datos."), _T("Error"), MB_ICONERROR);
        return;
    }

    if (strlen(titulo) == 0 || strlen(autor) == 0 || strlen(isbn) == 0 ||
        strlen(editorial) == 0 || strlen(anio) == 0 || strlen(area) == 0) {
        MessageBox(NULL, _T("campos requeridos favor completar la información"), _T("Campos vacíos"), MB_ICONWARNING);
        return;
    }

    const char* checkQuery = "SELECT COUNT(*) FROM libros WHERE isbn = $1";
    const char* isbnParam[1] = { isbn };
    PGresult* checkRes = PQexecParams(conn, checkQuery, 1, NULL, isbnParam, NULL, NULL, 0);

    if (PQresultStatus(checkRes) != PGRES_TUPLES_OK) {
        MessageBox(NULL, _T(PQerrorMessage(conn)), _T("Error al verificar ISBN"), MB_ICONERROR);
        PQclear(checkRes);
        return;
    }

    int count = atoi(PQgetvalue(checkRes, 0, 0));
    PQclear(checkRes);

    if (count > 0) {
        MessageBox(NULL, _T("ISBN ya esta registrado favor verificar"), _T("Duplicado"), MB_ICONWARNING);
        return;
    }

    const char* disponibilidad = "1";
    const char* paramValues[7] = { titulo, autor, isbn, editorial, anio, area, disponibilidad };
    const char* insertQuery = "INSERT INTO libros (titulo, autor, isbn, editorial, anioPublicacion, areaCientifica, disponibilidad) VALUES ($1, $2, $3, $4, $5, $6, $7)";
    
    PGresult* insertRes = PQexecParams(conn, insertQuery, 7, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(insertRes) != PGRES_COMMAND_OK) {
        MessageBox(NULL, _T(PQerrorMessage(conn)), _T("Error al insertar libro"), MB_ICONERROR);
    } else {
        MessageBox(NULL, _T("El libro ha sido agregado exitosamente"), _T("Éxito"), MB_OK | MB_ICONINFORMATION);
        limpiarCampos();
    }

    PQclear(insertRes);
}

LRESULT CALLBACK VentanaAgregarLibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    const int labelX = 20, inputX = 120, labelW = 80, inputW = 300;
    int y = 20, stepY = 30;

    switch (msg) {
    case WM_CREATE:
        conexionLibro = new ConexionDB();
        connAgregar = conexionLibro->obtenerConexion();

        CreateWindow(_T("STATIC"), _T("Titulo:"), WS_VISIBLE | WS_CHILD, labelX, y, labelW, 20, hwnd, NULL, NULL, NULL);
        hTitulo = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, inputX, y, inputW, 20, hwnd, NULL, NULL, NULL);
        y += stepY;

        CreateWindow(_T("STATIC"), _T("Autor:"), WS_VISIBLE | WS_CHILD, labelX, y, labelW, 20, hwnd, NULL, NULL, NULL);
        hAutor = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, inputX, y, inputW, 20, hwnd, NULL, NULL, NULL);
        y += stepY;

        CreateWindow(_T("STATIC"), _T("ISBN:"), WS_VISIBLE | WS_CHILD, labelX, y, labelW, 20, hwnd, NULL, NULL, NULL);
        hISBN = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, inputX, y, inputW, 20, hwnd, NULL, NULL, NULL);
        y += stepY;

        CreateWindow(_T("STATIC"), _T("Editorial:"), WS_VISIBLE | WS_CHILD, labelX, y, labelW, 20, hwnd, NULL, NULL, NULL);
        hEditorial = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, inputX, y, inputW, 20, hwnd, NULL, NULL, NULL);
        y += stepY;

        CreateWindow(_T("STATIC"), _T("Anio:"), WS_VISIBLE | WS_CHILD, labelX, y, labelW, 20, hwnd, NULL, NULL, NULL);
        hAnio = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, inputX, y, inputW, 20, hwnd, NULL, NULL, NULL);
        y += stepY;

        CreateWindow(_T("STATIC"), _T("Area:"), WS_VISIBLE | WS_CHILD, labelX, y, labelW, 20, hwnd, NULL, NULL, NULL);
        hArea = CreateWindow(_T("EDIT"), _T(""), WS_VISIBLE | WS_CHILD | WS_BORDER, inputX, y, inputW, 20, hwnd, NULL, NULL, NULL);
        y += stepY;

        y += stepY + 10; // Saltar espacio como si existiera el campo

        CreateWindow(_T("BUTTON"), _T("Guardar"), WS_VISIBLE | WS_CHILD, inputX, y, 90, 30, hwnd, (HMENU)ID_BTN_GUARDAR, NULL, NULL);
        CreateWindow(_T("BUTTON"), _T("Limpiar"), WS_VISIBLE | WS_CHILD, inputX + 100, y, 90, 30, hwnd, (HMENU)ID_BTN_LIMPIAR, NULL, NULL);
        CreateWindow(_T("BUTTON"), _T("Cerrar"), WS_VISIBLE | WS_CHILD, inputX + 200, y, 90, 30, hwnd, (HMENU)ID_BTN_CERRAR, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_GUARDAR: {
            char titulo[255], autor[255], isbn[100], editorial[255], anio[10], area[100];
            GetWindowTextA(hTitulo, titulo, 255);
            GetWindowTextA(hAutor, autor, 255);
            GetWindowTextA(hISBN, isbn, 100);
            GetWindowTextA(hEditorial, editorial, 255);
            GetWindowTextA(hAnio, anio, 10);
            GetWindowTextA(hArea, area, 100);
            agregarLibro(connAgregar, titulo, autor, isbn, editorial, anio, area);
            break;
        }
        case ID_BTN_LIMPIAR:
            limpiarCampos();
            break;
        case ID_BTN_CERRAR:
            DestroyWindow(hwnd);
            break;
        }
        break;

    case WM_DESTROY:
        if (conexionLibro) {
            delete conexionLibro;
            conexionLibro = nullptr;
            connAgregar = nullptr;
        }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void AbrirVentanaAgregarLibro(HWND hwndPadre) {
    WNDCLASS wc = { };
    wc.lpfnWndProc = VentanaAgregarLibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = _T("VentanaAgregarLibro");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Tamaño calculado justo para los controles + márgenes
    int width = 480;
    int height = 340;

    // Centrar ventana
    int screenX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int screenY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    HWND hwnd = CreateWindow(_T("VentanaAgregarLibro"), _T("Agregar Libro"),
        WS_VISIBLE | WS_SYSMENU,  // Ventana no redimensionable
        screenX, screenY, width, height,
        hwndPadre, NULL, GetModuleHandle(NULL), NULL);

    if (!hwnd) {
        MessageBox(hwndPadre, _T("Error al crear la ventana Agregar Libro"), _T("Error"), MB_ICONERROR);
    }

    ShowWindow(hwnd, SW_SHOW);
}
