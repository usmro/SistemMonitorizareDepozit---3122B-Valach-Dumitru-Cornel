#include <iostream>
#include "Depozit.h"
#include "Tranzactie.h"

void afiseazaMeniu() {
    std::cout << "\n=== SISTEM GESTIUNE DEPOZIT ===\n";
    std::cout << "1. Adauga Produs Nou\n";
    std::cout << "2. Afiseaza Stoc Complet\n";
    std::cout << "3. Vanzare (Eliminare automata la 0)\n";
    std::cout << "4. Aprovizionare (Intrare Stoc)\n";
    std::cout << "5. Raport Alerte (Stoc Critic)\n";
    std::cout << "0. Iesire\n";
    std::cout << "Selectati optiunea: ";
}

void optiuneAdaugare(Depozit& depozit) {
    int id, cant, prag; 
    double pret; 
    std::string nume;
    
    std::cout << "ID: "; std::cin >> id;
    std::cout << "Nume: "; std::cin >> nume;
    std::cout << "Cantitate: "; std::cin >> cant;
    std::cout << "Pret: "; std::cin >> pret;
    std::cout << "Prag Alerta: "; std::cin >> prag;

    depozit.adaugaProdus(Produs(id, nume, cant, pret, prag));
}

void optiuneAfisare(const Depozit& depozit) {
    depozit.afiseazaToateProdusele();
}

void optiuneVanzare(Depozit& depozit) {
    int id, cant;
    std::cout << "ID Produs de vandut: "; std::cin >> id;
    std::cout << "Cantitate de vandut: "; std::cin >> cant;
    
    depozit.vindeProdus(id, cant); 
    Tranzactie<std::string>(id, cant, "VANZARE").afiseazaDetalii();
}

void optiuneAprovizionare(Depozit& depozit) {
    int id, cant;
    std::cout << "ID Produs de aprovizionat: "; std::cin >> id;
    std::cout << "Cantitate de adaugat: "; std::cin >> cant;
    
    depozit.getProdus(id) += cant;
    Tranzactie<std::string>(id, cant, "APROVIZIONARE").afiseazaDetalii();
}

void optiuneRapoarte(const Depozit& depozit) {
    depozit.genereazaRaportAlerta();
}

int main() {
    Depozit depozit;
    int optiune;

    while (true) {
        afiseazaMeniu();
        std::cin >> optiune;

        if (optiune == 0) break;

        try {
            switch (optiune) {
                case 1: optiuneAdaugare(depozit);       break;
                case 2: optiuneAfisare(depozit);        break;
                case 3: optiuneVanzare(depozit);        break;
                case 4: optiuneAprovizionare(depozit);  break;
                case 5: optiuneRapoarte(depozit);       break;
                default:
                    std::cout << "Optiune invalida!\n";
            }
        } catch (const std::exception& e) {
            std::cout << "EROARE: " << e.what() << "\n";
        }
    }

    std::cout << "Aplicatie inchisa.\n";
    return 0;
}