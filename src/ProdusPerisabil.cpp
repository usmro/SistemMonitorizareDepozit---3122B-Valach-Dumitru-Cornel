#include "ProdusPerisabil.h"

ProdusPerisabil::ProdusPerisabil(int id, std::string nume, int cant, double pret, int prag, std::string dataExp)
    : Produs(id, nume, cant, pret, prag), dataExpirare(dataExp) {}

void ProdusPerisabil::afiseazaDetalii() const {
    std::cout << "[PERISABIL] ID: " << id << " | Nume: " << nume 
              << " | Stoc: " << cantitate << " | Expira la: " << dataExpirare << "\n";
}