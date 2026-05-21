#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "Depozit.h"
#include "Tranzactie.h"

using namespace ftxui;

int main() {
    Depozit depozit;

    auto screen = ScreenInteractive::Fullscreen();

    depozit.adaugaCamionInFlota("SV-12-USV", 7500.0, "Disponibil");
    depozit.adaugaCamionInFlota("SV-04-LAB", 3500.0, "Disponibil");
    depozit.adaugaCamionInFlota("SV-86-CMC", 500.0, "Disponibil");
    // ==========================================
    // STATE-UL APLICAȚIEI (Variabile globale UI)
    // ==========================================
    int tab_index = 0; 
    std::string mesaj_status = "Sistem pornit. Gata pentru operatiuni.";

    std::string add_id, add_nume, add_cant, add_pret, add_prag, add_pret_achiz, add_pret_vanz, add_volum;;
    std::string vanzare_id, vanzare_cant;
    std::string aprov_id, aprov_cant;
    std::string search_query;

    Component input_pret_achiz = Input(&add_pret_achiz, " Pret Achizitie ");
    Component input_pret_vanz = Input(&add_pret_vanz, " Pret Vanzare ");
    Component input_volum = Input(&add_volum, " Volum (Ex: 0.05) ");


    auto test_date = depozit.getProdusePaginat(1, 0);
    // ==========================================
    // 1. PANOU AFISARE STOC (CU PAGINARE)
    // ==========================================
    int pagina_curenta = 0;
    const int PRODUSE_PER_PAGINA = 15;

    auto btn_inapoi = Button("< Pagina Anterioara", [&] { 
        if (pagina_curenta > 0) pagina_curenta--; 
    });
    auto btn_inainte = Button("Pagina Urmatoare >", [&] { 
        pagina_curenta++; 
    });

    auto layout_butoane = Container::Horizontal({ 
        btn_inapoi | flex, 
        btn_inainte | flex 
    });

    auto panou_stoc = Renderer(layout_butoane, [&] {
        auto produse_pagina = depozit.getProdusePaginat(PRODUSE_PER_PAGINA, pagina_curenta * PRODUSE_PER_PAGINA);
        
        std::vector<std::vector<Element>> rows;
        rows.push_back({
            text(" ID ") | bold, 
            text(" Nume Produs ") | bold, 
            text(" Cantitate ") | bold, 
            text(" Pret Achiz. ") | bold, 
            text(" Pret Vanz. ") | bold,
            text(" Volum (m3) ") | bold
        });

        if (produse_pagina.empty()) {
            rows.push_back({text("-"), text("BAZA DE DATE ESTE GOALA"), text("-"), text("-"), text("-")});
        } else {
            for (const auto& p : produse_pagina) {
                auto culoare_stoc = (p->getCantitate() <= p->getPragAlerta()) ? color(Color::Red) : color(Color::White);
                auto culoare_nume = color(Color::White);
                
                if (p->getNume().find("[Electronice]") != std::string::npos) culoare_nume = color(Color::Cyan);
                else if (p->getNume().find("[Perisabile]") != std::string::npos) culoare_nume = color(Color::Yellow);

                std::stringstream ssVolum;
                ssVolum << std::fixed << std::setprecision(3) << p->getVolum();

                rows.push_back({
                    text(" " + std::to_string(p->getId()) + " "),
                    text(" " + p->getNume() + " ") | culoare_nume,
                    text(" " + std::to_string(p->getCantitate()) + " ") | culoare_stoc,
                    text(" " + std::to_string(p->getPretAchizitie()) + " "),
                    text(" " + std::to_string(p->getPretVanzare()) + " "),
                    text(" " + ssVolum.str() + " ") | color(Color::Magenta)
                });
            }
        }

        auto tabel = Table(rows);           
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        return vbox({
            text(" GESTIUNE STOC - PAGINA " + std::to_string(pagina_curenta + 1)) | bold | hcenter,
            separator(),
            tabel.Render() | flex,  
            separator(),
            layout_butoane->Render() | hcenter
        });
    });
    // ==========================================
    // 2. PANOU ADAUGARE
    // ==========================================
    auto btn_add = Button("Confirma Adaugarea", [&] {
        try {
            if(add_nume.empty()) throw std::runtime_error("Numele nu poate fi gol.");
            if(add_volum.empty()) throw std::runtime_error("Volumul nu poate fi gol.");

            depozit.adaugaProdus(Produs(
                std::stoi(add_id), 
                add_nume, 
                std::stoi(add_cant), 
                std::stoi(add_prag), 
                std::stod(add_pret_achiz), 
                std::stod(add_pret_vanz),
                std::stod(add_volum)
            ));

            mesaj_status = "Succes: Produsul '" + add_nume + "' a fost adaugat!";
            
            add_id = add_nume = add_cant = add_prag = add_pret_achiz = add_pret_vanz = add_volum = ""; 
        } catch (const std::exception& e) { 
            mesaj_status = std::string("Eroare: ") + e.what(); 
        }
    });
    auto form_add = Container::Vertical({ 
        Input(&add_id, "ID"), 
        Input(&add_nume, "Nume"), 
        Input(&add_cant, "Cantitate"), 
        Input(&add_prag, "Prag"), 
        input_pret_achiz,
        input_pret_vanz, 
        btn_add 
    });
    auto panou_adaugare = Renderer(form_add, [&] {
        return window(text(" ADAUGARE PRODUS NOU "), vbox({ form_add->Render() | size(WIDTH, LESS_THAN, 40) })) | hcenter;
    });

    // ==========================================
    // 3. PANOU VANZARE
    // ==========================================
    std::string input_id_vanzare;
    std::string input_cant_vanzare;
    std::string input_nume_client;
    std::string input_adresa_client;
    std::string mesaj_vanzare = "Asteptare date comanda...";
    
    int masina_selectata = 0;
    std::vector<std::string> lista_masini = {"Fara Transport (Ridicare personala)"};

    // Funcție lambda care reîncarcă mașinile din DB pentru a avea lista la zi
    auto refresh_masini = [&]() {
        lista_masini = {"Fara Transport (Ridicare personala)"};
        auto disponibile = depozit.getCamioaneDisponibile();
        for(const auto& m : disponibile) {
            lista_masini.push_back(m);
        }
        masina_selectata = 0; // Resetăm selecția la prima opțiune
    };
    
    // Apelăm o dată la pornire pentru a popula lista
    refresh_masini();

    // Crearea componentelor vizuale
    Component input_id = Input(&input_id_vanzare, " ID Produs (Ex: 1) ");
    Component input_cant = Input(&input_cant_vanzare, " Cantitate (Ex: 50) ");
    Component input_client = Input(&input_nume_client, " Nume / Companie Client ");
    Component input_adresa = Input(&input_adresa_client, " Adresa de Livrare ");
    Component dropdown_masini = Dropdown(&lista_masini, &masina_selectata);

    auto btn_vinde = Button("Confirma Comanda & Expedierea", [&] {
        try {
            if(input_id_vanzare.empty() || input_cant_vanzare.empty()) {
                throw std::runtime_error("ID-ul si Cantitatea sunt obligatorii!");
            }

            int id = std::stoi(input_id_vanzare);
            int cant = std::stoi(input_cant_vanzare);
            std::string masina = lista_masini[masina_selectata];
            
            // Apelăm "creierul" pe care l-am scris în Depozit.h
            depozit.proceseazaComandaCompleta(id, cant, input_nume_client, input_adresa_client, masina);
            
            mesaj_vanzare = "SUCCES: Comanda expediata!";
            if (masina != "Fara Transport (Ridicare personala)") {
                mesaj_vanzare = "SUCCES: Vehiculul " + masina + " a plecat in cursa!";
            }
            
            // Curățăm formularul
            input_id_vanzare.clear();
            input_cant_vanzare.clear();
            input_nume_client.clear();
            input_adresa_client.clear();
            
            // Reîmprospătăm mașinile (pentru că cea tocmai plecată trebuie să dispară din listă)
            refresh_masini();
            
        } catch (const std::exception& e) {
            mesaj_vanzare = std::string("EROARE: ") + e.what();
        }
    });

    auto layout_vanzare = Container::Vertical({ 
        input_id, 
        input_cant, 
        input_client, 
        input_adresa, 
        dropdown_masini, 
        btn_vinde 
    });

    auto panou_vanzare = Renderer(layout_vanzare, [&] {
        auto culoar_mesaj = mesaj_vanzare.find("EROARE") != std::string::npos ? color(Color::Red) : color(Color::Green);
        
        return vbox({
            text(" MODUL PROCESARE COMENZI SI LOGISTICA ") | bold | hcenter,
            separator(),
            hbox(text(" Date Produs:   "), input_id->Render() | size(WIDTH, EQUAL, 20), text("  "), input_cant->Render() | flex ) | border,
            hbox(text(" Date Client:   "), input_client->Render() | size(WIDTH, EQUAL, 25), text("  "), input_adresa->Render() | flex ) | border,
            hbox(text(" Alocare Flota: "), dropdown_masini->Render() | flex ) | border,
            separator(),
            btn_vinde->Render() | hcenter,
            separator(),
            text(mesaj_vanzare) | bold | culoar_mesaj | hcenter
        });
    });

    // ==========================================
    // 4. PANOU DISPECERAT FLOTA
    // ==========================================
    int index_masina_disp = 0;
    std::vector<std::string> lista_masini_disp = {"- Selecteaza vehicul -"};
    std::string status_dispecerat = "";
    

    auto refresh_dispecerat = [&]() {
        lista_masini_disp = {"- Selecteaza vehicul -"};
        auto masini = depozit.getCamioaneDisponibile();
        for(const auto& m : masini) lista_masini_disp.push_back(m);
        index_masina_disp = 0;
    };
    refresh_dispecerat();

    Component dropdown_disp = Dropdown(&lista_masini_disp, &index_masina_disp);
    
    auto btn_expediaza = Button("Expediaza Vehiculul in Cursa", [&] {
        std::string masina_selectata = lista_masini_disp[index_masina_disp];
        if (masina_selectata != "- Selecteaza vehicul -") {
            bool ok = depozit.declanseazaExpediere(masina_selectata);
            if (ok) {
                status_dispecerat = "SUCCES: Vehiculul " + masina_selectata + " a fost expediat!";
                refresh_dispecerat(); 
            } else {
                status_dispecerat = "EROARE: Nu s-a putut expedia.";
            }
        }
    });

    auto layout_dispecerat = Container::Vertical({ dropdown_disp, btn_expediaza });

    auto panou_dispecerat = Renderer(layout_dispecerat, [&] {
        std::string masina_curenta = lista_masini_disp[index_masina_disp];
        double volum_ocupat = 0.0;
        
        if (masina_curenta != "- Selecteaza vehicul -") {
            volum_ocupat = depozit.verificaIncarcareVehicul(masina_curenta);
        }
        
        std::stringstream ssVol;
        ssVol << std::fixed << std::setprecision(3) << volum_ocupat;
        
        auto culoare_bara = (volum_ocupat > 0.0) ? color(Color::Green) : color(Color::GrayLight);

        return vbox({
            text(" DISPECERAT LOGISTICA ") | bold | hcenter,
            separator(),
            hbox(text(" Flota in curte: "), dropdown_disp->Render() | flex) | border,
            text(" Volum marfa incarcata (AWB-uri in asteptare): " + ssVol.str() + " m3") | bold | culoare_bara | hcenter,
            separator(),
            btn_expediaza->Render() | hcenter,
            separator(),
            text(status_dispecerat) | bold | color(Color::Yellow) | hcenter
        });
    });

    // ==========================================
    // 5. PANOU APROVIZIONARE
    // ==========================================
    auto btn_aprov = Button("Confirma Aprovizionarea", [&] {
        try {
            int id = std::stoi(aprov_id);
            int cant = std::stoi(aprov_cant);
            
            depozit.aprovizioneazaProdus(id, cant);
            
            mesaj_status = "Succes: Stocul produsului " + aprov_id + " a fost suplimentat cu " + aprov_cant;
            aprov_id = aprov_cant = "";
        } catch (const std::exception& e) { 
            mesaj_status = std::string("Eroare: Verifica ID-ul! (") + e.what() + ")"; 
        }
    });

    auto form_aprov = Container::Vertical({ Input(&aprov_id, "ID Produs"), Input(&aprov_cant, "Cantitate adaugata"), btn_aprov });
    auto panou_aprovizionare = Renderer(form_aprov, [&] {
        return window(text(" INTRARE STOC (APROVIZIONARE) "), vbox({ form_aprov->Render() | size(WIDTH, LESS_THAN, 40) })) | hcenter;
    });

    // ==========================================
    // 6. PANOU ALERTE
    // ==========================================
    std::vector<std::unique_ptr<Produs>> produse_critice;
    int pagina_curenta_alerte = 0;
    const int ALERTE_PER_PAGINA = 15;

    auto btn_inapoi_alerte = Button("< Pagina Anterioara", [&] { if (pagina_curenta_alerte > 0) pagina_curenta_alerte--; });
    auto btn_inainte_alerte = Button("Pagina Urmatoare >", [&] { 
        if ((pagina_curenta_alerte + 1) * ALERTE_PER_PAGINA < depozit.getProduseCuStocCritic().size()) pagina_curenta_alerte++; 
    });
    auto layout_nav_alerte = Container::Horizontal({ btn_inapoi_alerte, btn_inainte_alerte });

    auto panou_alerte = Renderer(layout_nav_alerte, [&] {
        auto produse_critice = depozit.getProduseCuStocCritic(); 
        int total = produse_critice.size();

        std::vector<std::vector<Element>> rows;
        rows.push_back({
            text(" ID ")|bold, 
            text(" Nume ")|bold, 
            text(" Tip ")|bold, 
            text(" Stoc ")|bold, 
            text(" Prag ")|bold
        });

        if (total == 0) {
            rows.push_back({text("-"), text("STOCURI OPTIME."), text("-"), text("-"), text("-")});
        } else {
            int start_idx = pagina_curenta_alerte * ALERTE_PER_PAGINA;
            int end_idx = std::min(start_idx + ALERTE_PER_PAGINA, total);

            for (int i = start_idx; i < end_idx; ++i) {
                const auto& p = produse_critice[i];
                rows.push_back({
                    text(" " + std::to_string(p->getId()) + " "), 
                    text(" " + p->getNume() + " "),
                    text(" " + p->getTipProdus() + " ") | color(Color::Yellow),
                    text(" " + std::to_string(p->getCantitate()) + " ") | color(Color::Red) | bold, 
                    text(" " + std::to_string(p->getPragAlerta()) + " ") | color(Color::Cyan)
                });
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        return vbox({
            text("PRODUSE IN STOC CRITIC") | bold | color(Color::Red) | hcenter,
            separator(),
            tabel.Render(),
            separator(),
            layout_nav_alerte->Render() | hcenter,
            filler()
        });
    });

    // ==========================================
    // 7. PANOU CAUTARE
    // ==========================================
    auto input_cautare = Input(&search_query, " Ex: senzor, rulment... ");
    
    auto panou_cautare = Renderer(input_cautare, [&] {
        std::vector<std::vector<Element>> rows;
        rows.push_back({ text(" ID ") | bold, text(" Nume Produs ") | bold, text(" Stoc curent ") | bold });

        if (!search_query.empty()) {
            std::string query_lower = search_query;
            std::transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);

            int afisate = 0;
            for (const auto& p : depozit.getToateProdusele()) {
                std::string nume_lower = p.getNume(); 
                std::transform(nume_lower.begin(), nume_lower.end(), nume_lower.begin(), ::tolower);

                if (nume_lower.find(query_lower) != std::string::npos) {
                    if (afisate >= 15) break; 
                    
                    auto culoare_stoc = (p.getCantitate() == 0) ? color(Color::Red) : color(Color::White);
                    
                    rows.push_back({
                        text(" " + std::to_string(p.getId()) + " "), 
                        text(" " + p.getNume() + " "), 
                        text(" " + std::to_string(p.getCantitate()) + " ") | culoare_stoc
                    });
                    afisate++;
                }
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Yellow));

        return vbox({ 
            text("CAUTARE LIVE IN BAZA DE DATE") | bold | hcenter, 
            separator(), 
            hbox({ text(" Cauta: "), input_cautare->Render() | flex }) | border, 
            separator(), 
            search_query.empty() ? text(" Introduceti text pentru a cauta...") | dim | hcenter 
                                 : (rows.size() == 1 ? text(" Niciun produs gasit.") | color(Color::Red) | hcenter : tabel.Render()),
            filler()
        });
    });

    //==========================================
    // 8. PANOU ISTORIC
    // ==========================================
    
    std::vector<Tranzactie<std::string>> lista_istoric;

    auto panou_istoric = Renderer([&] {
        auto lista_istoric = depozit.getIstoric(); 

        std::vector<std::vector<Element>> rows;
        rows.push_back({
            text(" ID Log ") | bold, 
            text(" ID Produs ") | bold, 
            text(" Operatie ") | bold, 
            text(" Cantitate ") | bold, 
            text(" Pret Unitar ") | bold,
            text(" Valoare Totala ") | bold,
            text(" Data / Ora ") | bold
        });

        auto formateazaBani = [](double valoare) {
            std::string str = std::to_string(valoare);
            return str.substr(0, str.find('.') + 3) + " RON";
        };

        if (lista_istoric.empty()) {
            rows.push_back({text("-"), text("-"), text("Nicio activitate inregistrata."), text("-"), text("-"), text("-"), text("-")});
        } else {
            for (const auto& t : lista_istoric) {
                auto culoare_operatie = (t.getTipOperatie() == "VANZARE") ? color(Color::Red) : color(Color::Green);

                rows.push_back({
                    text(" " + std::to_string(t.getIdLog()) + " "),
                    text(" " + std::to_string(t.getIdProdus()) + " "),
                    text(" " + t.getTipOperatie() + " ") | culoare_operatie | bold,
                    text(" " + std::to_string(t.getCantitate()) + " "),
                    text(" " + formateazaBani(t.getPret()) + " ") | color(Color::Yellow),          // Afișare preț unitar
                    text(" " + formateazaBani(t.getValoareTotala()) + " ") | color(Color::Cyan) | bold, // Afișare valoare totală
                    text(" " + t.getDataOraString() + " ") | color(Color::GrayLight)
                });
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Yellow));

        return vbox({
            text("JURNAL DE AUDIT AUTOMAT") | bold | hcenter,
            separator(),
            tabel.Render(),
            filler()
        });
    });

    // ==========================================
    // 9. PANOU RAPORT FINANCIAR (PROFIT)
    // ==========================================
    auto panou_profit = Renderer([&] {
        double total_investitie = 0.0;
        double total_profit_potential = 0.0;

        auto produse = depozit.getToateProdusele(); 
        
        for(const auto& p : produse) {
            if (p.getCantitate() > 0) {
                double investitie_curenta = p.getPretAchizitie() * p.getCantitate();
                double vanzare_potentiala = p.getPretVanzare() * p.getCantitate();
                
                total_investitie += investitie_curenta;
                total_profit_potential += (vanzare_potentiala - investitie_curenta);
            }
        }

        auto formateazaBani = [](double valoare) {
            std::string str = std::to_string(valoare);
            return str.substr(0, str.find('.') + 3) + " RON";
        };

        std::vector<Produs> top_produse = produse;
        std::sort(top_produse.begin(), top_produse.end(), [](const Produs& a, const Produs& b){
            double profitA = (a.getPretVanzare() - a.getPretAchizitie()) * a.getCantitate();
            double profitB = (b.getPretVanzare() - b.getPretAchizitie()) * b.getCantitate();
            return profitA > profitB;
        });

        std::vector<std::vector<Element>> rows;
        rows.push_back({ text(" ID ") | bold, text(" Nume Produs ") | bold, text(" Profit / Bucata ") | bold, text(" Profit Total Estimat ") | bold });

        int afisate = 0;
        for (const auto& p : top_produse) {
            if (afisate >= 10 || p.getCantitate() == 0) break; 

            double profit_bucata = p.getPretVanzare() - p.getPretAchizitie();
            double profit_total = profit_bucata * p.getCantitate();
            
            rows.push_back({
                text(" " + std::to_string(p.getId()) + " "),
                text(" " + p.getNume() + " "),
                text(" " + formateazaBani(profit_bucata) + " ") | color(Color::Green),
                text(" " + formateazaBani(profit_total) + " ") | color(Color::Green) | bold
            });
            afisate++;
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        auto sumar_financiar = hbox({
            vbox({
                text(" CAPITAL BLOCAT (PE RAFT) ") | dim, 
                text(" " + formateazaBani(total_investitie) + " ") | bold | color(Color::Yellow) | hcenter
            }) | border,
            text("   "),
            
            vbox({
                text(" PROFIT POTENTIAL (ESTIMAT) ") | dim, 
                text(" " + formateazaBani(total_profit_potential) + " ") | bold | color(Color::Cyan) | hcenter
            }) | border,
            text("   "),

            vbox({
                text(" PROFIT REALIZAT (INCASAT) ") | dim, 
                text(" " + formateazaBani(depozit.getProfitRealizat()) + " ") | bold | color(Color::Green) | hcenter
            }) | border,
        }) | hcenter;

        return vbox({
            text("DASHBOARD ANALIZA FINANCIARA") | bold | hcenter,
            separator(),
            sumar_financiar,
            separator(),
            text("TOP 10 PRODUSE CARE GENEREAZA CEL MAI MARE PROFIT") | bold | hcenter,
            tabel.Render(),
            filler()
        });
    });

    // ==========================================
    // ASAMBLAREA INTERFEȚEI
    // ==========================================
   std::vector<std::string> meniu_text = { 
        " 1. Afisare Stoc ", 
        " 2. Adaugare Produs ", 
        " 3. Vanzare ", 
        " 4. Dispecerat",
        " 5. Aprovizionare ", 
        " 6. Raport Alerte ", 
        " 7. Cautare",
        " 8. Jurnal Istoric ",
        " 9. Raport Profit ",
        " 0. Iesire " 
    };
    auto meniu_lateral = Menu(&meniu_text, &tab_index);

    auto tab_container = Container::Tab({ 
        panou_stoc,         
        panou_adaugare,      
        panou_vanzare,  
        panou_dispecerat,     
        panou_aprovizionare,
        panou_alerte,
        panou_cautare,
        panou_istoric,
        panou_profit,
        Button("EXIT", screen.ExitLoopClosure())    
    }, &tab_index);
    
    auto main_container = Container::Horizontal({ meniu_lateral, tab_container });

    auto ecran_final = Renderer(main_container, [&] {
        auto culoare_status = mesaj_status.find("Eroare") != std::string::npos ? color(Color::Red) : color(Color::Green);
        return vbox({
            hbox({
                vbox({ text(" DEPOZIT APP ") | bold | bgcolor(Color::Blue) | color(Color::White), separator(), meniu_lateral->Render() | flex }) | size(WIDTH, EQUAL, 25) | border,
                tab_container->Render() | flex | border
            }) | flex,
            hbox({ text(" Status: ") | bold, text(mesaj_status) | culoare_status }) | border
        });
    });

    screen.Loop(ecran_final);

    return 0;
}