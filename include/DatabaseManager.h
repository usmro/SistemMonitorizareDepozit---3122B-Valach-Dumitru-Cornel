#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

class Produs;
template <typename T> class Tranzactie;

struct InregistrareService {
    std::string idCamion;
    std::string data;
    std::string tipInterventie;
};

class DatabaseManager {
private:
    sqlite3* db;

    void creeazaTabele();
    void creeazaTabelIstoric();
    void creeazaTabelComenzi();
    void creeazaTabelMentenanta();

public:
    explicit DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    void golesteBazaDeDate();

    void salveazaProdus(const Produs& p);
    std::vector<Produs> incarcaProduse();
    int getUrmatorulIdProdus();
    void importaMasivDinCSV(const std::string& numeFisier);
    std::vector<std::unique_ptr<Produs>> getProdusePaginat(int limita, int offset);
    std::vector<std::unique_ptr<Produs>> getProduseCuStocCritic();
    bool actualizeazaStocInDB(int id, int nouaCantitate);
    bool actualizeazaPreturiProdus(int id, double nouAchiz, double nouVanz);

    bool adaugaInIstoric(int idProdus, const std::string& tip, int cantitate);
    std::vector<Tranzactie<std::string>> getIstoricTranzactii();
    double getProfitRealizat();

    bool adaugaCamion(const std::string& id, double capacitate, const std::string& status);
    double getCapacitateCamion(const std::string& idCamion);
    std::vector<std::pair<std::string, std::string>> getToateCamioanele();
    std::vector<std::string> getCamioaneDisponibile();
    std::vector<std::string> getCamioaneInCursa();
    std::vector<std::string> getCamioaneInService();
    bool actualizeazaStatusCamion(const std::string& idCamion, const std::string& noulStatus);
    double getGradIncarcare(const std::string& idCamion);

    bool salveazaComanda(const std::string& awb, int idProdus, int cantitate, const std::string& client, const std::string& adresa, const std::string& idCamion);
    bool expediazaCamion(const std::string& idCamion);
    bool finalizeazaCursa(const std::string& idCamion);


    bool efectueazaRevizie(const std::string& idCamion, const std::string& tipInterventie = "Revizie periodica (Limita curse)");
    std::vector<InregistrareService> getIstoricService();
};

#endif // DATABASEMANAGER_H