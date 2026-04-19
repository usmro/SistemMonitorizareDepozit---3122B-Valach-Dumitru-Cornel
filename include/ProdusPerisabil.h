#pragma once
#include "Produs.h"

class ProdusPerisabil : public Produs {
private:
    std::string dataExpirare;

public:
    ProdusPerisabil(int id, std::string nume, int cant, double pret, int prag, std::string dataExp);
    
    void afiseazaDetalii() const override; 
};