#include <iostream>
#include "Depozit.h"

int main() {
    std::cout << "=== INITIALIZARE SISTEM DEPOZIT ===\n\n";    
    Depozit depozitCentral;

    try {
        depozitCentral.adaugaProdus(Produs(101, "Laptop Dell", 50, 3200.50, 10));
        depozitCentral.adaugaProdus(Produs(102, "Mouse Wireless", 5, 120.0, 15)); 
        depozitCentral.adaugaProdus(Produs(103, "Tastatura Mecanica", 30, 450.0, 5));
        
        std::cout << "\n[Securitate] Incercam adaugarea unui ID duplicat (101)...\n";
        depozitCentral.adaugaProdus(Produs(101, "Monitor Hacker", 10, 1000.0, 5));
        
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTIE PRINSĂ: " << e.what() << "\n";
    }

    depozitCentral.afiseazaToateProdusele();

    std::cout << "\n-> Se vand 10 Laptopuri Dell...\n";
    try {
        Produs& laptop = depozitCentral.getProdus(101); 
        laptop -= 10;
        std::cout << "Vanzare reusita! Noul stoc pentru Laptop: " << laptop.getCantitate() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Eroare la vanzare: " << e.what() << "\n";
    }

    depozitCentral.genereazaRaportAlerta();

    std::cout << "\n=== INCHIDERE SISTEM ===\n";
    return 0;
}