#ifndef CONEXION_DB_H
#define CONEXION_DB_H

#include <libpq-fe.h>
#include <iostream>

using namespace std;

class ConexionDB {
private:
    PGconn* conn;
public:
    ConexionDB();
    ~ConexionDB();

    PGconn* obtenerConexion();
    void cerrarConexion();
};

#endif // CONEXION_DB_H
