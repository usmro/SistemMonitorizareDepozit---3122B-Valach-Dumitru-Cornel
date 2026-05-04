#include "Produs.h"
#include <stdexcept>

Produs::Produs(int id, std::string nume, int cantitate, double pret, int pragAlerta)
    : id(id), nume(nume), cantitate(cantitate), pret(pret), pragAlerta(pragAlerta) {}

void Produs::setCantitate(int nouaCantitate) {
    if (nouaCantitate < 0) {
        throw std::invalid_argument("Cantitatea nu poate fi negativa!");
    }
    cantitate = nouaCantitate;
}

void Produs::setPret(double nouPret) {
    if (nouPret < 0) {
        throw std::invalid_argument("Pretul nu poate fi negativ!");
    }
    pret = nouPret;
}

Produs& Produs::operator+=(int adaugaCantitate) {
    if (adaugaCantitate < 0) {
        throw std::invalid_argument("Nu poti adauga o cantitate negativa!");
    }
    this->cantitate += adaugaCantitate;
    return *this;
}

Produs& Produs::operator-=(int scadeCantitate) {
    if (scadeCantitate < 0) {
        throw std::invalid_argument("Nu poti scadea o cantitate negativa!");
    }
    if (this->cantitate - scadeCantitate < 0) {
        throw std::runtime_error("Eroare: Stoc insuficient pentru vanzare!");
    }
    this->cantitate -= scadeCantitate;
    return *this;
}

void Produs::afiseazaDetalii() const {
    std::cout << "[PRODUS STANDARD] ID: " << id 
              << " | Nume: " << nume 
              << " | Stoc: " << cantitate 
              << " | Pret: " << pret << " RON\n";
}

std::ostream& operator<<(std::ostream& os, const Produs& produs) {
    os << "ID: " << produs.id 
       << " | Nume: " << produs.nume 
       << " | Cantitate: " << produs.cantitate 
       << " | Pret: " << produs.pret << " RON";
    return os;
}

int Produs::getId() const { return id; }
std::string Produs::getNume() const { return nume; }
int Produs::getCantitate() const { return cantitate; }
double Produs::getPret() const { return pret; }
int Produs::getPragAlerta() const { return pragAlerta; }