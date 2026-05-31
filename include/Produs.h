#pragma once
#include <string>
#include <iostream>

class Produs {
protected:
    int id;
    std::string nume;
    int cantitate;
    int pragAlerta;
    double pretAchizitie;
    double pretVanzare;
    double volum;

public:
    Produs(int i, std::string n, int c, int pr, double pretAchiz, double pretVanz, double volum);
    
    virtual ~Produs() = default;

    int getId() const { return id; }
    Produs& operator+=(int valoare);
    Produs& operator-=(int valoare);
    std::string getNume() const { return nume; }
    int getCantitate() const { return cantitate; }
    int getPragAlerta() const { return pragAlerta; }

    virtual std::string getTipProdus() const;
    virtual std::string getDetaliiSpecifice() const;

    double getPretAchizitie() const { return pretAchizitie; }
    double getPretVanzare() const { return pretVanzare; }
    double getProfitPerBucata() const { return pretVanzare - pretAchizitie; }
    double getVolum() const { return volum; }
};