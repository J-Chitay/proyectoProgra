#include "buscar_libro.h"
#include <vector>
#include <string>
#include <sstream>
#include "conexion.h"

using namespace std;

ConexionDB* conexionBuscar = nullptr;
PGconn* connBuscar = nullptr;

HWND hwndEditTitulo, hwndBtnBuscar, hwndListView, hwndCerrar;

LRESULT CALLBACK WindowProcedureBuscarLibro(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void AbrirVentanaBuscarLibro(PGconn* conn, const std::string& usuario) {
    conexionBuscar = new ConexionDB();
    connBuscar = conexionBuscar->obtenerConexion();  // abre la conexión al abrir la ventana

    if (PQstatus(connBuscar) != CONNECTION_OK) {
        MessageBox(NULL, "Error al conectar a la base de datos.", "Error", MB_OK | MB_ICONERROR);
        delete conexionBuscar;
        conexionBuscar = nullptr;
        return;
    }

    WNDCLASS wc = {};
    wc.lpszClassName = "BuscarLibroClass";
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProcedureBuscarLibro;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "BuscarLibroClass", "Buscar Libro",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 400,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    // Quitar botones
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_MINIMIZEBOX;
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SYSMENU;
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    // Centrar ventana
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    MoveWindow(hwnd, x, y, windowWidth, windowHeight, FALSE);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

}

void BuscarLibroPorTitulo(HWND hwndListView, const string& titulo) {
    ListView_DeleteAllItems(hwndListView);

    string consulta = "SELECT titulo, autor, isbn, editorial, aniopublicacion, areacientifica, disponibilidad "
                      "FROM libros WHERE titulo ILIKE '%" + titulo + "%'";

    PGresult* resultado = PQexec(connBuscar, consulta.c_str());

    if (PQresultStatus(resultado) != PGRES_TUPLES_OK) {
        MessageBox(NULL, "Error al buscar libros", "Error", MB_OK | MB_ICONERROR);
        PQclear(resultado);
        return;
    }

    int filas = PQntuples(resultado);
    for (int i = 0; i < filas; ++i) {
        string datos[7];
        for (int j = 0; j < 7; ++j) {
            const char* val = PQgetvalue(resultado, i, j);
            datos[j] = val ? val : "";
        }

        datos[6] = (datos[6] == "1") ? "Si" : "No";

        LVITEM item = {};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = const_cast<char*>(datos[0].c_str());
        ListView_InsertItem(hwndListView, &item);

        for (int j = 1; j < 7; ++j) {
            ListView_SetItemText(hwndListView, i, j, const_cast<char*>(datos[j].c_str()));
        }
    }

    PQclear(resultado);
}

LRESULT CALLBACK WindowProcedureBuscarLibro(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        InitCommonControls();

        CreateWindow("STATIC", "Titulo del libro:",
            WS_VISIBLE | WS_CHILD,
            20, 20, 120, 20,
            hwnd, NULL, NULL, NULL);

        hwndEditTitulo = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            150, 18, 300, 25,
            hwnd, (HMENU)IDC_EDIT_TITULO, NULL, NULL);

        hwndBtnBuscar = CreateWindow("BUTTON", "Buscar",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            470, 18, 80, 25,
            hwnd, (HMENU)IDC_BTN_BUSCAR, NULL, NULL);

        hwndCerrar = CreateWindow("BUTTON", "Cerrar",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            570, 320, 90, 30,
            hwnd, (HMENU)IDC_BTN_CERRAR, NULL, NULL);

        hwndListView = CreateWindowEx(0, WC_LISTVIEW, "",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            20, 60, 640, 250,
            hwnd, (HMENU)IDC_LISTVIEW_RESULTADOS, NULL, NULL);

        ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_FULLROWSELECT);

        const char* headers[] = { "Titulo", "Autor", "ISBN", "Editorial", "Anio", "Area", "Disponibilidad" };
        int colWidths[] = { 150, 120, 100, 100, 50, 70, 80 };

        LVCOLUMN col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        for (int i = 0; i < 7; ++i) {
            col.pszText = const_cast<char*>(headers[i]);
            col.cx = colWidths[i];
            col.iSubItem = i;
            ListView_InsertColumn(hwndListView, i, &col);
        }

        BuscarLibroPorTitulo(hwndListView, "");

        break;
    }

    case WM_COMMAND:
        if (LOWORD(wp) == IDC_BTN_BUSCAR) {
            char titulo[256];
            GetWindowText(hwndEditTitulo, titulo, sizeof(titulo));
            string tituloStr = titulo;

            if (tituloStr.empty()) {
                MessageBox(hwnd, "Ingrese el titulo del libro.", "Aviso", MB_OK | MB_ICONWARNING);
            } else {
                BuscarLibroPorTitulo(hwndListView, tituloStr);
            }
        } else if (LOWORD(wp) == IDC_BTN_CERRAR) {
            DestroyWindow(hwnd);  // Esto dispara WM_DESTROY
        }
        break;

    case WM_DESTROY:
        if (conexionBuscar) {
            conexionBuscar->cerrarConexion();  // Cierra PostgreSQL
            delete conexionBuscar;
            conexionBuscar = nullptr;
            connBuscar = nullptr;
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }

    return 0;
}
