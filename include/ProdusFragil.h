#pragma once
#include "Produs.h"

class ProdusFragil : public Produs {
private:
    int maxStivuire;

public:
    ProdusFragil(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, int maxStivuire, double volume);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};