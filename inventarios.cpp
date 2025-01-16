#include <sqlite3.h>
#include <iostream>
#include <string>

sqlite3 *db;
char *errMsg = 0;

// Función para abrir la base de datos
int abrirDB() {
    int rc = sqlite3_open("inventario.db", &db);
    if (rc) {
        std::cerr << "Error al abrir la base de datos: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    return 0;
}

// Función para crear la tabla de productos
int crearTabla() {
    const char* sql = "CREATE TABLE IF NOT EXISTS productos ("
                      "id INTEGER PRIMARY KEY, "
                      "nombre TEXT, "
                      "descripcion TEXT, "
                      "cantidad INTEGER, "
                      "precio REAL);";
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al crear la tabla: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return rc;
    }
    return SQLITE_OK;
}

// Función para insertar un producto
int insertarProducto(const std::string& nombre, const std::string& descripcion, int cantidad, double precio) {
    const char* sql = "INSERT INTO productos (nombre, descripcion, cantidad, precio) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    sqlite3_bind_text(stmt, 1, nombre.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, descripcion.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, cantidad);
    sqlite3_bind_double(stmt, 4, precio);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Error al insertar producto: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Función para modificar un producto
int modificarProducto(int id, const std::string& nombre, const std::string& descripcion, int cantidad, double precio) {
    const char* sql = "UPDATE productos SET nombre = ?, descripcion = ?, cantidad = ?, precio = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    sqlite3_bind_text(stmt, 1, nombre.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, descripcion.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, cantidad);
    sqlite3_bind_double(stmt, 4, precio);
    sqlite3_bind_int(stmt, 5, id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Error al modificar producto: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Función para eliminar un producto
int eliminarProducto(int id) {
    const char* sql = "DELETE FROM productos WHERE id = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Error al eliminar producto: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Función para listar productos
void listarProductos() {
    const char* sql = "SELECT * FROM productos;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* nombre = (const char*)sqlite3_column_text(stmt, 1);
        const char* descripcion = (const char*)sqlite3_column_text(stmt, 2);
        int cantidad = sqlite3_column_int(stmt, 3);
        double precio = sqlite3_column_double(stmt, 4);

        std::cout << "ID: " << id << ", Nombre: " << nombre << ", Descripción: " << descripcion
                  << ", Cantidad: " << cantidad << ", Precio: " << precio << std::endl;
    }

    sqlite3_finalize(stmt);
}

// Función para buscar productos por nombre
void buscarProductoPorNombre(const std::string& nombre) {
    const char* sql = "SELECT * FROM productos WHERE nombre LIKE ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, ("%"+nombre+"%").c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cout << "No se encontraron productos con ese nombre." << std::endl;
    }

    while (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* nombre = (const char*)sqlite3_column_text(stmt, 1);
        const char* descripcion = (const char*)sqlite3_column_text(stmt, 2);
        int cantidad = sqlite3_column_int(stmt, 3);
        double precio = sqlite3_column_double(stmt, 4);

        std::cout << "ID: " << id << ", Nombre: " << nombre << ", Descripción: " << descripcion
                  << ", Cantidad: " << cantidad << ", Precio: " << precio << std::endl;
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
}

// Función para buscar productos por rango de precio
void buscarProductoPorPrecio(double precioMin, double precioMax) {
    const char* sql = "SELECT * FROM productos WHERE precio BETWEEN ? AND ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_double(stmt, 1, precioMin);
    sqlite3_bind_double(stmt, 2, precioMax);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        std::cout << "No se encontraron productos en ese rango de precio." << std::endl;
    }

    while (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* nombre = (const char*)sqlite3_column_text(stmt, 1);
        const char* descripcion = (const char*)sqlite3_column_text(stmt, 2);
        int cantidad = sqlite3_column_int(stmt, 3);
        double precio = sqlite3_column_double(stmt, 4);

        std::cout << "ID: " << id << ", Nombre: " << nombre << ", Descripción: " << descripcion
                  << ", Cantidad: " << cantidad << ", Precio: " << precio << std::endl;
        rc = sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
}

// Función para listar productos ordenados
void listarProductosOrdenados(const std::string& criterio) {
    std::string sql;
    
    if (criterio == "nombre") {
        sql = "SELECT * FROM productos ORDER BY nombre;";
    } else if (criterio == "precio") {
        sql = "SELECT * FROM productos ORDER BY precio;";
    } else {
        std::cout << "Criterio no válido." << std::endl;
        return;
    }

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* nombre = (const char*)sqlite3_column_text(stmt, 1);
        const char* descripcion = (const char*)sqlite3_column_text(stmt, 2);
        int cantidad = sqlite3_column_int(stmt, 3);
        double precio = sqlite3_column_double(stmt, 4);

        std::cout << "ID: " << id << ", Nombre: " << nombre << ", Descripción: " << descripcion
                  << ", Cantidad: " << cantidad << ", Precio: " << precio << std::endl;
    }

    sqlite3_finalize(stmt);
}

// Función para mostrar el menú con nuevas opciones
void mostrarMenu() {
    std::cout << "\nSeleccione una opción:\n";
    std::cout << "1. Añadir producto\n";
    std::cout << "2. Modificar producto\n";
    std::cout << "3. Eliminar producto\n";
    std::cout << "4. Listar productos\n";
    std::cout << "5. Buscar producto por nombre\n";
    std::cout << "6. Buscar producto por precio\n";
    std::cout << "7. Listar productos ordenados\n";
    std::cout << "8. Salir\n";
    std::cout << "Opción: ";
}

int main() {
    if (abrirDB() != 0) {
        return 1;
    }
    crearTabla();

    bool salir = false;
    while (!salir) {
        mostrarMenu();
        int opcion;
        std::cin >> opcion;

        switch (opcion) {
            case 1: {
                std::string nombre, descripcion;
                int cantidad;
                double precio;

                std::cout << "Nombre del producto: ";
                std::cin.ignore();  // Limpiar el buffer
                std::getline(std::cin, nombre);

                std::cout << "Descripción del producto: ";
                std::getline(std::cin, descripcion);

                std::cout << "Cantidad: ";
                std::cin >> cantidad;

                std::cout << "Precio: ";
                std::cin >> precio;

                if (insertarProducto(nombre, descripcion, cantidad, precio) == SQLITE_OK) {
                    std::cout << "Producto añadido con éxito." << std::endl;
                }
                break;
            }
            case 2: {
                int id;
                std::string nombre, descripcion;
                int cantidad;
                double precio;

                std::cout << "ID del producto a modificar: ";
                std::cin >> id;

                std::cout << "Nuevo nombre del producto: ";
                std::cin.ignore();  // Limpiar el buffer
                std::getline(std::cin, nombre);

                std::cout << "Nueva descripción del producto: ";
                std::getline(std::cin, descripcion);

                std::cout << "Nueva cantidad: ";
                std::cin >> cantidad;

                std::cout << "Nuevo precio: ";
                std::cin >> precio;

                if (modificarProducto(id, nombre, descripcion, cantidad, precio) == SQLITE_OK) {
                    std::cout << "Producto modificado con éxito." << std::endl;
                }
                break;
            }
            case 3: {
                int id;
                std::cout << "ID del producto a eliminar: ";
                std::cin >> id;

                if (eliminarProducto(id) == SQLITE_OK) {
                    std::cout << "Producto eliminado con éxito." << std::endl;
                }
                break;
            }
            case 4:
                listarProductos();
                break;
            case 5: {
                std::string nombre;
                std::cout << "Introduce el nombre del producto: ";
                std::cin.ignore();  // Limpiar el buffer
                std::getline(std::cin, nombre);
                buscarProductoPorNombre(nombre);
                break;
            }
            case 6: {
                double precioMin, precioMax;
                std::cout << "Introduce el precio mínimo: ";
                std::cin >> precioMin;
                std::cout << "Introduce el precio máximo: ";
                std::cin >> precioMax;
                buscarProductoPorPrecio(precioMin, precioMax);
                break;
            }
            case 7: {
                std::string criterio;
                std::cout << "¿Cómo deseas ordenar? (nombre/precio): ";
                std::cin >> criterio;
                listarProductosOrdenados(criterio);
                break;
            }
            case 8:
                salir = true;
                break;
            default:
                std::cout << "Opción no válida, intente de nuevo." << std::endl;
        }
    }

    sqlite3_close(db);
    return 0;
}
