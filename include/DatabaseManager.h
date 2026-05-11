#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include "Produs.h"
#include "ProdusElectronic.h"
#include "ProdusPerisabil.h"

class DatabaseManager {
private:
    sqlite3* db;
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    void creeazaTabele();
    void salveazaProdus(const Produs& p);
    std::vector<Produs> incarcaProduse();
    
    // Metode noi pentru ERP
    void adaugaAngajat(const std::string& nume, const std::string& rol);
    void inregistreazaTranzactie(int produsId, int cantitate, const std::string& tip);

    void importaMasivDinCSV(const std::string& numeFisier);
    std::vector<std::unique_ptr<Produs>> getProdusePaginat(int limita, int offset);
};