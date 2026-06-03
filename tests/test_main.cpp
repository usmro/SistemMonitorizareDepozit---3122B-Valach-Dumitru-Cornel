#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include "../include/Produs.h"
#include "../include/Depozit.h"

int teste_rulate = 0;
int teste_picate = 0;

const std::string VERDE = "\033[1;32m";
const std::string ROSU = "\033[1;31m";
const std::string RESET = "\033[0m";

#define RUN_TEST(TestFunc) \
    try { \
        teste_rulate++; \
        std::cout << "Rulare " << #TestFunc << "... "; \
        TestFunc(); \
        std::cout << VERDE << "[PASSED]" << RESET << "\n"; \
    } catch (const std::exception& e) { \
        teste_picate++; \
        std::cout << ROSU << "[FAILED]" << RESET << "\n"; \
        std::cout << "    -> Motiv: " << e.what() << "\n"; \
    }

// SUITA DE TESTE: DOMENIU (Logica din memorie)
void testOperatoriProdus() {
    Produs p(1, "Test Produs", 10, 5, 0.0, 0.0, 0.5);
    p += 15;
    if (p.getCantitate() != 25) throw std::runtime_error("Operatorul += nu aduna corect stocul.");
    
    p -= 5;
    if (p.getCantitate() != 20) throw std::runtime_error("Operatorul -= nu scade corect stocul.");
}

// SUITA DE TESTE: PERSISTENTA SI BAZA DE DATE
void testGenerareIdAutomat() {
    Depozit d;
    d.resetareTotala();
    
    int id1 = d.genereazaIdProdusNou();
    d.adaugaProdus(Produs(id1, "Produs A", 10, 5, 0, 0, 0));
    
    int id2 = d.genereazaIdProdusNou();
    
    if (id2 <= id1) {
        throw std::runtime_error("ID-ul automat nu este incrementat corect. A generat: " + std::to_string(id2));
    }
}

void testSecuritateIdDuplicat() {
    Depozit d;
    d.resetareTotala();
    d.adaugaProdus(Produs(1, "Produs Original", 10, 5, 0.0, 0.0, 0.0));
    
    try {
        d.adaugaProdus(Produs(1, "Produs Hack", 5, 1, 0.0, 0.0, 0.0));

        throw std::runtime_error("Sistemul a permis adaugarea unui produs cu ID duplicat.");
    } catch (const std::invalid_argument&) {

    }
}

// SUITA DE TESTE: FLOTA SI MENTENANTA (CMMS)
void testBlocareAutomataCamion() {
    Depozit d;
    d.resetareTotala();
    
    std::string masinaTest = "SV-TEST-01";
    d.adaugaCamionInFlota(masinaTest, 10.0, "Disponibil");
    
    d.finalizeazaCursa(masinaTest);
    d.finalizeazaCursa(masinaTest);
    d.finalizeazaCursa(masinaTest);

    auto masiniInService = d.getCamioaneInService();
    bool gasitInService = false;
    for (const auto& m : masiniInService) {
        if (m == masinaTest) gasitInService = true;
    }
    
    if (!gasitInService) {
        throw std::runtime_error("Vehiculul nu a intrat in status 'Necesita Service' dupa 3 curse.");
    }
}

int main() {
    std::cout << "\n============================================\n";
    std::cout << "   RULARE TESTE AUTOMATE: ERP SYSTEM\n";
    std::cout << "============================================\n\n";
    
    RUN_TEST(testOperatoriProdus);
    RUN_TEST(testGenerareIdAutomat);
    RUN_TEST(testSecuritateIdDuplicat);
    RUN_TEST(testBlocareAutomataCamion);
    
    std::cout << "\n============================================\n";
    std::cout << "Rezultat final: " << (teste_rulate - teste_picate) << " / " << teste_rulate << " teste trecute.\n";
    
    if (teste_picate > 0) {
        std::cout << ROSU << "[!] Exista probleme in cod care trebuie rezolvate!" << RESET << "\n\n";
        return 1;
    } else {
        std::cout << VERDE << "[OK] Toate sistemele functioneaza in parametri optimi." << RESET << "\n\n";
        return 0;
    }
}