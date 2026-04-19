#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

template <typename T>
class Tranzactie {
private:
    int idProdus;
    int cantitate;
    T tipOperatie;
    std::time_t dataOra;

public:
    Tranzactie(int id, int cant, T tip) 
        : idProdus(id), cantitate(cant), tipOperatie(tip) {
        auto acum = std::chrono::system_clock::now();
        dataOra = std::chrono::system_clock::to_time_t(acum);
    }

    void afiseazaDetalii() const {
        std::cout << "[Tranzactie] Produs ID: " << idProdus 
                  << " | Cantitate: " << cantitate 
                  << " | Tip: " << tipOperatie 
                  << " | Data: " << std::ctime(&dataOra);
    }
};