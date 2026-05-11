#pragma once
#include "Produs.h"

class ProdusPericulos : public Produs {
private:
    std::string clasaRisc;

public:
    ProdusPericulos(int i, std::string n, int c, double p, int pr, std::string risc);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};