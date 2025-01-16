cmake_minimum_required(VERSION 3.10)

project(inventarios)

set(CMAKE_CXX_STANDARD 11)

# Agregar la ubicación de SQLite3
find_package(SQLite3 REQUIRED)

# Incluir los directorios necesarios
include_directories(${SQLite3_INCLUDE_DIRS})

# Agregar tu archivo fuente y crear el ejecutable
add_executable(inventarios inventarios.cpp)

# Enlazar SQLite3
target_link_libraries(inventarios ${SQLite3_LIBRARIES})

