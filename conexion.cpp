#include "conexion.h"

ConexionDB::ConexionDB() : conn(nullptr) {
    const char *conninfo = "dbname=postgres user=postgres password=admin$ host=localhost port=5432";
    conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Error de conexion: " << PQerrorMessage(conn) << endl;
    } else {
        cout << "--------------------" << endl;
        cout << "| Conexion exitosa |" << endl;
        cout << "--------------------" << endl;
    }
}

ConexionDB::~ConexionDB() {
    cerrarConexion();
}

PGconn* ConexionDB::obtenerConexion() {
    return conn;
}

void ConexionDB::cerrarConexion() {
    if (conn != nullptr) {
        PQfinish(conn);
        conn = nullptr;
        cout << "Conexion cerrada." << endl;
    }
}
