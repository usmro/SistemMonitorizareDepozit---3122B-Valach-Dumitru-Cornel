#pragma once
#include "Produs.h"

class ProdusPerisabil : public Produs {
private:
    int zileValabilitate;
    int temperatura;

public:
    ProdusPerisabil(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, int zileValabilitate, int temperatura, double volume);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};