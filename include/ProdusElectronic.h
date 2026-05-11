#pragma once
#include "Produs.h"

class ProdusElectronic : public Produs {
private:
    int garantieLuni;
    double tensiuneAlimentare;
public:
    ProdusElectronic(int i, std::string n, int c, double p, int pr, int garantie, double tensiune);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};