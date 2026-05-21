#include "ProdusFragil.h"

ProdusFragil::ProdusFragil(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, int maxStivuire, double volume)
    : Produs(id, nume, cantitate, prag, pretAchiz, pretVanz, volume){}

std::string ProdusFragil::getTipProdus() const { 
    return "Fragil"; 
}

std::string ProdusFragil::getDetaliiSpecifice() const { 
    return "ATENTIE! Stivuire max: " + std::to_string(maxStivuire) + " cutii"; 
}