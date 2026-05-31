#pragma once
#include "Produs.h"

class ProdusVoluminos : public Produs {
private:
    double greutateKg;
    std::string necesitaMotostivuitor;

public:
    ProdusVoluminos(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, double greutate, double volume);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};