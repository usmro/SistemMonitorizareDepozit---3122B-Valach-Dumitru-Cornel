#include <iostream>
#include "Depozit.h"
#include "Tranzactie.h"

int citesteIntValid(const std::string& mesaj) {
    int valoare;
    while (true) {
        std::cout << mesaj;
        if (std::cin >> valoare && valoare >= 0) {
            return valoare;
        } else {
            std::cout << "[!] Eroare: Te rog introdu un numar intreg pozitiv valid!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Aruncam caracterele gresite
        }
    }
}

double citesteDoubleValid(const std::string& mesaj) {
    double valoare;
    while (true) {
        std::cout << mesaj;
        if (std::cin >> valoare && valoare >= 0) {
            return valoare;
        } else {
            std::cout << "[!] Eroare: Te rog introdu un numar valid!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void afiseazaMeniu() {
    std::cout << "\n=== SISTEM GESTIUNE DEPOZIT ===\n";
    std::cout << "1. Adauga Produs Nou\n";
    std::cout << "2. Afiseaza Stoc Complet\n";
    std::cout << "3. Vanzare (Eliminare automata la 0)\n";
    std::cout << "4. Aprovizionare (Intrare Stoc)\n";
    std::cout << "5. Raport Alerte (Stoc Critic)\n";
    std::cout << "6. Cautare produs dupa nume\n";
    std::cout << "0. Iesire\n";
    std::cout << "Selectati optiunea: ";
}

void optiuneAdaugare(Depozit& depozit) {
    int id = citesteIntValid("ID: ");

    std::string nume;
    std::cout << "Nume: "; 
    std::cin >> nume;
    
    int cant = citesteIntValid("Cantitate: ");
    double pret = citesteDoubleValid("Pret: ");
    int prag = citesteIntValid("Prag Alerta: ");

    depozit.adaugaProdus(Produs(id, nume, cant, pret, prag));
}

void optiuneAfisare(const Depozit& depozit) {
    depozit.afiseazaToateProdusele();
}

void optiuneVanzare(Depozit& depozit) {
    int id = citesteIntValid("ID Produs de vandut: ");
    int cant = citesteIntValid("Cantitate de vandut: ");
    
    depozit.vindeProdus(id, cant); 
    Tranzactie<std::string>(id, cant, "VANZARE").afiseazaDetalii();
}

void optiuneAprovizionare(Depozit& depozit) {
    int id = citesteIntValid("ID Produs de aprovizionat: ");
    int cant = citesteIntValid("Cantitate de adaugat: ");
    
    depozit.getProdus(id) += cant;
    Tranzactie<std::string>(id, cant, "APROVIZIONARE").afiseazaDetalii();
}

void optiuneRapoarte(const Depozit& depozit) {
    depozit.genereazaRaportAlerta();
}

void optiuneCautare(const Depozit& depozit) {
    std::string numeCautat;
    std::cout << "Introduceti textul de cautat: ";
    std::cin >> numeCautat;
    depozit.cautaProdusDupaNume(numeCautat);
}

int main() {
    Depozit depozit;
    int optiune;
    const std::string FISIER_STOC = "stoc.csv";

    depozit.incarcaDinFisier(FISIER_STOC);

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
                case 6: optiuneCautare(depozit);       break;
                default:
                    std::cout << "Optiune invalida!\n";
            }
        } catch (const std::exception& e) {
            std::cout << "EROARE: " << e.what() << "\n";
        }
    }

    depozit.salveazaInFisier(FISIER_STOC);
    std::cout << "Aplicatie inchisa.\n";
    return 0;
}