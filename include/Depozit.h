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
    Depozit();

    void adaugaProdus(const Produs& produs);
    void eliminaProdus(int idProdus);
    
    //void afiseazaToateProdusele() const;
    //void genereazaRaportAlerta() const;

    Produs& getProdus(int idProdus);

    void vindeProdus(int id, int cantitate);
    void aprovizioneazaProdus(int id, int cantitate);

    //void salveazaInFisier(const std::string& numeFisier) const;
    //void incarcaDinFisier(const std::string& numeFisier);

    void cautaProdusDupaNume(const std::string& numeCautat) const;

    std::vector<Produs> getToateProdusele() const;

    void importaDateDinCSV(const std::string& cale);

    std::vector<std::unique_ptr<Produs>> getProdusePaginat(int limita, int offset);
    std::vector<std::unique_ptr<Produs>> getProduseCuStocCritic();

    std::vector<Tranzactie<std::string>> getIstoric();

    double getProfitRealizat();

    std::string proceseazaComandaCompleta(int idProdus, int cantitate, const std::string& client, const std::string& adresa, const std::string& idCamion);

    double verificaIncarcareVehicul(const std::string& id) { return dbManager.getGradIncarcare(id); }
    bool declanseazaExpediere(const std::string& id) { return dbManager.expediazaCamion(id); }

    bool adaugaCamionInFlota(const std::string& id, double cap, const std::string& stat);
    std::vector<std::string> getCamioaneDisponibile();
    std::vector<std::pair<std::string, std::string>> getToateCamioanele();
    
    void resetareTotala() {
        dbManager.golesteBazaDeDate();
    }
};