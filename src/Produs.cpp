#include "Produs.h"

Produs::Produs(int i, std::string n, int c,int pr, double pretAchiz, double pretVanz, double v) 
    : id(i), nume(n), cantitate(c), pragAlerta(pr), pretAchizitie(pretAchiz), pretVanzare(pretVanz), volum(v) {}

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