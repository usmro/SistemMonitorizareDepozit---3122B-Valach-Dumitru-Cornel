#include "ProdusElectronic.h"

ProdusElectronic::ProdusElectronic(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, int luniGarantie, double voltaj, double volume)
    : Produs(id, nume, cantitate, prag, pretAchiz, pretVanz, volume) {
        this->luniGarantie = luniGarantie;
        this->voltaj = voltaj;
    }

std::string ProdusElectronic::getTipProdus() const { 
    return "Electronic"; 
}

std::string ProdusElectronic::getDetaliiSpecifice() const { 
    return "Gar: " + std::to_string(luniGarantie) + " luni | Alim: " + std::to_string((int)voltaj) + "V"; 
}