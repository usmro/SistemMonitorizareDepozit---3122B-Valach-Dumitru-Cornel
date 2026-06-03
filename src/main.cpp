#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <algorithm>

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

    depozit.adaugaCamionInFlota("SV-11-DEL", 3.5, "Disponibil");
    depozit.adaugaCamionInFlota("SV-12-USV", 4.2, "Disponibil");
    depozit.adaugaCamionInFlota("SV-13-WMS", 5.0, "Disponibil");

    depozit.adaugaCamionInFlota("SV-04-LAB", 12.5, "Disponibil");
    depozit.adaugaCamionInFlota("SV-45-LOG", 15.0, "Disponibil");
    depozit.adaugaCamionInFlota("SV-46-LOG", 18.5, "Disponibil");

    depozit.adaugaCamionInFlota("SV-86-CMC", 40.0, "Disponibil");
    depozit.adaugaCamionInFlota("SV-91-TIR", 65.0, "Disponibil");
    depozit.adaugaCamionInFlota("SV-92-TIR", 80.0, "Disponibil");

    // ==========================================
    // STATE-UL APLICAȚIEI (Variabile globale UI)
    // ==========================================
    int tab_index = 0; 
    std::string mesaj_status = "Sistem pornit. Gata pentru operatiuni.";

    std::string add_id, add_nume, add_cant, add_pret_achiz, add_pret_vanz, add_prag, add_volum;
    std::string vanzare_id, vanzare_cant;
    std::string aprov_id, aprov_cant;
    std::string search_query;

    Component input_pret_achiz = Input(&add_pret_achiz, "Pret Achizitie ");
    Component input_pret_vanz = Input(&add_pret_vanz, "Pret Vanzare ");
    Component input_volum = Input(&add_volum, "Volum (Ex: 0.05) ");

    std::string input_nume_furnizor = "";
    std::string input_contact_furnizor = "";
    std::string eroare_furnizor = "";
    std::string input_id_stergere_f = "";

    Component camp_id_stergere_f = Input(&input_id_stergere_f, "Ex: 3");
    Component camp_nume_f = Input(&input_nume_furnizor, "Ex: SC Componente SRL");
    Component camp_contact_f = Input(&input_contact_furnizor, "Ex: 0722123456 sau email");

    auto render_eroare = [](const std::string& mesaj) {
        if (mesaj.empty()) {
            return text("") | flex; 
        }
        return hbox({
            text(" [!] EROARE: " + mesaj + " ") | color(Color::Red) | bold
        }) | border | hcenter;
    };

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
        btn_inapoi, 
        btn_inainte 
    });

    auto panou_stoc = Renderer(layout_butoane, [&] {
        auto produse_pagina = depozit.getProdusePaginat(PRODUSE_PER_PAGINA, pagina_curenta * PRODUSE_PER_PAGINA);
        
        std::vector<std::vector<Element>> rows;
        
        rows.push_back({
            text(" ID ") | bold | center, 
            text(" Nume Produs ") | bold | center, 
            text(" Cantitate ") | bold | center, 
            text(" Pret Achiz. ") | bold | center, 
            text(" Pret Vanz. ") | bold | center,
            text(" Volum (m3) ") | bold | center
        });

        if (produse_pagina.empty()) {
            rows.push_back({text("-") | center, text("BAZA DE DATE ESTE GOALA") | align_right, text("-") | center, text("-") | center, text("-") | center, text("-") | center});
        } else {
            for (const auto& p : produse_pagina) {
                auto culoare_stoc = (p->getCantitate() <= p->getPragAlerta()) ? color(Color::Red) : color(Color::White);
                auto culoare_nume = color(Color::White);
                
                if (p->getNume().find("[Electronice]") != std::string::npos) culoare_nume = color(Color::Cyan);
                else if (p->getNume().find("[Perisabile]") != std::string::npos) culoare_nume = color(Color::Yellow);

                std::stringstream ssVolum;
                ssVolum << std::fixed << std::setprecision(3) << p->getVolum();

                rows.push_back({
                    text(" " + std::to_string(p->getId()) + " ") | center,
                    text(" " + p->getNume() + " ") | culoare_nume,
                    text(" " + std::to_string(p->getCantitate()) + " ") | culoare_stoc | center,
                    text(" " + std::to_string(p->getPretAchizitie()) + " ") | center,
                    text(" " + std::to_string(p->getPretVanzare()) + " ") | center,
                    text(" " + ssVolum.str() + " ") | color(Color::Magenta) | center
                });
            }
        }

        auto tabel = Table(rows);          
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        int total_produse = depozit.getToateProdusele().size();
        auto widget_total = hbox({
            text(" Total Produse în Catalog: " + std::to_string(total_produse) + " ") | bold | color(Color::Cyan)
        }) | border | hcenter;

        return vbox({
            text(" GESTIUNE STOC - PAGINA " + std::to_string(pagina_curenta + 1)) | bold | hcenter,
            widget_total,
            separator(),
            tabel.Render() | hcenter,  
            separator(),
            layout_butoane->Render() | hcenter,
            filler() 
        });
    });

   // ==========================================
    // 2. PANOU INTRARI MARFA
    // ==========================================

    int index_furnizor_aprov = 0;
    std::vector<std::string> lista_furnizori_aprov = {"- Fara furnizor specificat -"};
    Component dropdown_furnizori = Dropdown(&lista_furnizori_aprov, &index_furnizor_aprov);

    auto btn_add = Button("Creeaza Produs Nou", [&] {
        try {
            if(add_nume.empty() || add_cant.empty() || add_prag.empty() || 
               add_pret_achiz.empty() || add_pret_vanz.empty() || add_volum.empty()) {
                throw std::runtime_error("Toate campurile trebuie completate!");
            }

            int idAutomat = depozit.genereazaIdProdusNou();

            int cantitate = std::stoi(add_cant);
            int prag = std::stoi(add_prag);
            double pretAchiz = std::stod(add_pret_achiz);
            double pretVanz = std::stod(add_pret_vanz);
            double volum = std::stod(add_volum);

            depozit.adaugaProdus(Produs(idAutomat, add_nume, cantitate, prag, pretAchiz, pretVanz, volum));

            mesaj_status = "Succes: Produsul '" + add_nume + "' a primit ID-ul " + std::to_string(idAutomat);
            
            add_nume = add_cant = add_prag = add_pret_achiz = add_pret_vanz = add_volum = ""; 
            
        } catch (const std::invalid_argument& e) {
            mesaj_status = "Eroare: Va rugam introduceti doar numere valide (fara litere)!";
        } catch (const std::exception& e) { 
            mesaj_status = std::string("Eroare: ") + e.what(); 
        }
    });

    auto btn_aprov = Button("Actualizeaza Stoc", [&] {
        try {
            if (aprov_id.empty() || aprov_cant.empty()) {
                throw std::runtime_error("Ambele campuri (ID si Cantitate) sunt obligatorii!");
            }
            int id = std::stoi(aprov_id);
            int cant = std::stoi(aprov_cant);
            
            std::string furnizor_selectat = lista_furnizori_aprov[index_furnizor_aprov];

            depozit.aprovizioneazaProdus(id, cant);
            
            mesaj_status = "Succes: Stocul produsului " + aprov_id + " suplimentat. (Sursa: " + furnizor_selectat + ")";
            
            aprov_id = aprov_cant = "";
            index_furnizor_aprov = 0; 
            
        } catch (const std::exception& e) { 
            mesaj_status = std::string("Eroare Aprovizionare: ") + e.what(); 
        }
    });

    auto form_add = Container::Vertical({ 
        Input(&add_nume,"Nume Produs (Ex: Filtru Aer)"), 
        Input(&add_cant,"Cantitate Initiala"), 
        Input(&add_prag,"Prag Alerta"), 
        input_pret_achiz, 
        input_pret_vanz, 
        input_volum, 
        btn_add 
    });

    auto form_aprov = Container::Vertical({ 
        Input(&aprov_id, "ID Produs Existent"), 
        Input(&aprov_cant, "Cantitate Primita"), 
        dropdown_furnizori,
        btn_aprov 
    });

    auto layout_intrari = Container::Horizontal({ form_add, form_aprov });

    auto panou_intrari = Renderer(layout_intrari, [&] {
        
        auto furnizori_db = depozit.getFurnizori();
        if (furnizori_db.size() + 1 != lista_furnizori_aprov.size()) {
            lista_furnizori_aprov.clear();
            lista_furnizori_aprov.push_back("- Fara furnizor specificat -");
            for(const auto& f : furnizori_db) {
                lista_furnizori_aprov.push_back(f.getNume());
            }
            if (index_furnizor_aprov >= lista_furnizori_aprov.size()) {
                index_furnizor_aprov = 0;
            }
        }

        return vbox({
            text(" MODUL RECEPTIE MARFA (INTRARI) ") | bold | hcenter,
            separator(),
            hbox({
                window(text(" 1. DEFINIRE PRODUS NOU (CATALOG) "), form_add->Render() | flex),
                text("   "),
                window(text(" 2. APROVIZIONARE STOC EXISTENT "), vbox({
                    text("Folositi acest formular pentru produsele care") | dim | hcenter,
                    text("exista deja in baza de date.") | dim | hcenter,
                    separator(),
                    
                    hbox(text(" ID Produs: "), form_aprov->ChildAt(0)->Render() | flex),
                    hbox(text(" Cantitate: "), form_aprov->ChildAt(1)->Render() | flex),
                    hbox(text(" Sursa:     "), form_aprov->ChildAt(2)->Render() | flex),
                    
                    filler(),
                    form_aprov->ChildAt(3)->Render() | hcenter
                })) | flex
            }) | flex
        });
    });

    // ==========================================
    // 3. PANOU VANZARE SI LOGISTICA
    // ==========================================
    std::string input_id_vanzare;
    std::string input_cant_vanzare;
    std::string input_nume_client;
    std::string input_adresa_client;
    std::string mesaj_vanzare = "Asteptare date comanda...";
    
    int masina_selectata = 0;
    std::vector<std::string> lista_masini = {"Fara Transport (Ridicare personala)"};

    Component input_id = Input(&input_id_vanzare, " ID Produs (Ex: 1) ");
    Component input_cant = Input(&input_cant_vanzare, " Cantitate (Ex: 50) ");
    Component input_client = Input(&input_nume_client, " Nume / Companie Client ");
    Component input_adresa = Input(&input_adresa_client, " Adresa de Livrare ");
    Component dropdown_masini = Dropdown(&lista_masini, &masina_selectata);

    auto btn_vinde = Button("Confirma Comanda & Expedierea", [&] {
        try {
            if (input_id_vanzare.empty() || input_cant_vanzare.empty()) {
                throw std::runtime_error("Ambele campuri trebuie completate pentru a finaliza vanzarea!");
            }

            int id = std::stoi(input_id_vanzare);
            int cant = std::stoi(input_cant_vanzare);
            std::string masina = lista_masini[masina_selectata];
            
            std::string awb = depozit.proceseazaComandaCompleta(id, cant, input_nume_client, input_adresa_client, masina);
            
            mesaj_vanzare = "SUCCES! AWB generat: " + awb;
            if (masina != "Fara Transport (Ridicare personala)") {
                mesaj_vanzare += " (Asignat pe " + masina + ")";
            }
            
            input_id_vanzare.clear();
            input_cant_vanzare.clear();
            input_nume_client.clear();
            input_adresa_client.clear();
            
        } catch (const std::exception& e) {
            mesaj_vanzare = std::string("EROARE: ") + e.what();
        }
    });

    auto layout_vanzare = Container::Vertical({ 
        input_id, input_cant, input_client, input_adresa, dropdown_masini, btn_vinde 
    });

    auto panou_vanzare = Renderer(layout_vanzare, [&] {
        auto disponibile = depozit.getCamioaneDisponibile();
        lista_masini.clear();
        lista_masini.push_back("Fara Transport (Ridicare personala)");
        for(const auto& m : disponibile) {
            lista_masini.push_back(m);
        }
        if (masina_selectata >= lista_masini.size()) masina_selectata = 0;

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
    std::string eroare_dispecerat = "";

    Component dropdown_disp = Dropdown(&lista_masini_disp, &index_masina_disp);
    
    auto btn_expediaza = Button("Expediaza Vehiculul in Cursa", [&] {
        try {
            eroare_dispecerat = "";
            status_dispecerat = "";

            if (index_masina_disp == 0 || index_masina_disp >= lista_masini_disp.size()) {
                throw std::invalid_argument("Va rugam sa selectati un vehicul valid din flota.");
            }

            std::string masina_selectata = lista_masini_disp[index_masina_disp];
            
            bool ok = depozit.declanseazaExpediere(masina_selectata);
            if (!ok) {
                throw std::runtime_error("Operatiunea a fost respinsa de baza de date.");
            }

            status_dispecerat = "SUCCES: Vehiculul " + masina_selectata + " a plecat in cursa!";
            index_masina_disp = 0;

        } catch (const std::exception& e) {
            eroare_dispecerat = e.what();
        }
    });

    auto layout_dispecerat = Container::Vertical({ dropdown_disp, btn_expediaza });

    auto panou_dispecerat = Renderer(layout_dispecerat, [&] {
        auto masini_db = depozit.getCamioaneDisponibile();
        if (masini_db.size() + 1 != lista_masini_disp.size()) {
            lista_masini_disp.clear();
            lista_masini_disp.push_back("- Selecteaza vehicul -");
            for(const auto& m : masini_db) {
                lista_masini_disp.push_back(m);
            }
            if (index_masina_disp >= lista_masini_disp.size()) {
                index_masina_disp = 0;
            }
        }

        std::string masina_curenta = lista_masini_disp[index_masina_disp];
        double volum_ocupat = 0.0;
        double capacitate_maxima = 0.0;
        float procent_incarcare = 0.0f;
        
        if (masina_curenta != "- Selecteaza vehicul -") {
            volum_ocupat = depozit.verificaIncarcareVehicul(masina_curenta);
            capacitate_maxima = depozit.getCapacitateCamion(masina_curenta); 
            if (capacitate_maxima > 0.0) {
                procent_incarcare = static_cast<float>(volum_ocupat / capacitate_maxima);
            }
        }
        
        std::stringstream ssVol, ssCap, ssProcent;
        ssVol << std::fixed << std::setprecision(2) << volum_ocupat;
        ssCap << std::fixed << std::setprecision(2) << capacitate_maxima;
        ssProcent << std::fixed << std::setprecision(1) << (procent_incarcare * 100.0f);
        
        auto culoare_bara = color(Color::Green);
        if (procent_incarcare > 0.95f) culoare_bara = color(Color::Red);
        else if (procent_incarcare > 0.75f) culoare_bara = color(Color::Yellow);

        Element bara_grafica = gauge(std::min(procent_incarcare, 1.0f)) | culoare_bara;

        Element afisaj_mesaje = text("");
        if (!eroare_dispecerat.empty()) {
            afisaj_mesaje = hbox({ text(" [!] EROARE: " + eroare_dispecerat + " ") | bold | color(Color::Red) }) | border | hcenter;
        } else if (!status_dispecerat.empty()) {
            afisaj_mesaje = text(status_dispecerat) | bold | color(Color::Yellow) | hcenter;
        }

        return vbox({
            text(" DISPECERAT LOGISTICA FLOTA ") | bold | hcenter,
            separator(),
            hbox(text(" Selectie Vehicul: "), dropdown_disp->Render() | flex) | border,
            separator(),
            masina_curenta == "- Selecteaza vehicul -" ? 
                text("Selectati un vehicul din lista pentru a-i vedea planul de incarcare.") | dim | hcenter :
                vbox({
                    text(" STATISTICI INCARCARE VEHICUL: " + masina_curenta) | bold | color(Color::Cyan) | hcenter,
                    text(" Marfa alocata: " + ssVol.str() + " / " + ssCap.str() + " m³") | hcenter,
                    hbox({
                        text(" ["),
                        bara_grafica | flex,
                        text("] " + ssProcent.str() + " %")
                    }) | bold,
                }) | border,
            separator(),
            afisaj_mesaje,
            btn_expediaza->Render() | hcenter
        });
    });

    // ==========================================
    // 5. PANOU ALERTE
    // ==========================================
    int pagina_curenta_alerte = 0;
    const int ALERTE_PER_PAGINA = 10;

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
            text(" ID ") | bold | center, 
            text(" Nume ") | bold | center, 
            text(" Tip ") | bold | center, 
            text(" Stoc ") | bold | center, 
            text(" Prag ") | bold | center
        });

        if (total == 0) {
            rows.push_back({text("-") | center, text("STOCURI OPTIME.") | center, text("-") | center, text("-") | center, text("-") | center});
        } else {
            int start_idx = pagina_curenta_alerte * ALERTE_PER_PAGINA;
            int end_idx = std::min(start_idx + ALERTE_PER_PAGINA, total);

            for (int i = start_idx; i < end_idx; ++i) {
                const auto& p = produse_critice[i];
                auto culoare_nume = color(Color::White);

                if (p->getNume().find("[Electronice]") != std::string::npos) culoare_nume = color(Color::Cyan);
                else if (p->getNume().find("[Perisabile]") != std::string::npos) culoare_nume = color(Color::Yellow);

               rows.push_back({
                    text(" " + std::to_string(p->getId()) + " ") | center, 
                    text(" " + p->getNume() + " ") | culoare_nume,
                    text(" " + p->getTipProdus() + " ") | color(Color::Yellow) | center,
                    text(" " + std::to_string(p->getCantitate()) + " ") | color(Color::Red) | bold | center, 
                    text(" " + std::to_string(p->getPragAlerta()) + " ") | color(Color::Cyan) | center
                });
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        auto culoare_widget = total > 0 ? color(Color::Red) : color(Color::Green);
        auto widget_alerte = hbox({
            text(" Avertismente Active (Stoc Critic): " + std::to_string(total) + " ") | bold | culoare_widget
        }) | border | hcenter;

        return vbox({
            text("PRODUSE IN STOC CRITIC") | bold | color(Color::Red) | hcenter,
            widget_alerte, 
            separator(),
            tabel.Render() | hcenter,
            separator(),
            layout_nav_alerte->Render() | hcenter,
            filler()
        });
    });

    // ==========================================
    // 6. PANOU CAUTARE
    // ==========================================
    auto input_cautare = Input(&search_query, " Ex: senzor, rulment... ");
    
    auto panou_cautare = Renderer(input_cautare, [&] {
        std::vector<std::vector<Element>> rows;
        
        // 1. Centram Header-ul, dar Numele il aliniem la dreapta
        rows.push_back({ 
            text(" ID ") | bold | center, 
            text(" Nume Produs ") | bold | center, 
            text(" Stoc curent ") | bold | center 
        });

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
                    
                    // 2. Centram celulele randului, dar Numele il aliniem la dreapta
                    rows.push_back({
                        text(" " + std::to_string(p.getId()) + " ") | center, 
                        text(" " + p.getNume() + " "), 
                        text(" " + std::to_string(p.getCantitate()) + " ") | culoare_stoc | center
                    });
                    afisate++;
                }
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Yellow));

        // 3. Extragem zona care se schimba in functie de rezultate pentru a-i aplica centrarea globala frumos
        Element zona_rezultate;
        if (search_query.empty()) {
            zona_rezultate = text(" Introduceti text pentru a cauta...") | dim | hcenter;
        } else if (rows.size() == 1) {
            zona_rezultate = text(" Niciun produs gasit.") | color(Color::Red) | hcenter;
        } else {
            // Aici centram efectiv tabelul pe ecran
            zona_rezultate = tabel.Render() | hcenter; 
        }

        return vbox({ 
            text("CAUTARE LIVE IN BAZA DE DATE") | bold | hcenter, 
            separator(), 
            hbox({ text(" Cauta: "), input_cautare->Render() | flex }) | border, 
            separator(), 
            
            // 4. Afisam zona de rezultate (care acum include tabelul perfect centrat)
            zona_rezultate,
            
            filler()
        });
    });

    //==========================================
    // 7. PANOU ISTORIC
    // ==========================================
    int pagina_curenta_istoric = 0;
    const int ELEMENTE_PE_PAGINA_I = 10;

    auto btn_prev_istoric = Button(" < Pagina Anterioara ", [&] {
        if (pagina_curenta_istoric > 0) pagina_curenta_istoric--;
    });

    auto btn_next_istoric = Button(" Pagina Urmatoare > ", [&] {
        int total = depozit.getNumarTotalTranzactii();
        int max_pagini = (total == 0) ? 1 : (total + ELEMENTE_PE_PAGINA_I - 1) / ELEMENTE_PE_PAGINA_I;
        if (pagina_curenta_istoric < max_pagini - 1) pagina_curenta_istoric++;
    });

    auto layout_istoric = Container::Horizontal({ btn_prev_istoric, btn_next_istoric });

    auto panou_istoric = Renderer(layout_istoric, [&] {
        auto lista_istoric = depozit.getIstoric(); 
        int total_loguri = depozit.getNumarTotalTranzactii();
        
        int max_pagini = (total_loguri == 0) ? 1 : (total_loguri + ELEMENTE_PE_PAGINA_I - 1) / ELEMENTE_PE_PAGINA_I;
        
        if (pagina_curenta_istoric >= max_pagini && pagina_curenta_istoric > 0) {
            pagina_curenta_istoric = max_pagini - 1;
        }

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

        if (total_loguri == 0) {
            rows.push_back({text("-"), text("-"), text("Nicio activitate inregistrata."), text("-"), text("-"), text("-"), text("-")});
        } else {
            int start_idx = pagina_curenta_istoric * ELEMENTE_PE_PAGINA_I;
            int end_idx = std::min(start_idx + ELEMENTE_PE_PAGINA_I, total_loguri);

            for (int i = start_idx; i < end_idx; ++i) {
                const auto& t = lista_istoric[i];
                auto culoare_operatie = (t.getTipOperatie() == "VANZARE") ? color(Color::Red) : color(Color::Green);

                rows.push_back({
                    text(" " + std::to_string(t.getIdLog()) + " "),
                    text(" " + std::to_string(t.getIdProdus()) + " "),
                    text(" " + t.getTipOperatie() + " ") | culoare_operatie | bold,
                    text(" " + std::to_string(t.getCantitate()) + " "),
                    text(" " + formateazaBani(t.getPret()) + " ") | color(Color::Yellow),
                    text(" " + formateazaBani(t.getValoareTotala()) + " ") | color(Color::Cyan) | bold,
                    text(" " + t.getDataOraString() + " ") | color(Color::GrayLight)
                });
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Yellow));

        auto widget_istoric = hbox({
            text(" Volum Total Tranzactii Inregistrate: " + std::to_string(total_loguri) + " ") | bold | color(Color::Yellow)
        }) | border | hcenter;

        Element navigare_istoric = hbox({
            btn_prev_istoric->Render(),
            text(" Pagina " + std::to_string(pagina_curenta_istoric + 1) + " din " + std::to_string(max_pagini) + " ") | center,
            btn_next_istoric->Render()
        }) | hcenter;

        return vbox({
            text("JURNAL DE AUDIT AUTOMAT") | bold | hcenter,
            widget_istoric,
            separator(),
            tabel.Render() | hcenter,
            separator(),
            navigare_istoric,
            filler()
        });
    });

    // ==========================================
    // 8. PANOU RAPORT FINANCIAR (PROFIT)
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
        
        rows.push_back({ 
            text(" ID ") | bold | center, 
            text(" Nume Produs ") | bold | center, 
            text(" Profit / Bucata ") | bold | center, 
            text(" Profit Total Estimat ") | bold | center 
        });

        int afisate = 0;
        for (const auto& p : top_produse) {
            if (afisate >= 10 || p.getCantitate() == 0) break; 

            double profit_bucata = p.getPretVanzare() - p.getPretAchizitie();
            double profit_total = profit_bucata * p.getCantitate();
            
            rows.push_back({
                text(" " + std::to_string(p.getId()) + " ") | center,
                text(" " + p.getNume() + " "),
                text(" " + formateazaBani(profit_bucata) + " ") | color(Color::Green) | center,
                text(" " + formateazaBani(profit_total) + " ") | color(Color::Green) | bold | center
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
            
            tabel.Render() | hcenter,
            
            filler()
        });
    });

    // ==========================================
    // 9. PANOU MENTENANTA FLOTA
    // ==========================================
    int idx_cursa = 0;
    int idx_service = 0;
    int idx_manual = 0;
    std::vector<std::string> lista_sosiri = {"- Fara vehicule pe traseu -"};
    std::vector<std::string> lista_revizii = {"- Flota este in stare optima -"};
    std::vector<std::string> lista_manuala = {"- Niciun vehicul disponibil -"};
    std::string mesaj_mentenanta = "Monitorizare activa.";

    int pagina_curenta_mentenanta = 0;
    const int ELEMENTE_PE_PAGINA_M = 6;

    Component drop_sosiri = Dropdown(&lista_sosiri, &idx_cursa);
    Component drop_revizii = Dropdown(&lista_revizii, &idx_service);
    Component drop_manual = Dropdown(&lista_manuala, &idx_manual);

    auto btn_prev_m = Button(" < Inapoi ", [&] {
        if (pagina_curenta_mentenanta > 0) pagina_curenta_mentenanta--;
    });

    auto btn_next_m = Button(" Inainte > ", [&] {
        int total = depozit.getIstoricService().size();
        int max_pagini = (total == 0) ? 1 : (total + ELEMENTE_PE_PAGINA_M - 1) / ELEMENTE_PE_PAGINA_M;
        if (pagina_curenta_mentenanta < max_pagini - 1) pagina_curenta_mentenanta++;
    });

    auto btn_sosire = Button("Confirma Sosirea", [&] {
        if (lista_sosiri[idx_cursa] != "- Fara vehicule pe traseu -") {
            depozit.finalizeazaCursa(lista_sosiri[idx_cursa]);
            mesaj_mentenanta = "Vehiculul " + lista_sosiri[idx_cursa] + " a revenit in baza.";
            pagina_curenta_mentenanta = 0; 
        }
    });

    auto btn_revizie = Button("Finalizeaza Revizie Periodica", [&] {
        if (lista_revizii[idx_service] != "- Flota este in stare optima -") {
            depozit.efectueazaRevizie(lista_revizii[idx_service], "Revizie periodica obligatorie");
            mesaj_mentenanta = "Revizie finalizata pentru " + lista_revizii[idx_service];
            pagina_curenta_mentenanta = 0;
        }
    });

    auto btn_manual = Button("Aplica Mentenanta Preventiva Acum", [&] {
        if (lista_manuala[idx_manual] != "- Niciun vehicul disponibil -") {
            depozit.efectueazaRevizie(lista_manuala[idx_manual], "Revizie preventiva (La cerere)");
            mesaj_mentenanta = "Mentenanta la cerere efectuata pentru " + lista_manuala[idx_manual];
            pagina_curenta_mentenanta = 0;
        }
    });

    auto layout_mentenanta = Container::Vertical({ 
        drop_sosiri, btn_sosire, 
        drop_revizii, btn_revizie, 
        drop_manual, btn_manual,
        btn_prev_m, btn_next_m 
    });

    auto panou_mentenanta = Renderer(layout_mentenanta, [&] {
        auto in_cursa = depozit.getCamioaneInCursa();
        lista_sosiri.clear();
        if (in_cursa.empty()) lista_sosiri.push_back("- Fara vehicule pe traseu -");
        else for (const auto& m : in_cursa) lista_sosiri.push_back(m);
        if (idx_cursa >= lista_sosiri.size()) idx_cursa = 0;

        auto in_service = depozit.getCamioaneInService();
        lista_revizii.clear();
        if (in_service.empty()) lista_revizii.push_back("- Flota este in stare optima -");
        else for (const auto& m : in_service) lista_revizii.push_back(m);
        if (idx_service >= lista_revizii.size()) idx_service = 0;

        auto disponibile = depozit.getCamioaneDisponibile();
        lista_manuala.clear();
        if (disponibile.empty()) lista_manuala.push_back("- Niciun vehicul disponibil -");
        else for(const auto& m : disponibile) lista_manuala.push_back(m);
        if (idx_manual >= lista_manuala.size()) idx_manual = 0;

        auto jurnal = depozit.getIstoricService();
        
        int total_inregistrari = jurnal.size();
        int max_pagini = (total_inregistrari == 0) ? 1 : (total_inregistrari + ELEMENTE_PE_PAGINA_M - 1) / ELEMENTE_PE_PAGINA_M;
        
        if (pagina_curenta_mentenanta >= max_pagini && pagina_curenta_mentenanta > 0) {
            pagina_curenta_mentenanta = max_pagini - 1;
        }

        Elements randuri_jurnal;
        
        randuri_jurnal.push_back(hbox({
            text(" Data ") | bold | size(WIDTH, EQUAL, 20) | center, 
            text(" Vehicul ") | bold | size(WIDTH, EQUAL, 15) | center, 
            text(" Tip Interventie ") | bold | center
        }) | hcenter);
        randuri_jurnal.push_back(separator());
        
        if (total_inregistrari == 0) {
            randuri_jurnal.push_back(text(" Niciun istoric de mentenanta inregistrat.") | color(Color::GrayLight) | hcenter);
        } else {
            int start_idx = pagina_curenta_mentenanta * ELEMENTE_PE_PAGINA_M;
            int end_idx = std::min(start_idx + ELEMENTE_PE_PAGINA_M, total_inregistrari);

            for (int i = start_idx; i < end_idx; ++i) {
                const auto& rec = jurnal[i];
                randuri_jurnal.push_back(hbox({
                    text(" " + rec.data) | size(WIDTH, EQUAL, 20) | color(Color::GrayLight) | center,
                    text(" " + rec.idCamion) | size(WIDTH, EQUAL, 15) | bold | color(Color::Cyan) | center,
                    text(" " + rec.tipInterventie) | color(Color::Yellow) | center
                }) | hcenter);
            }
        }

        Element navigare_jurnal = hbox({
            btn_prev_m->Render(),
            text(" Pagina " + std::to_string(pagina_curenta_mentenanta + 1) + " din " + std::to_string(max_pagini) + " ") | center,
            btn_next_m->Render()
        }) | hcenter;

        auto zona_receptie = window(text(" Receptie din Cursa ") | hcenter, vbox({
            hbox(filler(), text("Vehicul la poarta: "), drop_sosiri->Render() | size(WIDTH, EQUAL, 30), filler()),
            btn_sosire->Render() | hcenter
        }));

        auto zona_service = window(text(" Atelier Mecanic (Obligatoriu) ") | hcenter, vbox({
            hbox(filler(), text("Vehicul imobilizat: "), drop_revizii->Render() | size(WIDTH, EQUAL, 30), filler()),
            btn_revizie->Render() | hcenter
        }));

        auto zona_manuala = window(text(" Mentenanta La Cerere (Preventiv) ") | hcenter, vbox({
            hbox(filler(), text("Vehicul in curte:  "), drop_manual->Render() | size(WIDTH, EQUAL, 30), filler()),
            btn_manual->Render() | hcenter
        }));

        auto zona_jurnal = window(text(" Jurnal Mentenanta ") | hcenter, vbox({
            vbox(std::move(randuri_jurnal)),
            separator(),
            navigare_jurnal 
        }));

        return vbox({
            text(" MANAGEMENT MENTENANTA SI UZURA ") | bold | hcenter,
            separator(),
            
            hbox(filler(), zona_receptie | size(WIDTH, EQUAL, 60), text("   "), zona_service | size(WIDTH, EQUAL, 60), filler()),
            hbox(filler(), zona_manuala | size(WIDTH, EQUAL, 93), filler()),
            hbox(filler(), zona_jurnal | size(WIDTH, EQUAL, 93), filler()),
            
            separator(),
            text(mesaj_mentenanta) | bold | color(Color::Yellow) | hcenter,
            filler()
        });
    });

    // ==========================================
    // 10. PANOU FURNIZOR
    // ==========================================
    int pagina_curenta_f = 0;
    const int ELEMENTE_PE_PAGINA_F = 5;

    auto btn_prev_f = Button(" < Inapoi ", [&] {
        if (pagina_curenta_f > 0) pagina_curenta_f--;
    });

    auto btn_next_f = Button(" Inainte > ", [&] {
        int total = depozit.getTotalFurnizori();
        int max_pagini = (total == 0) ? 1 : (total + ELEMENTE_PE_PAGINA_F - 1) / ELEMENTE_PE_PAGINA_F;
        if (pagina_curenta_f < max_pagini - 1) pagina_curenta_f++;
    });

    auto btn_adauga_furnizor = Button("Inregistreaza Furnizor", [&] {
        try {
            eroare_furnizor = "";
            
            if (input_nume_furnizor.empty() || input_contact_furnizor.empty()) {
                throw std::runtime_error("Toate campurile (Nume, Contact) sunt obligatorii!");
            }

            Furnizor f(0, input_nume_furnizor, input_contact_furnizor);
            depozit.adaugaFurnizor(f);

            input_nume_furnizor = "";
            input_contact_furnizor = "";
        } catch (const std::exception& e) {
            eroare_furnizor = e.what();
        }
    });

    auto btn_sterge_furnizor = Button("Elimina Furnizor", [&] {
        try {
            eroare_furnizor = "";
            
            if (input_id_stergere_f.empty()) {
                throw std::runtime_error("Introduceti ID-ul furnizorului pe care doriti sa il stergeti!");
            }

            int id = std::stoi(input_id_stergere_f);
            
            bool ok = depozit.stergeFurnizor(id);
            if (!ok) {
                throw std::runtime_error("A aparut o eroare la stergere.");
            }

            input_id_stergere_f = "";
        } catch (const std::invalid_argument&) {
            eroare_furnizor = "ID-ul introdus trebuie sa contina doar cifre!";
        } catch (const std::exception& e) {
            eroare_furnizor = e.what();
        }
    });

    auto layout_furnizori = Container::Vertical({ 
        camp_nume_f, 
        camp_contact_f, 
        btn_adauga_furnizor,
        camp_id_stergere_f,
        btn_sterge_furnizor,
        btn_prev_f,
        btn_next_f
    });

    auto panou_furnizori = Renderer(layout_furnizori, [&] {
        int total_furnizori = depozit.getTotalFurnizori();
        int max_pagini = (total_furnizori == 0) ? 1 : (total_furnizori + ELEMENTE_PE_PAGINA_F - 1) / ELEMENTE_PE_PAGINA_F;
        
        if (pagina_curenta_f >= max_pagini && pagina_curenta_f > 0) {
            pagina_curenta_f = max_pagini - 1;
        }

        std::vector<std::vector<Element>> rows;
        
        rows.push_back({
            text(" ID Partener ") | bold, 
            text(" Nume Companie / Furnizor ") | bold, 
            text(" Date de Contact (Tel / Email) ") | bold
        });
        
        int offset = pagina_curenta_f * ELEMENTE_PE_PAGINA_F;
        auto lista_paginata = depozit.getFurnizoriPaginat(ELEMENTE_PE_PAGINA_F, offset);
        
        if (lista_paginata.empty()) {
            rows.push_back({text("-"), text("Niciun furnizor inregistrat pe aceasta pagina."), text("-")});
        } else {
            for (const auto& f : lista_paginata) {
                rows.push_back({
                    text(" " + std::to_string(f.getId()) + " "),
                    text(" " + f.getNume() + " ") | color(Color::White),
                    text(" " + f.getContact() + " ") | color(Color::Yellow)
                });
            }
        }
        
        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        Element navigare_pagini = hbox({
            btn_prev_f->Render(),
            text(" Pagina " + std::to_string(pagina_curenta_f + 1) + " din " + std::to_string(max_pagini) + " ") | center,
            btn_next_f->Render()
        }) | hcenter;

        return vbox({
            text(" GESTIUNE FURNIZORI SI ACHIZITII ") | bold | hcenter,
            separator(),
            
            hbox({
                vbox({
                    text(" Adaugare Partener Nou:") | bold,
                    hbox(text(" Nume Companie: "), camp_nume_f->Render() | flex) | border,
                    hbox(text(" Date Contact:  "), camp_contact_f->Render() | flex) | border,
                    btn_adauga_furnizor->Render() | hcenter
                }) | flex,
                
                separator(),
                vbox({
                    text(" Eliminare Partener:") | bold,
                    hbox(text(" ID Furnizor:   "), camp_id_stergere_f->Render() | flex) | border,
                    filler(),
                    btn_sterge_furnizor->Render() | hcenter
                }) | flex
            }),
            
            render_eroare(eroare_furnizor),
            separator(),
            
            text(" REGISTRU FURNIZORI ACTIVI (Total: " + std::to_string(total_furnizori) + ") ") | bold | hcenter,
            
            tabel.Render() | hcenter,
            
            navigare_pagini,
            
            filler()
        });
    });

    // ==========================================
    // ASAMBLAREA INTERFEȚEI
    // ==========================================
    std::vector<std::string> meniu_text = { 
        " 1. Afisare Stoc ", 
        " 2. Receptie Marfa ", 
        " 3. Vanzare / Comanda", 
        " 4. Dispecerat",
        " 5. Raport Alerte ", 
        " 6. Cautare Live",
        " 7. Jurnal Istoric ",
        " 8. Raport Profit ",
        " 9. Mentenanta Auto",
        " 10. Furnizori",
        " 0. Iesire " 
    };
    auto meniu_lateral = Menu(&meniu_text, &tab_index);

    auto tab_container = Container::Tab({ 
        panou_stoc,         
        panou_intrari,
        panou_vanzare,  
        panou_dispecerat,     
        panou_alerte,
        panou_cautare,
        panou_istoric,
        panou_profit,
        panou_mentenanta,
        panou_furnizori,
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