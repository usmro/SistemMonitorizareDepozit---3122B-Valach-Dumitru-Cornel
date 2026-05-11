#include "Produs.h"

// Implementarea constructorului
Produs::Produs(int i, std::string n, int c, double p, int pr) 
    : id(i), nume(n), cantitate(c), pret(p), pragAlerta(pr) {}

// Implementarea metodelor de bază
std::string Produs::getTipProdus() const { 
    return "General"; 
}

std::string Produs::getDetaliiSpecifice() const { 
    return "Manipulare standard"; 
}

Produs& Produs::operator+=(int valoare) {
    if (valoare > 0) {
        this->cantitate += valoare;
    }
    return *this;
}

Produs& Produs::operator-=(int valoare) {
    if (valoare > 0 && this->cantitate >= valoare) {
        this->cantitate -= valoare;
    }
    return *this;
}