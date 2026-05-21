#pragma once
#include "Produs.h"

class ProdusElectronic : public Produs {
private:
    int luniGarantie;
    double voltaj;
public:
    ProdusElectronic(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, int luniGarantie, double voltaj, double volum);
    
    std::string getTipProdus() const override;
    std::string getDetaliiSpecifice() const override;
};