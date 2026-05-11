#include "ProdusVoluminos.h"

ProdusVoluminos::ProdusVoluminos(int i, std::string n, int c, double p, int pr, double greutate) 
    : Produs(i, n, c, p, pr), greutateKg(greutate) {
        necesitaMotostivuitor = (greutate > 30.0) ? "DA" : "NU";
}

std::string ProdusVoluminos::getTipProdus() const { 
    return "Voluminos"; 
}

std::string ProdusVoluminos::getDetaliiSpecifice() const { 
    return "Masa: " + std::to_string((int)greutateKg) + "kg | Motostivuitor: " + necesitaMotostivuitor; 
}