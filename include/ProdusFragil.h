#pragma once
#include "Produs.h"

class ProdusFragil : public Produs {
private:
    int maxStivuire;

public:
    ProdusFragil(int i, std::string n, int c, double p, int pr, int maxStiv);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};