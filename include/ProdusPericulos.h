#pragma once
#include "Produs.h"

class ProdusPericulos : public Produs {
private:
    std::string clasaRisc;

public:
    ProdusPericulos(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, std::string risc, double volum);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};