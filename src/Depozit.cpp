#include "Depozit.h"
#include <stdexcept>
#include <fstream>
#include <sstream>

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

// void Depozit::afiseazaToateProdusele() const {
//     if (stoc.empty()) {
//         std::cout << "\n[!] Depozitul este complet gol.\n";
//         return;
//     }
    
//     std::cout << "\n=== STOC CURENT DEPOZIT ===\n";
//     for (const auto& [id, produs] : stoc) {
//         //std::cout << produs << "\n";
//     }
//     std::cout << "===========================\n";
// }

// void Depozit::genereazaRaportAlerta() const {
//     std::cout << "\n=== RAPORT: PRODUSE SUB PRAGUL DE ALERTA ===\n";
//     bool gasit = false;
    
//     for (const auto& [id, produs] : stoc) {
//         if (produs.getCantitate() <= produs.getPragAlerta()) {
//             std::cout << "-> ATENTIE: '" << produs.getNume() 
//                       << "' mai are doar " << produs.getCantitate() << " bucati!\n";
//             gasit = true;
//         }
//     }
    
//     if (!gasit) {
//         std::cout << "Toate produsele sunt in stoc suficient. Totul este OK!\n";
//     }
//     std::cout << "============================================\n";
// }

Produs& Depozit::getProdus(int idProdus) {
    auto it = stoc.find(idProdus);
    if (it != stoc.end()) {
        return it->second;
    }
    throw std::invalid_argument("Eroare: Produsul nu exista!");
}

void Depozit::vindeProdus(int id, int cantitateVanduta) {
    Produs& p = getProdus(id); 
    
    p -= cantitateVanduta;     
    
    dbManager.actualizeazaStocInDB(id, p.getCantitate());
}

// void Depozit::salveazaInFisier(const std::string& numeFisier) const {
//     std::ofstream out(numeFisier);
//     if (!out.is_open()) {
//         throw std::runtime_error("Eroare: Nu am putut deschide fisierul pentru salvare!");
//     }

//     for (const auto& [id, produs] : stoc) {
//         out << produs.getId() << ","
//             << produs.getNume() << ","
//             << produs.getCantitate() << ","
//             << produs.getPret() << ","
//             << produs.getPragAlerta() << "\n";
//     }
    
//     out.close();
//     std::cout << "[INFO] Stocul a fost salvat in siguranță în '" << numeFisier << "'.\n";
// }

// void Depozit::incarcaDinFisier(const std::string& numeFisier) {
//     std::ifstream in(numeFisier);
//     if (!in.is_open()) {
//         std::cout << "[INFO] Fisierul de stoc nu exista inca. Va fi creat automat la prima salvare.\n";
//         return;
//     }

//     std::string linie;
//     while (std::getline(in, linie)) {
//         if (linie.empty()) continue;

//         std::stringstream ss(linie);
//         std::string token;
        
//         int id, cantitate, pragAlerta;
//         double pret;
//         std::string nume;

//         std::getline(ss, token, ','); id = std::stoi(token);
//         std::getline(ss, nume, ',');
//         std::getline(ss, token, ','); cantitate = std::stoi(token);
//         std::getline(ss, token, ','); pret = std::stod(token);
//         std::getline(ss, token, ','); pragAlerta = std::stoi(token);

//         stoc.insert({id, Produs(id, nume, cantitate, pret, pragAlerta)});
//     }
    
//     in.close();
//     std::cout << "[INFO] Datele au fost incarcate cu succes din '" << numeFisier << "'.\n";
// }

void Depozit::cautaProdusDupaNume(const std::string& numeCautat) const {
    bool gasit = false;
    std::cout << "\n=== REZULTATE CAUTARE: '" << numeCautat << "' ===\n";
    
    for (const auto& [id, produs] : stoc) {
        if (produs.getNume().find(numeCautat) != std::string::npos) {
            //std::cout << produs << "\n";
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

// void Depozit::importaDateDinCSV(const std::string& cale) {
//     dbManager.importaMasivDinCSV(cale);
//     auto produse_noi = dbManager.incarcaProduse();
//     stoc.clear();
//     for (const auto& p : produse_noi) {
//         stoc.insert_or_assign(p.getId(), p);
//     }
// }

std::vector<std::unique_ptr<Produs>> Depozit::getProdusePaginat(int limita, int offset) {
    return dbManager.getProdusePaginat(limita, offset);
}

std::vector<std::unique_ptr<Produs>> Depozit::getProduseCuStocCritic() {
        return dbManager.getProduseCuStocCritic();
    }