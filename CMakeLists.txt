cmake_minimum_required(VERSION 3.10)
project(MonitorizareDepozit)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

include_directories(include)
find_package(SQLite3 REQUIRED)
file(GLOB SOURCES "src/*.cpp")

add_executable(depozit_app ${SOURCES})

target_link_libraries(depozit_app PRIVATE 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
    SQLite::SQLite3
)

add_executable(test_app 
    tests/test_main.cpp
    src/Produs.cpp
    src/ProdusElectronic.cpp
    src/ProdusPerisabil.cpp
    src/ProdusFragil.cpp
    src/ProdusVoluminos.cpp
    src/ProdusPericulos.cpp
    src/Depozit.cpp
    src/DatabaseManager.cpp
)

target_link_libraries(test_app PRIVATE SQLite::SQLite3)