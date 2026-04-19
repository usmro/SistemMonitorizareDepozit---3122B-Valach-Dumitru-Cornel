#include "ProdusElectronic.h"

ProdusElectronic::ProdusElectronic(int id, std::string nume, int cant, double pret, int prag, int garantie)
    : Produs(id, nume, cant, pret, prag), luniGarantie(garantie) {}

void ProdusElectronic::afiseazaDetalii() const {
    std::cout << "[ELECTRONIC] ID: " << id << " | Nume: " << nume 
              << " | Stoc: " << cantitate << " | Garantie: " << luniGarantie << " luni\n";
}