#include "ProdusPericulos.h"

ProdusPericulos::ProdusPericulos(int i, std::string n, int c, double p, int pr, std::string risc) 
    : Produs(i, n, c, p, pr), clasaRisc(risc) {}

std::string ProdusPericulos::getTipProdus() const { 
    return "Periculos"; 
}

std::string ProdusPericulos::getDetaliiSpecifice() const { 
    return "RISC: " + clasaRisc + " | Zona izolata"; 
}