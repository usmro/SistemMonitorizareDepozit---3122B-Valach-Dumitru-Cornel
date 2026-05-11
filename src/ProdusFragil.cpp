#include "ProdusFragil.h"

ProdusFragil::ProdusFragil(int i, std::string n, int c, double p, int pr, int maxStiv) 
    : Produs(i, n, c, p, pr), maxStivuire(maxStiv) {}

std::string ProdusFragil::getTipProdus() const { 
    return "Fragil"; 
}

std::string ProdusFragil::getDetaliiSpecifice() const { 
    return "ATENTIE! Stivuire max: " + std::to_string(maxStivuire) + " cutii"; 
}