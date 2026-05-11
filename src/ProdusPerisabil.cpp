#include "ProdusPerisabil.h"

ProdusPerisabil::ProdusPerisabil(int i, std::string n, int c, double p, int pr, int temp, int umiditate) 
    : Produs(i, n, c, p, pr), tempStocare(temp), umiditateMaxima(umiditate) {}

std::string ProdusPerisabil::getTipProdus() const { 
    return "Perisabil"; 
}

std::string ProdusPerisabil::getDetaliiSpecifice() const { 
    return "Temp: " + std::to_string(tempStocare) + "C | Umiditate max: " + std::to_string(umiditateMaxima) + "%"; 
}