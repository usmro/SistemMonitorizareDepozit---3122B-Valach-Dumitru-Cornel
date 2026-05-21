#include "ProdusVoluminos.h"

ProdusVoluminos::ProdusVoluminos(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, double greutate, double volume)
    : Produs(id, nume, cantitate, prag, pretAchiz, pretVanz, volume) {
        necesitaMotostivuitor = (greutate > 30.0) ? "DA" : "NU";
}

std::string ProdusVoluminos::getTipProdus() const { 
    return "Voluminos"; 
}

std::string ProdusVoluminos::getDetaliiSpecifice() const { 
    return "Masa: " + std::to_string((int)greutateKg) + "kg | Motostivuitor: " + necesitaMotostivuitor; 
}