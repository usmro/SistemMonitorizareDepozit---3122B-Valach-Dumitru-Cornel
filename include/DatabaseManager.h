#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include "Produs.h"
#include "Tranzactie.h"
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

    void creeazaTabelIstoric();
    bool adaugaInIstoric(int idProdus, const std::string& tip, int cantitate);
    std::vector<Tranzactie<std::string>> getIstoricTranzactii();
    
    void adaugaAngajat(const std::string& nume, const std::string& rol);
    void inregistreazaTranzactie(int produsId, int cantitate, const std::string& tip);

    void importaMasivDinCSV(const std::string& numeFisier);
    std::vector<std::unique_ptr<Produs>> getProdusePaginat(int limita, int offset);

    std::vector<std::unique_ptr<Produs>> getProduseCuStocCritic();
    bool actualizeazaStocInDB(int id, int nouaCantitate);
    bool actualizeazaPreturiProdus(int id, double nouAchiz, double nouVanz);

    double getProfitRealizat();

    void creeazaTabelComenzi();
    void golesteBazaDeDate();

    bool adaugaCamion(const std::string& id, double capacitate, const std::string& status);
    std::vector<std::pair<std::string, std::string>> getToateCamioanele();
    std::vector<std::string> getCamioaneDisponibile();
    bool salveazaComanda(const std::string& awb, int idProdus, int cantitate, const std::string& client, const std::string& adresa, const std::string& idCamion);
    bool actualizeazaStatusCamion(const std::string& idCamion, const std::string& noulStatus);
    double getGradIncarcare(const std::string& idCamion);
    bool expediazaCamion(const std::string& idCamion);
};