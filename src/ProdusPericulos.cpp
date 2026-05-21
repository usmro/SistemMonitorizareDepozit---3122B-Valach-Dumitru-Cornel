#include "ProdusPericulos.h"

ProdusPericulos::ProdusPericulos(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, std::string risc, double volum)
    : Produs(id, nume, cantitate, prag, pretAchiz, pretVanz, volum) {
        this->clasaRisc = risc;
    }

std::string ProdusPericulos::getTipProdus() const { 
    return "Periculos"; 
}

std::string ProdusPericulos::getDetaliiSpecifice() const { 
    return "RISC: " + clasaRisc + " | Zona izolata"; 
}