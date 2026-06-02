#include "DatabaseManager.h"
#include "Produs.h"
#include "ProdusElectronic.h"
#include "ProdusPerisabil.h"
#include "ProdusFragil.h"
#include "ProdusVoluminos.h"
#include "ProdusPericulos.h"
#include "Tranzactie.h"
#include <stdexcept>
#include <iomanip>

DatabaseManager::DatabaseManager(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Nu s-a putut deschide baza de date!");
    }
    creeazaTabele();
    creeazaTabelIstoric();
    creeazaTabelComenzi();
    creeazaTabelMentenanta();
    creeazaTabelFurnizori();

    sqlite3_exec(db, "ALTER TABLE Camioane ADD COLUMN NumarCurse INTEGER DEFAULT 0;", nullptr, nullptr, nullptr);
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}

// creare tabele
void DatabaseManager::creeazaTabele() {
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS Produse ("
        "ID INTEGER PRIMARY KEY, "
        "Nume TEXT NOT NULL, "
        "Cantitate INTEGER, "
        "Pret REAL, "
        "PragAlerta INTEGER);"
        
        "CREATE TABLE IF NOT EXISTS Staff ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Nume TEXT, "
        "Rol TEXT);"
        
        "CREATE TABLE IF NOT EXISTS Camioane ("
        "ID TEXT PRIMARY KEY, "
        "Capacitate REAL, "
        "Status TEXT,"
        "NumarCurse INTEGER DEFAULT 0);";
        
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);

    sqlite3_exec(db, "ALTER TABLE Produse ADD COLUMN PretAchizitie REAL DEFAULT 0.0;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE Produse ADD COLUMN PretVanzare REAL DEFAULT 0.0;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE Produse ADD COLUMN Volum REAL DEFAULT 0.0;", nullptr, nullptr, nullptr);
}

void DatabaseManager::creeazaTabelIstoric() {
    std::string sql = "CREATE TABLE IF NOT EXISTS IstoricTranzactii ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "ID_Produs INTEGER, "
                      "Tip TEXT, "
                      "Cantitate INTEGER, "
                      "DataHora INTEGER);";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseManager::creeazaTabelComenzi() {
    std::string sql = "CREATE TABLE IF NOT EXISTS Comenzi ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "AWB TEXT, "
                      "ID_Produs INTEGER, "
                      "Cantitate INTEGER, "
                      "NumeClient TEXT, "
                      "AdresaLivrare TEXT, "
                      "ID_Camion TEXT, "
                      "StatusComanda TEXT, "
                      "DataComanda INTEGER);";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseManager::creeazaTabelMentenanta() {
    std::string sql = "CREATE TABLE IF NOT EXISTS IstoricService ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "ID_Camion TEXT, "
                      "DataRevizie INTEGER, "
                      "TipInterventie TEXT);";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DatabaseManager::creeazaTabelFurnizori() {
    std::string sql = "CREATE TABLE IF NOT EXISTS Furnizori ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "NumeCompanie TEXT NOT NULL, "
                      "Contact TEXT);";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

// golire baza de date (debug)
void DatabaseManager::golesteBazaDeDate() {
    sqlite3_exec(db, "DROP TABLE IF EXISTS Produse;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DROP TABLE IF EXISTS IstoricTranzactii;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DROP TABLE IF EXISTS Comenzi;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DROP TABLE IF EXISTS Camioane;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "DROP TABLE IF EXISTS IstoricService;", nullptr, nullptr, nullptr);
    
    creeazaTabele();
    creeazaTabelIstoric();
    creeazaTabelComenzi();
    creeazaTabelMentenanta();
}

int DatabaseManager::getNumarTotalTranzactii() {
    int count = 0;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM IstoricTranzactii;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;
}

void DatabaseManager::salveazaProdus(const Produs& p) {
    std::string sql = "INSERT OR REPLACE INTO Produse (ID, Nume, Cantitate, PragAlerta, PretAchizitie, PretVanzare, Volum) VALUES (" +
        std::to_string(p.getId()) + ", '" + p.getNume() + "', " +
        std::to_string(p.getCantitate()) + ", " + 
        std::to_string(p.getPragAlerta()) + ", " +
        std::to_string(p.getPretAchizitie()) + ", " + 
        std::to_string(p.getPretVanzare()) + ", " +
        std::to_string(p.getVolum()) + ");";
        
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

std::vector<Produs> DatabaseManager::incarcaProduse() {
    std::vector<Produs> lista;
    const char* sql = "SELECT ID, Nume, Cantitate, PragAlerta, PretAchizitie, PretVanzare, Volum FROM Produse;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int cantitate = sqlite3_column_int(stmt, 2);
            int prag = sqlite3_column_int(stmt, 3);
            double pretAchiz = sqlite3_column_double(stmt, 4);
            double pretVanz = sqlite3_column_double(stmt, 5);
            double volum = sqlite3_column_double(stmt, 6);

            lista.push_back(Produs(id, nume, cantitate, prag, pretAchiz, pretVanz, volum));
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

void DatabaseManager::importaMasivDinCSV(const std::string& numeFisier) {
    std::ifstream file(numeFisier);
    if (!file.is_open()) {
        std::cerr << "Nu s-a putut deschide " << numeFisier << "\n";
        return;
    }

    std::string line;
    std::getline(file, line); 

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char* sql = "INSERT OR REPLACE INTO Produse (ID, Nume, Cantitate, PragAlerta, PretAchizitie, PretVanzare, Volum) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    int count = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, tip, nume, cant_str, prag_str, pret_achiz_str, pret_vanz_str, volum_str;
        
        std::getline(ss, id_str, ',');
        std::getline(ss, tip, ',');
        std::getline(ss, nume, ',');
        std::getline(ss, cant_str, ',');
        std::getline(ss, prag_str, ',');
        std::getline(ss, pret_achiz_str, ',');
        std::getline(ss, pret_vanz_str, ',');
        std::getline(ss, volum_str, ',');

        if (id_str.empty() || pret_vanz_str.empty()) continue; 

        sqlite3_bind_int(stmt, 1, std::stoi(id_str));
        sqlite3_bind_text(stmt, 2, nume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, std::stoi(cant_str));
        sqlite3_bind_int(stmt, 4, std::stoi(prag_str));
        sqlite3_bind_double(stmt, 5, std::stod(pret_achiz_str));
        sqlite3_bind_double(stmt, 6, std::stod(pret_vanz_str));
        sqlite3_bind_double(stmt, 7, std::stod(volum_str));

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
        count++;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    std::cout << "Import complet! S-au incarcat/actualizat " << count << " produse in baza de date.\n";
}

std::vector<std::unique_ptr<Produs>> DatabaseManager::getProdusePaginat(int limita, int offset) {
    std::vector<std::unique_ptr<Produs>> lista;
    std::string sql = "SELECT ID, Nume, Cantitate, PragAlerta, PretAchizitie, PretVanzare, Volum FROM Produse ORDER BY ID ASC LIMIT " + 
                      std::to_string(limita) + " OFFSET " + std::to_string(offset) + ";";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int cantitate = sqlite3_column_int(stmt, 2);
            int prag = sqlite3_column_int(stmt, 3);
            double pretAchiz = sqlite3_column_double(stmt, 4);
            double pretVanz = sqlite3_column_double(stmt, 5);
            double volum = sqlite3_column_double(stmt, 6);
            
            if (nume.find("[Electronice]") != std::string::npos || nume.find("[Electrocasnice]") != std::string::npos) {
                lista.push_back(std::make_unique<ProdusElectronic>(id, nume, cantitate, prag, pretAchiz, pretVanz, volum, 24, 220.0));
            } 
            else if (nume.find("[Perisabile]") != std::string::npos) {
                lista.push_back(std::make_unique<ProdusPerisabil>(id, nume, cantitate, prag, pretAchiz, pretVanz, volum, 4, 70));
            } 
            else {
                lista.push_back(std::make_unique<Produs>(id, nume, cantitate, prag, pretAchiz, pretVanz, volum));
            }
        }
    } else {
        std::cerr << "Eroare la pregatirea SQL (Paginare): " << sqlite3_errmsg(db) << "\n";
    }
    sqlite3_finalize(stmt);
    return lista;
}

std::vector<std::unique_ptr<Produs>> DatabaseManager::getProduseCuStocCritic() {
    std::vector<std::unique_ptr<Produs>> lista;
    std::string sql = "SELECT ID, Nume, Cantitate, PragAlerta, PretAchizitie, PretVanzare, Volum FROM Produse WHERE Cantitate <= PragAlerta;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int cantitate = sqlite3_column_int(stmt, 2);
            int prag = sqlite3_column_int(stmt, 3);
            double pretAchiz = sqlite3_column_double(stmt, 4);
            double pretVanz = sqlite3_column_double(stmt, 5);
            double volum = sqlite3_column_double(stmt, 6);

            lista.push_back(std::make_unique<Produs>(id, nume, cantitate, prag, pretAchiz, pretVanz, volum)); 
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

bool DatabaseManager::actualizeazaStocInDB(int id, int nouaCantitate) {
    std::string sql = "UPDATE Produse SET Cantitate = ? WHERE ID = ?;";
    sqlite3_stmt* stmt;
    bool success = false;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, nouaCantitate);
        sqlite3_bind_int(stmt, 2, id);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
    }
    sqlite3_finalize(stmt);
    return success;
}

// bool DatabaseManager::actualizeazaPreturiProdus(int id, double nouAchiz, double nouVanz) {
//     std::string sql = "UPDATE Produse SET PretAchizitie = ?, PretVanzare = ? WHERE ID = ?;";
//     sqlite3_stmt* stmt;
//     bool success = false;
    
//     if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
//         sqlite3_bind_double(stmt, 1, nouAchiz);
//         sqlite3_bind_double(stmt, 2, nouVanz);
//         sqlite3_bind_int(stmt, 3, id);
        
//         if (sqlite3_step(stmt) == SQLITE_DONE) success = true;
//     }
//     sqlite3_finalize(stmt);
//     return success;
// }

bool DatabaseManager::adaugaInIstoric(int idProdus, const std::string& tip, int cantitate) {
    std::string sql = "INSERT INTO IstoricTranzactii (ID_Produs, Tip, Cantitate, DataHora) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    bool succes = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        auto acum = std::chrono::system_clock::now();
        std::time_t timp = std::chrono::system_clock::to_time_t(acum);

        sqlite3_bind_int(stmt, 1, idProdus);
        sqlite3_bind_text(stmt, 2, tip.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, cantitate);
        sqlite3_bind_int64(stmt, 4, timp);
        if (sqlite3_step(stmt) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmt);
    return succes;
}

std::vector<Tranzactie<std::string>> DatabaseManager::getIstoricTranzactii() {
    std::vector<Tranzactie<std::string>> istoric;
    std::string sql = "SELECT I.ID, I.ID_Produs, I.Tip, I.Cantitate, I.DataHora, "
                      "CASE WHEN I.Tip = 'VANZARE' THEN P.PretVanzare ELSE P.PretAchizitie END AS PretAplicat "
                      "FROM IstoricTranzactii I "
                      "LEFT JOIN Produse P ON I.ID_Produs = P.ID "
                      "ORDER BY I.ID DESC LIMIT 15;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int idL = sqlite3_column_int(stmt, 0);
            int idP = sqlite3_column_int(stmt, 1);
            std::string tip = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            int cant = sqlite3_column_int(stmt, 3);
            std::time_t timp = static_cast<std::time_t>(sqlite3_column_int64(stmt, 4));
            double pret = sqlite3_column_double(stmt, 5);
            
            istoric.push_back(Tranzactie<std::string>(idL, idP, cant, tip, timp, pret));
        }
    }
    sqlite3_finalize(stmt);
    return istoric;
}

double DatabaseManager::getProfitRealizat() {
    double profit = 0.0;
    std::string sql = "SELECT SUM(I.Cantitate * (P.PretVanzare - P.PretAchizitie)) "
                      "FROM IstoricTranzactii I "
                      "JOIN Produse P ON I.ID_Produs = P.ID "
                      "WHERE I.Tip = 'VANZARE';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) profit = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return profit;
}


bool DatabaseManager::adaugaCamion(const std::string& id, double capacitate, const std::string& status) {
    std::string sql = "INSERT OR REPLACE INTO Camioane (ID, Capacitate, Status) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    bool succes = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 2, capacitate);
        sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmt);
    return succes;
}
double DatabaseManager::getCapacitateCamion(const std::string& idCamion) {
    double capacitate = 0.0;
    std::string sql = "SELECT Capacitate FROM Camioane WHERE ID = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            capacitate = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return capacitate;
}
std::vector<std::pair<std::string, std::string>> DatabaseManager::getToateCamioanele() {
    std::vector<std::pair<std::string, std::string>> lista;
    const char* sql = "SELECT ID, Status FROM Camioane;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            lista.push_back({id, status});
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

std::vector<std::string> DatabaseManager::getCamioaneDisponibile() {
    std::vector<std::string> lista;
    const char* sql = "SELECT ID FROM Camioane WHERE Status = 'Disponibil';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            lista.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

std::vector<std::string> DatabaseManager::getCamioaneInCursa() {
    std::vector<std::string> lista;
    const char* sql = "SELECT ID FROM Camioane WHERE Status = 'In Cursa';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            lista.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

std::vector<std::string> DatabaseManager::getCamioaneInService() {
    std::vector<std::string> lista;
    const char* sql = "SELECT ID FROM Camioane WHERE Status = 'Necesita Service';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            lista.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

bool DatabaseManager::salveazaComanda(const std::string& awb, int idProdus, int cantitate, const std::string& client, const std::string& adresa, const std::string& idCamion) {
    std::string sql = "INSERT INTO Comenzi (AWB, ID_Produs, Cantitate, NumeClient, AdresaLivrare, ID_Camion, StatusComanda, DataComanda) "
                      "VALUES (?, ?, ?, ?, ?, ?, 'In Pregatire', ?);";
    sqlite3_stmt* stmt;
    bool succes = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        auto acum = std::chrono::system_clock::now();
        std::time_t timp = std::chrono::system_clock::to_time_t(acum);

        sqlite3_bind_text(stmt, 1, awb.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, idProdus);
        sqlite3_bind_int(stmt, 3, cantitate);
        sqlite3_bind_text(stmt, 4, client.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, adresa.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 7, timp);

        if (sqlite3_step(stmt) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmt);
    return succes;
}

bool DatabaseManager::actualizeazaStatusCamion(const std::string& idCamion, const std::string& noulStatus) {
    std::string sql = "UPDATE Camioane SET Status = ? WHERE ID = ?;";
    sqlite3_stmt* stmt;
    bool succes = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, noulStatus.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, idCamion.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmt);
    return succes;
}

// afisare spatiu autoturism
double DatabaseManager::getGradIncarcare(const std::string& idCamion) {
    double volumTotal = 0.0;
    
    std::string sql = "SELECT SUM(C.Cantitate * P.Volum) FROM Comenzi C "
                      "JOIN Produse P ON C.ID_Produs = P.ID "
                      "WHERE C.ID_Camion = ? AND C.StatusComanda = 'In Pregatire';";
                      
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            volumTotal = sqlite3_column_double(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return volumTotal;
}

bool DatabaseManager::expediazaCamion(const std::string& idCamion) {
    bool succes = false;
    std::string sqlMasina = "UPDATE Camioane SET Status = 'In Cursa' WHERE ID = ?;";
    std::string sqlComenzi = "UPDATE Comenzi SET StatusComanda = 'Expediat' WHERE ID_Camion = ? AND StatusComanda = 'In Pregatire';";
    
    sqlite3_stmt* stmtM;
    sqlite3_stmt* stmtC;

    if (sqlite3_prepare_v2(db, sqlMasina.c_str(), -1, &stmtM, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmtM, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmtM) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmtM);

    if (succes && sqlite3_prepare_v2(db, sqlComenzi.c_str(), -1, &stmtC, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmtC, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmtC);
    }
    sqlite3_finalize(stmtC);
    
    return succes;
}

bool DatabaseManager::finalizeazaCursa(const std::string& idCamion) {
    std::string sql = "UPDATE Camioane SET NumarCurse = NumarCurse + 1, "
                      "Status = CASE WHEN NumarCurse + 1 >= 3 THEN 'Necesita Service' ELSE 'Disponibil' END "
                      "WHERE ID = ?;";
    sqlite3_stmt* stmt;
    bool succes = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmt);
    return succes;
}

bool DatabaseManager::efectueazaRevizie(const std::string& idCamion, const std::string& tipInterventie) {
    std::string sql = "UPDATE Camioane SET Status = 'Disponibil', NumarCurse = 0 WHERE ID = ?;";
    sqlite3_stmt* stmt;
    bool succes = false;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) succes = true;
    }
    sqlite3_finalize(stmt);

    if (succes) {
        std::string sqlIstoric = "INSERT INTO IstoricService (ID_Camion, DataRevizie, TipInterventie) VALUES (?, ?, ?);";
        sqlite3_stmt* stmtIst;
        if (sqlite3_prepare_v2(db, sqlIstoric.c_str(), -1, &stmtIst, nullptr) == SQLITE_OK) {
            auto acum = std::chrono::system_clock::now();
            std::time_t timp = std::chrono::system_clock::to_time_t(acum);
            
            sqlite3_bind_text(stmtIst, 1, idCamion.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmtIst, 2, timp);
            sqlite3_bind_text(stmtIst, 3, tipInterventie.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmtIst);
        }
        sqlite3_finalize(stmtIst);
    }
    return succes;
}

std::vector<InregistrareService> DatabaseManager::getIstoricService() {
    std::vector<InregistrareService> istoric;
    const char* sql = "SELECT ID_Camion, DataRevizie, TipInterventie FROM IstoricService ORDER BY ID DESC LIMIT 8;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "[EROARE SQL MENTENANTA]: " << sqlite3_errmsg(db) << "\n";
        return istoric;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        InregistrareService rec;
        rec.idCamion = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::time_t timp = sqlite3_column_int64(stmt, 1);
        
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M", std::localtime(&timp));
        rec.data = buffer;
        
        rec.tipInterventie = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        istoric.push_back(rec);
    }
    
    sqlite3_finalize(stmt);
    return istoric;
}

int DatabaseManager::getUrmatorulIdProdus() {
    int nextId = 1;
    const char* sql = "SELECT MAX(ID) FROM Produse;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
                nextId = sqlite3_column_int(stmt, 0) + 1;
            }
        }
    }
    sqlite3_finalize(stmt);
    return nextId;
}

bool DatabaseManager::salveazaFurnizor(const Furnizor& f) {
    // Nu introducem ID-ul in INSERT, lasam SQLite sa il genereze automat (AUTOINCREMENT)
    std::string sql = "INSERT INTO Furnizori (NumeCompanie, Contact) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, f.getNume().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, f.getContact().c_str(), -1, SQLITE_TRANSIENT);
        
        bool succes = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return succes;
    }
    return false;
}

std::vector<Furnizor> DatabaseManager::getFurnizori() {
    std::vector<Furnizor> lista;
    std::string sql = "SELECT ID, NumeCompanie, Contact FROM Furnizori;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            
            lista.push_back(Furnizor(id, nume, contact));
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}
