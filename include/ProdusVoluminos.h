#pragma once
#include "Produs.h"

class ProdusVoluminos : public Produs {
private:
    double greutateKg;
    std::string necesitaMotostivuitor;

public:
    ProdusVoluminos(int i, std::string n, int c, double p, int pr, double greutate);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};