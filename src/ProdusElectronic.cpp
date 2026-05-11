#include "ProdusElectronic.h"

ProdusElectronic::ProdusElectronic(int i, std::string n, int c, double p, int pr, int garantie, double tensiune) 
    : Produs(i, n, c, p, pr), garantieLuni(garantie), tensiuneAlimentare(tensiune) {}

std::string ProdusElectronic::getTipProdus() const { 
    return "Electronic"; 
}

std::string ProdusElectronic::getDetaliiSpecifice() const { 
    return "Gar: " + std::to_string(garantieLuni) + " luni | Alim: " + std::to_string((int)tensiuneAlimentare) + "V"; 
}