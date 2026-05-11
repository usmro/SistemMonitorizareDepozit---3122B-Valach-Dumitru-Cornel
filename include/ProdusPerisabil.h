#pragma once
#include "Produs.h"

class ProdusPerisabil : public Produs {
private:
    int tempStocare;
    int umiditateMaxima;

public:
    ProdusPerisabil(int i, std::string n, int c, double p, int pr, int temp, int umiditate);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};