#include <windows.h>
#include <commctrl.h>
#include <string>
#include <libpq-fe.h>
#include "menu2.h" // Para acceder a idUsuarioActual

#pragma comment(lib, "comctl32.lib")

// IDs para controles
#define ID_TEXTBOX_ISBN 101
#define ID_BUTTON_DEVOLVER 102
#define ID_LISTVIEW_PRESTAMOS 103
#define ID_BUTTON_CERRAR 104

// Conexión global
static PGconn* conexion = nullptr;
static HWND hListView = nullptr;

// Prototipo
void DevolverLibro(const std::string& isbn);
void CargarPrestamos();

// Procedimiento de la ventana
LRESULT CALLBACK DevolverLibroProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        InitCommonControls();

        CreateWindow("STATIC", "Ingrese ISBN del libro a devolver:", WS_VISIBLE | WS_CHILD, 20, 20, 220, 20, hwnd, NULL, NULL, NULL);

        CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 50, 200, 25, hwnd, (HMENU)ID_TEXTBOX_ISBN, NULL, NULL);

        CreateWindow("BUTTON", "Devolver Libro", WS_VISIBLE | WS_CHILD, 230, 50, 130, 30, hwnd, (HMENU)ID_BUTTON_DEVOLVER, NULL, NULL);

        CreateWindow("BUTTON", "Cerrar", WS_VISIBLE | WS_CHILD, 230, 90, 130, 30, hwnd, (HMENU)ID_BUTTON_CERRAR, NULL, NULL);

        hListView = CreateWindow(WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT, 20, 130, 340, 200, hwnd, (HMENU)ID_LISTVIEW_PRESTAMOS, NULL, NULL);

        // Configurar columnas
        LVCOLUMN col = { 0 };
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        static char colIsbn[] = "ISBN";
        col.pszText = colIsbn;
        col.cx = 80;
        col.iSubItem = 0;
        ListView_InsertColumn(hListView, 0, &col);

        static char colTitulo[] = "Titulo";
        col.pszText = colTitulo;
        col.cx = 130;
        col.iSubItem = 1;
        ListView_InsertColumn(hListView, 1, &col);

        static char colFecha[] = "Fecha prestamo";
        col.pszText = colFecha;
        col.cx = 120;
        col.iSubItem = 2;
        ListView_InsertColumn(hListView, 2, &col);

        CargarPrestamos();
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wp) == ID_BUTTON_DEVOLVER) {
            char isbn[100] = { 0 };
            GetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_ISBN), isbn, sizeof(isbn));
            DevolverLibro(std::string(isbn));
            CargarPrestamos(); // Refrescar lista
        } else if (LOWORD(wp) == ID_BUTTON_CERRAR) {
            DestroyWindow(hwnd);
        }
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

// Función lógica de devolución
void DevolverLibro(const std::string& isbn) {
    if (isbn.empty()) {
        MessageBox(NULL, "Por favor ingrese un ISBN.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string query = "SELECT disponibilidad FROM libros WHERE isbn = " + isbn + ";";
    PGresult* res = PQexec(conexion, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        MessageBox(NULL, "No se encontró un libro con ese ISBN o error al consultar.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    char* disponibilidad = PQgetvalue(res, 0, 0);
    if (strcmp(disponibilidad, "1") == 0) {
        PQclear(res);
        MessageBox(NULL, "El libro ya ha sido devuelto.", "Aviso", MB_OK | MB_ICONINFORMATION);
        return;
    }

    PQclear(res);

    std::string updateQuery = "UPDATE libros SET disponibilidad = 1 WHERE isbn = " + isbn + ";";
    PGresult* resUpdate = PQexec(conexion, updateQuery.c_str());
    PQclear(resUpdate);

    std::string deleteQuery = "DELETE FROM prestamos WHERE isbn = " + isbn +
        " AND id_usuario = " + std::to_string(idUsuarioActual) + ";";
    PGresult* resDelete = PQexec(conexion, deleteQuery.c_str());
    PQclear(resDelete);

    MessageBox(NULL, "Libro devuelto correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
}

// Carga los préstamos del usuario actual
void CargarPrestamos() {
    ListView_DeleteAllItems(hListView);

    std::string query = "SELECT isbn, titulo, fecha_prestamo FROM prestamos WHERE id_usuario = " + std::to_string(idUsuarioActual) + ";";
    PGresult* res = PQexec(conexion, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        MessageBox(NULL, "Error al consultar préstamos.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = PQgetvalue(res, i, 0); // isbn
        ListView_InsertItem(hListView, &item);

        ListView_SetItemText(hListView, i, 1, PQgetvalue(res, i, 1)); // titulo
        ListView_SetItemText(hListView, i, 2, PQgetvalue(res, i, 2)); // fecha
    }

    PQclear(res);
}

// Abre la ventana de devolución
void AbrirVentanaDevolverLibro(PGconn* conn) {
    conexion = conn;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = DevolverLibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "DevolverLibroWindowClass";

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Error al registrar clase ventana.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    int width = 400;
    int height = 400;

    // Obtener tamaño de pantalla
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    HWND hwnd = CreateWindowEx(
        0,
        "DevolverLibroWindowClass",
        "Devolver Libro",
        WS_POPUPWINDOW | WS_CAPTION, // Sin minimizar, maximizar ni cerrar
        x, y, width, height,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!hwnd) {
        MessageBox(NULL, "Error al crear ventana.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

