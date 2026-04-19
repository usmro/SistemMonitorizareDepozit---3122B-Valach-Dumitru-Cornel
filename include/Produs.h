#pragma once
#include <string>
#include <iostream>

class Produs {
protected:
    int id;
    std::string nume;
    int cantitate;
    double pret;
    int pragAlerta;

public:
    Produs(int id, std::string nume, int cantitate, double pret, int pragAlerta);
    
    virtual ~Produs() = default; 

    int getId() const;
    std::string getNume() const;
    int getCantitate() const;
    double getPret() const;
    int getPragAlerta() const;

    void setCantitate(int nouaCantitate);
    void setPret(double nouPret);

    Produs& operator+=(int adaugaCantitate);
    Produs& operator-=(int scadeCantitate);

    virtual void afiseazaDetalii() const; 
};