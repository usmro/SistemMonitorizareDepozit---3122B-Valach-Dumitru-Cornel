#pragma once
#include <unordered_map>
#include <iostream>
#include "Produs.h"

class Depozit {
private:
    std::unordered_map<int, Produs> stoc;

public:
    Depozit() = default;

    void adaugaProdus(const Produs& produs);
    void eliminaProdus(int idProdus);
    
    void afiseazaToateProdusele() const;
    void genereazaRaportAlerta() const;

    Produs& getProdus(int idProdus);

    void vindeProdus(int id, int cantitate);
};