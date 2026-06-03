#pragma once
#include <string>
#include <vector>

class Furnizor {
private:
    int idFurnizor;
    std::string numeCompanie;
    std::string contact;
    std::vector<int> idProduseFurnizate;
public:
    Furnizor(int id, std::string nume, std::string tel) 
        : idFurnizor(id), numeCompanie(nume), contact(tel) {}

    void asociazaProdus(int idProdus) {
        idProduseFurnizate.push_back(idProdus);
    }
    
    int getId() const { return idFurnizor; }
    std::string getNume() const { return numeCompanie; }
    std::string getContact() const { return contact; }
};