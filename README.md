# Sistem de Monitorizare Depozit (C++ / Linux)

Acesta este un sistem modern de gestiune a inventarului, dezvoltat în C++17, optimizat pentru medii Linux/WSL. Proiectul utilizează containere STL avansate și algoritmi eficienți pentru monitorizarea produselor în timp real.

## Caracteristici Principale
- **Gestiune Produse:** Adăugare, eliminare și actualizare stoc.
- **Performanță:** Utilizarea `std::unordered_map` pentru căutări instantanee (O(1)).
- **Sistem de Alerte:** Notificări automate pentru produsele care scad sub pragul de alertă.
- **Programare Defensivă:** Gestionarea erorilor prin excepții (`try-catch`).
- **Standard Profesional:** Structură modulară (Header/Source) și build system cu CMake.

## Tehnologii utilizate
- **Limbaj:** C++17 (Standard modern)
- **Build System:** CMake 3.10+
- **Platformă:** Linux (Testat pe Ubuntu 24.04 via WSL)
- **Control Versiune:** Git (GitHub Organization)

## Structura Proiectului
- `include/`: Fișiere header (.hpp) - Declarațiile claselor.
- `src/`: Fișiere sursă (.cpp) - Implementarea logicii.
- `CMakeLists.txt`: Configurația de build.
- `build/`: Fișierele binare rezultate (exclus din Git).

## Cum se compilează și rulează

Pentru a rula proiectul pe un sistem Linux, urmează pașii de mai jos:

1. **Configurare CMake:**
   ```bash
   cmake -B build
   cmake --build build

```mermaid
classDiagram
    Depozit *-- DatabaseManager : "Utilizeaza (Compozitie)"
    Depozit o-- Produs : "Gestioneaza (Agregare)"
    DatabaseManager ..> Tranzactie : "Creeaza"
    DatabaseManager ..> InregistrareService : "Creeaza"
    
    Produs <|-- ProdusElectronic : "Mosteneste"
    Produs <|-- ProdusPerisabil : "Mosteneste"
    Produs <|-- ProdusFragil : "Mosteneste"
    Produs <|-- ProdusPericulos : "Mosteneste"
    Produs <|-- ProdusVoluminos : "Mosteneste"

    class InregistrareService {
        <<struct>>
        +String idCamion
        +String data
        +String tipInterventie
    }

    class Tranzactie~T~ {
        -int idLog
        -int idProdus
        -int cantitate
        -String tipOperatie
        -time_t dataOra
        -double pretUnitar
        +getValoareTotala() double
        +getDataOraString() String
    }

    class Produs {
        #int id
        #String nume
        #int cantitate
        #int pragAlerta
        #double pretAchizitie
        #double pretVanzare
        #double volumM3
        +getId() int
        +getNume() String
        +getVolumM3() double
        +operator-=(int)
        +operator+=(int)
    }

    class ProdusElectronic {
        -int luniGarantie
        -double tensiuneAlimentare
        +getTipProdus() string
        +getDetalii() String
    }

    class ProdusPerisabil {
        -int zileValabilitate
        -double temperaturaPastrare
        +getTipProdus() string
        +getDetalii() String
    }

    class ProdusFragil{
        -int maxStivuire
        +getTipProdus() string
        +getDetalii() String
    }

    class ProdusPericulos{
        -string clasaRisc
        +getTipProdus() string
        +getDetalii() String
    }

    class ProdusVoluminos{
        -double greutateKg
        -bool necesitaMotostivuitor
        +getTipProdus() string
        +getDetalii() String
    }

    class DatabaseManager {
        -sqlite3* db
        +DatabaseManager(String dbPath)
        +creeazaTabele() void
        +getUrmatorulIdProdus() int
        +salveazaProdus(Produs p) void
        +getProdusePaginat(int, int) List~Produs~
        +salveazaComanda(AWB, id, cant, masina) bool
        +getGradIncarcare(idCamion) double
        +finalizeazaCursa(idCamion) bool
        +efectueazaRevizie(idCamion, tip) bool
    }

    class Depozit {
        -DatabaseManager dbManager
        -unordered_map~int, Produs~ stoc
        +Depozit()
        +genereazaIdProdusNou() int
        +adaugaProdus(Produs p) void
        +vindeProdus(id, cantitate) void
        +proceseazaComandaCompleta() String
        +declanseazaExpediere(idCamion) bool
        +verificaIncarcareVehicul(idCamion) double
        +getIstoricService() List~InregistrareService~
    }
