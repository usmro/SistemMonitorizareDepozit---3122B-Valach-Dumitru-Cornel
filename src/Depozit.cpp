#include "Depozit.h"
#include <stdexcept>
#include <iomanip>
#include <ctime>

Depozit::Depozit() : dbManager("depozit_complex.db") {
    std::vector<Produs> produse_salvate = dbManager.incarcaProduse();
    
    for (const auto& p : produse_salvate) {
        stoc.insert({p.getId(), p});
    }
}

void Depozit::resetareTotala() {
    dbManager.golesteBazaDeDate();
    stoc.clear();
}

void Depozit::adaugaProdus(const Produs& produs) {
    if (stoc.find(produs.getId()) != stoc.end()) {
        throw std::invalid_argument("Eroare: Un produs cu acest ID exista deja in depozit!");
    }
    
    stoc.insert_or_assign(produs.getId(), produs);
    dbManager.salveazaProdus(produs);
    std::cout << "[+] Produsul '" << produs.getNume() << "' a fost adaugat in depozit.\n";
}

void Depozit::eliminaProdus(int idProdus) {
    if (stoc.erase(idProdus) == 0) {
        throw std::invalid_argument("Eroare: Produsul cu ID-ul specificat nu a fost gasit!");
    }
    std::cout << "[-] Produsul cu ID " << idProdus << " a fost eliminat.\n";
}

Produs& Depozit::getProdus(int idProdus) {
    auto it = stoc.find(idProdus);
    if (it != stoc.end()) {
        return it->second;
    }
    throw std::invalid_argument("Eroare: Produsul nu exista!");
}

void Depozit::vindeProdus(int id, int cantitateVanduta, const std::string& idCamion) {
    Produs& p = getProdus(id); 
    bool necesitaLivrare = !idCamion.empty() && idCamion != "- Ridicare Personala -";

    if (necesitaLivrare) {
        double volum_total_comanda = p.getVolum() * cantitateVanduta;
        double capacitate_masina = dbManager.getCapacitateCamion(idCamion);
        
        if (capacitate_masina > 0 && volum_total_comanda > capacitate_masina) {
            double procent = (volum_total_comanda / capacitate_masina) * 100.0;
            
            std::stringstream ssMesaj;
            ssMesaj << std::fixed << std::setprecision(1) 
                    << "Eroare Logistica: Volumul marfii (" << volum_total_comanda 
                    << " m3) depaseste capacitatea utilitarei (" << capacitate_masina 
                    << " m3). Ocupa " << procent << "% din spatiu!";
                    
            throw std::runtime_error(ssMesaj.str());
        }
    }

    p -= cantitateVanduta;     
    
    dbManager.actualizeazaStocInDB(id, p.getCantitate());
    dbManager.adaugaInIstoric(id, "VANZARE", cantitateVanduta);
}

void Depozit::aprovizioneazaProdus(int id, int cantitate) {
    Produs& p = getProdus(id); 
    p += cantitate; 
    
    dbManager.actualizeazaStocInDB(id, p.getCantitate());
    dbManager.adaugaInIstoric(id, "APROVIZIONARE", cantitate);
}

int Depozit::getNumarTotalTranzactii() { 
    return dbManager.getNumarTotalTranzactii(); 
}

void Depozit::cautaProdusDupaNume(const std::string& numeCautat) const {
    bool gasit = false;
    std::cout << "\n=== REZULTATE CAUTARE: '" << numeCautat << "' ===\n";
    
    for (const auto& [id, produs] : stoc) {
        if (produs.getNume().find(numeCautat) != std::string::npos) {
            gasit = true;
        }
    }
    
    if (!gasit) {
        std::cout << "[!] Nu s-a gasit niciun produs care sa contina '" << numeCautat << "'.\n";
    }
}

std::vector<Produs> Depozit::getToateProdusele() const {
    std::vector<Produs> lista;
    for (const auto& [id, produs] : stoc) {
        lista.push_back(produs);
    }
    return lista;
}

void Depozit::importaDateDinCSV(const std::string& cale) {
    dbManager.importaMasivDinCSV(cale);
    auto produse_noi = dbManager.incarcaProduse();
    stoc.clear();
    for (const auto& p : produse_noi) {
        stoc.insert_or_assign(p.getId(), p);
    }
}

int Depozit::genereazaIdProdusNou() {
    return dbManager.getUrmatorulIdProdus();
}

std::vector<std::unique_ptr<Produs>> Depozit::getProdusePaginat(int limita, int offset) {
    return dbManager.getProdusePaginat(limita, offset);
}

std::vector<std::unique_ptr<Produs>> Depozit::getProduseCuStocCritic() {
    return dbManager.getProduseCuStocCritic();
}

std::vector<Tranzactie<std::string>> Depozit::getIstoric() { 
    return dbManager.getIstoricTranzactii(); 
}

double Depozit::getProfitRealizat() { 
    return dbManager.getProfitRealizat(); 
}

std::string Depozit::proceseazaComandaCompleta(int idProdus, int cantitate, const std::string& client, const std::string& adresa, const std::string& idCamion) {
    vindeProdus(idProdus, cantitate, idCamion); 

    auto acum = std::time(nullptr);
    std::string awb = "AWB-" + std::to_string(acum) + "-" + std::to_string(rand() % 10000 + 1000);

    bool salvat = dbManager.salveazaComanda(awb, idProdus, cantitate, client, adresa, idCamion);
    if (!salvat) {
        throw std::runtime_error("Eroare SQL: Comanda nu a putut fi salvata in tabelul Comenzi!");
    }

    if (idCamion != "Fara Transport (Ridicare personala)") {
        dbManager.expediazaCamion(idCamion); 
    }

    return awb;
}

double Depozit::getCapacitateCamion(const std::string& idCamion) { 
    return dbManager.getCapacitateCamion(idCamion); 
}

bool Depozit::adaugaCamionInFlota(const std::string& id, double cap, const std::string& stat) {
    return dbManager.adaugaCamion(id, cap, stat);
}

std::vector<std::string> Depozit::getCamioaneDisponibile() {
    return dbManager.getCamioaneDisponibile();
}

std::vector<std::pair<std::string, std::string>> Depozit::getToateCamioanele() {
    return dbManager.getToateCamioanele();
}

std::vector<std::string> Depozit::getCamioaneInCursa() {
    return dbManager.getCamioaneInCursa();
}

std::vector<std::string> Depozit::getCamioaneInService() {
    return dbManager.getCamioaneInService();
}

bool Depozit::declanseazaExpediere(const std::string& idCamion) {
    return dbManager.expediazaCamion(idCamion);
}

double Depozit::verificaIncarcareVehicul(const std::string& idCamion) {
    return dbManager.getGradIncarcare(idCamion);
}

bool Depozit::finalizeazaCursa(const std::string& idCamion) {
    return dbManager.finalizeazaCursa(idCamion);
}

bool Depozit::efectueazaRevizie(const std::string& idCamion, const std::string& tip) { 
    return dbManager.efectueazaRevizie(idCamion, tip); 
}

std::vector<InregistrareService> Depozit::getIstoricService() { 
    return dbManager.getIstoricService(); 
}

void Depozit::adaugaFurnizor(const Furnizor& f) {
    dbManager.salveazaFurnizor(f);
}

std::vector<Furnizor> Depozit::getFurnizori() {
    return dbManager.getFurnizori();
}

bool Depozit::stergeFurnizor(int id) {
    return dbManager.stergeFurnizor(id);
}

std::vector<Furnizor> Depozit::getFurnizoriPaginat(int limita, int offset) {
    return dbManager.getFurnizoriPaginat(limita, offset);
}

int Depozit::getTotalFurnizori() {
    return dbManager.getTotalFurnizori();
}