#include "Depozit.h"
#include <stdexcept>

void Depozit::adaugaProdus(const Produs& produs) {
    if (stoc.find(produs.getId()) != stoc.end()) {
        throw std::invalid_argument("Eroare: Un produs cu acest ID exista deja in depozit!");
    }
    
    stoc.insert({produs.getId(), produs});
    std::cout << "[+] Produsul '" << produs.getNume() << "' a fost adaugat in depozit.\n";
}

void Depozit::eliminaProdus(int idProdus) {
    if (stoc.erase(idProdus) == 0) {
        throw std::invalid_argument("Eroare: Produsul cu ID-ul specificat nu a fost gasit!");
    }
    std::cout << "[-] Produsul cu ID " << idProdus << " a fost eliminat.\n";
}

void Depozit::afiseazaToateProdusele() const {
    if (stoc.empty()) {
        std::cout << "\n[!] Depozitul este complet gol.\n";
        return;
    }
    
    std::cout << "\n=== STOC CURENT DEPOZIT ===\n";
    for (const auto& [id, produs] : stoc) {
        std::cout << "ID: " << id 
                  << " | Nume: " << produs.getNume() 
                  << " | Cantitate: " << produs.getCantitate() 
                  << " | Pret: " << produs.getPret() << " RON\n";
    }
    std::cout << "===========================\n";
}

void Depozit::genereazaRaportAlerta() const {
    std::cout << "\n=== RAPORT: PRODUSE SUB PRAGUL DE ALERTA ===\n";
    bool gasit = false;
    
    for (const auto& [id, produs] : stoc) {
        if (produs.getCantitate() <= produs.getPragAlerta()) {
            std::cout << "-> ATENTIE: '" << produs.getNume() 
                      << "' mai are doar " << produs.getCantitate() << " bucati!\n";
            gasit = true;
        }
    }
    
    if (!gasit) {
        std::cout << "Toate produsele sunt in stoc suficient. Totul este OK!\n";
    }
    std::cout << "============================================\n";
}

Produs& Depozit::getProdus(int idProdus) {
    auto it = stoc.find(idProdus);
    if (it != stoc.end()) {
        return it->second;
    }
    throw std::invalid_argument("Eroare: Produsul nu exista!");
}