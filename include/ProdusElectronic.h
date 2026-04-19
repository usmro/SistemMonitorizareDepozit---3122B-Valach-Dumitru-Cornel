#pragma once
#include "Produs.h"

class ProdusElectronic : public Produs {
private:
    int luniGarantie;

public:
    ProdusElectronic(int id, std::string nume, int cant, double pret, int prag, int garantie);
    void afiseazaDetalii() const override; 
};