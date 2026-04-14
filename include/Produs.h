#pragma once
#include <string>

class Produs {
private:
    int id;
    std::string nume;
    int cantitate;
    double pret;
    int pragAlerta;

public:
    Produs(int id, std::string nume, int cantitate, double pret, int pragAlerta);

    int getId() const;
    std::string getNume() const;
    int getCantitate() const;
    double getPret() const;
    int getPragAlerta() const;

    void setCantitate(int nouaCantitate);
    void setPret(double nouPret);

    Produs& operator+=(int adaugaCantitate);
    Produs& operator-=(int scadeCantitate); 
};