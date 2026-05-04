cmake_minimum_required(VERSION 3.10)
project(MonitorizareDepozit)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Spunem compilatorului unde sa gaseasca fisierele .h
include_directories(include)

# Aplicatia Principala (Cea cu meniu)
add_executable(depozit_app 
    src/main.cpp
    src/Produs.cpp
    src/ProdusPerisabil.cpp
    src/ProdusElectronic.cpp 
    src/Depozit.cpp
)

# Aplicatia de Testare (Cea cu assert-uri)
add_executable(test_app 
    tests/test_main.cpp
    src/Produs.cpp
    src/Depozit.cpp
)