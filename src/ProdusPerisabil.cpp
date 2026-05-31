#include "ProdusPerisabil.h"

ProdusPerisabil::ProdusPerisabil(int id, const std::string& nume, int cantitate, int prag, double pretAchiz, double pretVanz, int zileValabilitate, int temperatura, double volume) 
    : Produs(id, nume, cantitate, prag, pretAchiz, pretVanz, volume) {
        this->zileValabilitate = zileValabilitate;
        this->temperatura = temperatura;
    }

std::string ProdusPerisabil::getTipProdus() const { 
    return "Perisabil"; 
}

std::string ProdusPerisabil::getDetaliiSpecifice() const { 
    return "Valabilitate: " + std::to_string(zileValabilitate) + "zile | Temperatura: " + std::to_string(temperatura) + "°C"; 
}