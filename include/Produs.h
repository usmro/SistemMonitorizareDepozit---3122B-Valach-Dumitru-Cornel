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
    // Declarația constructorului
    Produs(int i, std::string n, int c, double p, int pr);
    
    virtual ~Produs() = default;

    // Gettere inline (sunt super rapide, se acceptă în .h)
    int getId() const { return id; }
    Produs& operator+=(int valoare);
    Produs& operator-=(int valoare);
    std::string getNume() const { return nume; }
    int getCantitate() const { return cantitate; }
    double getPret() const { return pret; }
    int getPragAlerta() const { return pragAlerta; }

    // Metode virtuale care vor fi suprascrise
    virtual std::string getTipProdus() const;
    virtual std::string getDetaliiSpecifice() const;
};