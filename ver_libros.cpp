#include "ver_libros.h"
#include "conexion.h"
#include <commctrl.h>  // Para controles comunes (ListView)
#include <string>
#include <vector>

// Indica al linker que debe incluir la librería comctl32.lib necesaria para controles comunes
#pragma comment(lib, "comctl32.lib")

extern PGconn* conexionGlobal; 


LRESULT CALLBACK VerLibrosProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

// Función para crear y mostrar la ventana "Ver Libros"
void AbrirVentanaVerLibros(HWND hwndPadre) {
    // Estructura para registrar la clase de la ventana
    WNDCLASS wc = {};
    wc.lpfnWndProc = VerLibrosProc;             
    wc.hInstance = GetModuleHandle(NULL);       
    wc.lpszClassName = "VerLibrosClass";        

    RegisterClass(&wc);  // Registrar la clase de ventana

    // Crear la ventana principal con ciertas características
    HWND hwnd = CreateWindowEx(
        0,                          
        "VerLibrosClass",           
        "Libros Disponibles",       // Título de la ventana
        WS_OVERLAPPEDWINDOW         // Estilo ventana normal
            & ~WS_SYSMENU           // Sin menú sistema (cerrar)
            & ~WS_MAXIMIZEBOX       // Sin maximizar
            & ~WS_MINIMIZEBOX,      // Sin minimizar
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 400,  // Posición y tamaño
        hwndPadre,                  
        NULL,                       
        GetModuleHandle(NULL),      
        NULL                        
    );

    // Centrar la ventana en la pantalla
    RECT rect;
    GetWindowRect(hwnd, &rect);               // Obtener tamaño actual
    int w = rect.right - rect.left;           // Ancho
    int h = rect.bottom - rect.top;           // Alto
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;  // X centrado
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;  // Y centrado
    MoveWindow(hwnd, x, y, w, h, FALSE);     // Mover ventana a posición centrada

    ShowWindow(hwnd, SW_SHOW);                // Mostrar ventana
    UpdateWindow(hwnd);                       // Actualizar/redibujar ventana
}

// Función para cargar libros desde la base de datos y llenar el ListView
void CargarLibros(HWND hwndListView) {
    if (!conexionGlobal) return;  // Si no hay conexión, salir

    // Ejecutar consulta SQL para obtener todos los libros
    PGresult* res = PQexec(conexionGlobal, "SELECT * FROM libros");

    // Verificar si la consulta fue exitosa
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);  // Liberar recursos
        return;
    }

    int rows = PQntuples(res);  // Número de filas devueltas

    // Recorrer cada fila y obtener los valores de las columnas
    for (int i = 0; i < rows; i++) {
        string titulo = PQgetvalue(res, i, 0);
        string autor = PQgetvalue(res, i, 1);
        string isbn = PQgetvalue(res, i, 2);
        string editorial = PQgetvalue(res, i, 3);
        string anio = PQgetvalue(res, i, 4);
        string area = PQgetvalue(res, i, 5);
        string disponibilidad = PQgetvalue(res, i, 6);

        // Crear un nuevo item en el ListView para el título
        LVITEM item = {};
        item.mask = LVIF_TEXT;              // Indicamos que vamos a pasar texto
        item.iItem = i;                     // Índice de la fila
        item.pszText = const_cast<char*>(titulo.c_str());  // Texto (titulo)
        ListView_InsertItem(hwndListView, &item);          // Insertar la fila

        // Establecer el texto para las demás columnas de la fila
        ListView_SetItemText(hwndListView, i, 1, const_cast<char*>(autor.c_str()));
        ListView_SetItemText(hwndListView, i, 2, const_cast<char*>(isbn.c_str()));
        ListView_SetItemText(hwndListView, i, 3, const_cast<char*>(editorial.c_str()));
        ListView_SetItemText(hwndListView, i, 4, const_cast<char*>(anio.c_str()));
        ListView_SetItemText(hwndListView, i, 5, const_cast<char*>(area.c_str()));
        ListView_SetItemText(hwndListView, i, 6, const_cast<char*>(disponibilidad.c_str()));
    }

    PQclear(res);  // Liberar memoria usada por el resultado
}

// Procedimiento de ventana para la ventana "Ver Libros"
LRESULT CALLBACK VerLibrosProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HWND listView; 

    switch (msg) {
    case WM_CREATE: {
        InitCommonControls();  // Inicializar controles comunes (ListView, etc.)

        // Crear el control ListView para mostrar los libros
        listView = CreateWindowEx(0, WC_LISTVIEW, "",
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            10, 10, 760, 300,
            hwnd, NULL, GetModuleHandle(NULL), NULL);

        // Estilo para seleccionar toda la fila (no solo una celda)
        SendMessage(listView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

        // Definir columnas para el ListView
        LVCOLUMN col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        // Nombres de columnas
        const char* headers[] = { "Titulo", "Autor", "ISBN", "Editorial", "Anio", "Area", "Disponibilidad" };
        for (int i = 0; i < 7; i++) {
            col.pszText = const_cast<char*>(headers[i]);
            col.cx = 100;  // Ancho de cada columna
            ListView_InsertColumn(listView, i, &col);
        }

        // Cargar libros desde la base de datos y mostrarlos
        CargarLibros(listView);

        // Crear botón "Cerrar" para cerrar la ventana
        CreateWindow("BUTTON", "Cerrar", WS_VISIBLE | WS_CHILD,
            340, 320, 100, 30, hwnd, (HMENU)10, NULL, NULL);

        break;
    }

    case WM_COMMAND:
        if (LOWORD(wp) == 10) {   // Si se pulsa el botón "Cerrar"
            DestroyWindow(hwnd);  // Cerrar la ventana
            return 0;
        }
        break;

    case WM_DESTROY:
        return 0;

    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wp, lp);  // Llamar al procedimiento por defecto
}
