#pragma once
#include <unordered_map>
#include <iostream>
#include <vector>
#include "Produs.h"
#include "DatabaseManager.h"

class Depozit {
private:
    std::unordered_map<int, Produs> stoc;
    DatabaseManager dbManager;
public:
    Depozit() : dbManager("depozit_complex.db") {
        std::vector<Produs> produse_salvate = dbManager.incarcaProduse();
        
        for (const auto& p : produse_salvate) {
            stoc.insert({p.getId(), p});
        }
    }

    void adaugaProdus(const Produs& produs);
    void eliminaProdus(int idProdus);
    
    void afiseazaToateProdusele() const;
    void genereazaRaportAlerta() const;

    Produs& getProdus(int idProdus);

    void vindeProdus(int id, int cantitate);

    void salveazaInFisier(const std::string& numeFisier) const;
    void incarcaDinFisier(const std::string& numeFisier);

    void cautaProdusDupaNume(const std::string& numeCautat) const;

    std::vector<Produs> getToateProdusele() const;

    void importaDateDinCSV(const std::string& cale);

    std::vector<std::unique_ptr<Produs>> getProdusePaginat(int limita, int offset);
};