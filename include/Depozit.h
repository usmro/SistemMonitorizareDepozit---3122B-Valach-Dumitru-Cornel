#ifndef DEPOZIT_H
#define DEPOZIT_H

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "DatabaseManager.h"
#include "Produs.h"
#include "Tranzactie.h"
#include "Furnizor.h"

class Depozit {
private:
    DatabaseManager dbManager;
    std::unordered_map<int, Produs> stoc;
    std::vector<Furnizor> listaFurnizori;
public:
    Depozit();

    // gestiune stoc
    void adaugaProdus(const Produs& produs);
    void eliminaProdus(int idProdus);
    Produs& getProdus(int idProdus);
    std::vector<Produs> getToateProdusele() const;
    void cautaProdusDupaNume(const std::string& numeCautat) const;
    
    // tranzactii
    void vindeProdus(int id, int cantitateVanduta, const std::string& idCamion);
    void aprovizioneazaProdus(int id, int cantitate);
    int getNumarTotalTranzactii();

    // baza de date
    void importaDateDinCSV(const std::string& cale);
    int genereazaIdProdusNou();
    std::vector<std::unique_ptr<Produs>> getProdusePaginat(int limita, int offset);
    std::vector<std::unique_ptr<Produs>> getProduseCuStocCritic();
    void resetareTotala();
    
    // istoric
    std::vector<Tranzactie<std::string>> getIstoric();
    double getProfitRealizat();

    // comenzi
    std::string proceseazaComandaCompleta(int idProdus, int cantitate, const std::string& client, const std::string& adresa, const std::string& idCamion);
    bool adaugaCamionInFlota(const std::string& id, double cap, const std::string& stat);
    double getCapacitateCamion(const std::string& idCamion);
    std::vector<std::pair<std::string, std::string>> getToateCamioanele();
    std::vector<std::string> getCamioaneDisponibile();
    std::vector<std::string> getCamioaneInCursa();
    std::vector<std::string> getCamioaneInService();
    bool declanseazaExpediere(const std::string& idCamion);
    double verificaIncarcareVehicul(const std::string& idCamion);

    // mentenanta
    bool finalizeazaCursa(const std::string& idCamion);
    bool efectueazaRevizie(const std::string& idCamion, const std::string& tip = "Revizie periodica (Limita curse)");
    std::vector<InregistrareService> getIstoricService();

    // furnizori
    void adaugaFurnizor(const Furnizor& f);
    std::vector<Furnizor> getFurnizori();
    std::vector<Furnizor> getFurnizoriPaginat(int limita, int offset);
    int getTotalFurnizori();
    bool stergeFurnizor(int id);
};

#endif // DEPOZIT_H