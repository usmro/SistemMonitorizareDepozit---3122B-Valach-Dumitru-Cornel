#include "DatabaseManager.h"
#include "ProdusElectronic.h"
#include "ProdusPerisabil.h"
#include "ProdusFragil.h"
#include "ProdusVoluminos.h"
#include "ProdusPericulos.h"
#include <stdexcept>

DatabaseManager::DatabaseManager(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Nu s-a putut deschide baza de date!");
    }
    creeazaTabele();
}

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
        "Status TEXT);";
        
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}

void DatabaseManager::salveazaProdus(const Produs& p) {
    std::string sql = "INSERT OR REPLACE INTO Produse (ID, Nume, Cantitate, Pret, PragAlerta) VALUES (" +
        std::to_string(p.getId()) + ", '" + p.getNume() + "', " +
        std::to_string(p.getCantitate()) + ", " + std::to_string(p.getPret()) + ", " +
        std::to_string(p.getPragAlerta()) + ");";
        
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}

std::vector<Produs> DatabaseManager::incarcaProduse() {
    std::vector<Produs> lista;
    const char* sql = "SELECT ID, Nume, Cantitate, Pret, PragAlerta FROM Produse;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int cantitate = sqlite3_column_int(stmt, 2);
            double pret = sqlite3_column_double(stmt, 3);
            int prag = sqlite3_column_int(stmt, 4);

            lista.push_back(Produs(id, nume, cantitate, pret, prag));
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

    const char* sql = "INSERT OR REPLACE INTO Produse (ID, Nume, Cantitate, Pret, PragAlerta) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    int count = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, tip, nume, cant_str, pret_str, prag_str;

        std::getline(ss, id_str, ',');
        std::getline(ss, tip, ',');
        std::getline(ss, nume, ',');
        std::getline(ss, cant_str, ',');
        std::getline(ss, pret_str, ',');
        std::getline(ss, prag_str, ',');

        sqlite3_bind_int(stmt, 1, std::stoi(id_str));
        sqlite3_bind_text(stmt, 2, nume.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, std::stoi(cant_str));
        sqlite3_bind_double(stmt, 4, std::stod(pret_str));
        sqlite3_bind_int(stmt, 5, std::stoi(prag_str));

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
        count++;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    std::cout << "Import complet! S-au incarcat " << count << " produse in baza de date.\n";
}

std::vector<std::unique_ptr<Produs>> DatabaseManager::getProdusePaginat(int limita, int offset) {
    std::vector<std::unique_ptr<Produs>> lista;
    std::string sql = "SELECT ID, Nume, Cantitate, Pret, PragAlerta FROM Produse ORDER BY ID ASC LIMIT " + 
                      std::to_string(limita) + " OFFSET " + std::to_string(offset) + ";";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int cantitate = sqlite3_column_int(stmt, 2);
            double pret = sqlite3_column_double(stmt, 3);
            int prag = sqlite3_column_int(stmt, 4);

            if (nume.find("[Electronice]") != std::string::npos || nume.find("[Electrocasnice]") != std::string::npos) {
                lista.push_back(std::make_unique<ProdusElectronic>(id, nume, cantitate, pret, prag, 24, 220.0));
            } 
            else if (nume.find("[Perisabile]") != std::string::npos) {
                lista.push_back(std::make_unique<ProdusPerisabil>(id, nume, cantitate, pret, prag, 4, 70));
            } 
            else {
                lista.push_back(std::make_unique<Produs>(id, nume, cantitate, pret, prag));
            }
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

std::vector<std::unique_ptr<Produs>> DatabaseManager::getProduseCuStocCritic() {
    std::vector<std::unique_ptr<Produs>> lista;
    std::string sql = "SELECT ID, Nume, Cantitate, Pret, PragAlerta FROM Produse WHERE Cantitate <= PragAlerta;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nume = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int cantitate = sqlite3_column_int(stmt, 2);
            double pret = sqlite3_column_double(stmt, 3);
            int prag = sqlite3_column_int(stmt, 4);

            lista.push_back(std::make_unique<Produs>(id, nume, cantitate, pret, prag)); 
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
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    sqlite3_finalize(stmt);
    return success;
}