#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

template <typename T>
class Tranzactie {
private:
    int idLog;
    int idProdus;
    int cantitate;
    T tipOperatie;
    std::time_t dataOra;
    double pret;

public:
    Tranzactie(int idL, int idP, int cant, T tip, double p = 0.0) 
        : idLog(idL), idProdus(idP), cantitate(cant), tipOperatie(tip) {
        auto acum = std::chrono::system_clock::now();
        dataOra = std::chrono::system_clock::to_time_t(acum);
        pret = p;
    }

    Tranzactie(int idL, int idP, int cant, T tip, std::time_t timpSalvat, double p = 0.0)
        : idLog(idL), idProdus(idP), cantitate(cant), tipOperatie(tip), dataOra(timpSalvat), pret(p) {}

    int getIdLog() const { return idLog; }
    int getIdProdus() const { return idProdus; }
    int getCantitate() const { return cantitate; }
    T getTipOperatie() const { return tipOperatie; }
    std::time_t getDataOra() const { return dataOra; }

    double getPret() const { return pret; }
    double getValoareTotala() const { return cantitate * pret; }

    std::string getDataOraString() const {
        struct tm* timeinfo = std::localtime(&dataOra);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    void afiseazaDetalii() const {
        std::cout << "[Tranzactie] Log ID: " << idLog 
                  << " | Produs ID: " << idProdus 
                  << " | Cantitate: " << cantitate 
                  << " | Tip: " << tipOperatie 
                  << " | Data: " << getDataOraString() << "\n";
    }
};