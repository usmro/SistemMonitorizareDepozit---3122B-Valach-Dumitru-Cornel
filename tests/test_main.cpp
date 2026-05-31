#include <iostream>
#include <cassert>
#include "../include/Produs.h"
#include "../include/Depozit.h"

void testAdaugareStocProdus() {
    Produs p(1, "Test Produs", 10, 5, 0.0, 0.0, 0.0);
    p += 15;
    assert(p.getCantitate() == 25);
    std::cout << "[OK] testAdaugareStocProdus a trecut!\n";
}

void testSecuritateIdDuplicat() {
    Depozit d;
    d.adaugaProdus(Produs(1, "Produs 1", 10, 5, 0.0, 0.0, 0.0));
    
    bool exceptiePrinsa = false;
    try {
        d.adaugaProdus(Produs(1, "Produs Hack", 5, 1, 0.0, 0.0, 0.0)); 
    } catch (const std::exception& e) {
        exceptiePrinsa = true;
    }
    
    assert(exceptiePrinsa == true);
    std::cout << "[OK] testSecuritateIdDuplicat a trecut!\n";
}

int main() {
    std::cout << "=== RULARE TESTE AUTOMATE ===\n";
    
    testAdaugareStocProdus();
    testSecuritateIdDuplicat();
    
    std::cout << "\nToate testele au trecut cu succes! Codul este robust.\n";
    std::cout << "=============================\n";
    return 0;
}