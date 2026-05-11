#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Include-uri FTXUI
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

// Include-urile tale
#include "Depozit.h"
#include "Tranzactie.h"

using namespace ftxui;

int main() {
    Depozit depozit;

    // const std::string FISIER_STOC = "stoc.csv";
    // depozit.incarcaDinFisier(FISIER_STOC);

    auto screen = ScreenInteractive::Fullscreen();

    // ==========================================
    // STATE-UL APLICAȚIEI (Variabile globale UI)
    // ==========================================
    int tab_index = 0; 
    std::string mesaj_status = "Sistem pornit. Gata pentru operatiuni.";

    // Variabile pentru formulare
    std::string add_id, add_nume, add_cant, add_pret, add_prag;
    std::string vanzare_id, vanzare_cant;
    std::string aprov_id, aprov_cant;
    std::string search_query;


    auto test_date = depozit.getProdusePaginat(1, 0);
    // ==========================================
    // 1. PANOU AFISARE STOC (CU PAGINARE)
    // ==========================================
    int pagina_curenta = 0;
    const int PRODUSE_PER_PAGINA = 10; // Un număr optim pentru a nu aglomera ecranul

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
        rows.push_back({text("ID")|bold, text("Nume")|bold, text("Tip")|bold, text("Stoc")|bold, text("Detalii")|bold});

        if (produse_pagina.empty()) {
            rows.push_back({text("-"), text("BAZA DE DATE ESTE GOALA"), text("-"), text("-"), text("-")});
        } else {
            for (const auto& p : produse_pagina) {
                // 'p' este acum std::unique_ptr<Produs>, folosim ->
                rows.push_back({
                    text(std::to_string(p->getId())),
                    text(p->getNume()),
                    text(p->getTipProdus()) | color(Color::Yellow), // Apel polimorfic!
                    text(std::to_string(p->getCantitate())),
                    text(p->getDetaliiSpecifice()) | color(Color::Cyan) // Apel polimorfic!
                });
            }
        }

        auto tabel_randat = Table(rows);
        tabel_randat.SelectAll().Border(LIGHT); // Adaugă marginile exterioare
        tabel_randat.SelectAll().Separator(LIGHT); // Adaugă liniile verticale și orizontale
        tabel_randat.SelectRow(0).Decorate(color(Color::Cyan));

        return vbox({
            text(" GESTIUNE STOC - PAGINA " + std::to_string(pagina_curenta + 1)) | bold | hcenter,
            separator(),
            tabel_randat.Render(),
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
            depozit.adaugaProdus(Produs(std::stoi(add_id), add_nume, std::stoi(add_cant), std::stod(add_pret), std::stoi(add_prag)));
            mesaj_status = "Succes: Produsul '" + add_nume + "' a fost adaugat!";
            add_id = add_nume = add_cant = add_pret = add_prag = ""; // Reset form
        } catch (const std::exception& e) { mesaj_status = std::string("Eroare: ") + e.what(); }
    });
    auto form_add = Container::Vertical({ Input(&add_id, "ID"), Input(&add_nume, "Nume"), Input(&add_cant, "Cantitate"), Input(&add_pret, "Pret"), Input(&add_prag, "Prag"), btn_add });
    auto panou_adaugare = Renderer(form_add, [&] {
        return window(text(" ADAUGARE PRODUS NOU "), vbox({ form_add->Render() | size(WIDTH, LESS_THAN, 40) })) | hcenter;
    });

    // ==========================================
    // 3. PANOU VANZARE
    // ==========================================
    auto btn_vinde = Button("Confirma Vanzarea", [&] {
        try {
            int id = std::stoi(vanzare_id);
            int cant = std::stoi(vanzare_cant);
            depozit.vindeProdus(id, cant);
            Tranzactie<std::string>(id, cant, "VANZARE").afiseazaDetalii(); // Se printeaza in spate/log
            mesaj_status = "Succes: Au fost vandute " + vanzare_cant + " bucati din produsul " + vanzare_id;
            vanzare_id = vanzare_cant = "";
        } catch (const std::exception& e) { mesaj_status = std::string("Eroare: ") + e.what(); }
    });
    auto form_vanzare = Container::Vertical({ Input(&vanzare_id, "ID Produs"), Input(&vanzare_cant, "Cantitate"), btn_vinde });
    auto panou_vanzare = Renderer(form_vanzare, [&] {
        return window(text(" IESIRE STOC (VANZARE) "), vbox({ form_vanzare->Render() | size(WIDTH, LESS_THAN, 40) })) | hcenter;
    });

    // ==========================================
    // 4. PANOU APROVIZIONARE
    // ==========================================
    auto btn_aprov = Button("Confirma Aprovizionarea", [&] {
        try {
            int id = std::stoi(aprov_id);
            int cant = std::stoi(aprov_cant);
            depozit.getProdus(id) += cant;
            Tranzactie<std::string>(id, cant, "APROVIZIONARE").afiseazaDetalii();
            mesaj_status = "Succes: Stocul produsului " + aprov_id + " a fost suplimentat cu " + aprov_cant;
            aprov_id = aprov_cant = "";
        } catch (const std::exception& e) { mesaj_status = std::string("Eroare: ") + e.what(); }
    });
    auto form_aprov = Container::Vertical({ Input(&aprov_id, "ID Produs"), Input(&aprov_cant, "Cantitate adaugata"), btn_aprov });
    auto panou_aprovizionare = Renderer(form_aprov, [&] {
        return window(text(" INTRARE STOC (APROVIZIONARE) "), vbox({ form_aprov->Render() | size(WIDTH, LESS_THAN, 40) })) | hcenter;
    });

    // ==========================================
    // 5. PANOU ALERTE
    // ==========================================
    auto panou_alerte = Renderer([&] {
        std::vector<std::vector<Element>> rows = {{text("ID")|bold, text("Nume")|bold, text("Stoc")|bold, text("Prag")|bold}};
        for (const auto& p : depozit.getToateProdusele()) {
            if (p.getCantitate() <= p.getPragAlerta()) { // Filtram DOAR alertele
                rows.push_back({ text(std::to_string(p.getId())), text(p.getNume()), text(std::to_string(p.getCantitate())) | color(Color::Red), text(std::to_string(p.getPragAlerta())) });
            }
        }
        auto t = Table(rows); t.SelectAll().Separator(LIGHT);
        return vbox({ text(" PRODUSE SUB PRAGUL CRITIC ") | bold | color(Color::Red) | hcenter, separator(), rows.size() > 1 ? t.Render() : text("Toate stocurile sunt optime!") | hcenter });
    });

    // ==========================================
    // 6. PANOU CAUTARE
    // ==========================================
    auto input_cautare = Input(&search_query, "Scrieti numele aici...");
    auto panou_cautare = Renderer(input_cautare, [&] {
        std::vector<Element> rezultate;
        if (!search_query.empty()) {
            for (const auto& p : depozit.getToateProdusele()) {
                if (p.getNume().find(search_query) != std::string::npos) {
                    rezultate.push_back(text("ID: " + std::to_string(p.getId()) + " | Nume: " + p.getNume() + " | Stoc: " + std::to_string(p.getCantitate())));
                }
            }
        }
        return vbox({ text(" CAUTARE LIVE ") | bold | hcenter, separator(), input_cautare->Render() | border, separator(), rezultate.empty() ? text("Niciun rezultat...") : vbox(rezultate) });
    });

    // ==========================================
    // ASAMBLAREA INTERFEȚEI
    // ==========================================
    std::vector<std::string> meniu_text = { " 1. Afisare Stoc ", " 2. Adaugare Produs ", " 3. Vanzare ", " 4. Aprovizionare ", " 5. Raport Alerte ", " 6. Cautare ", " 0. Iesire " };
    auto meniu_lateral = Menu(&meniu_text, &tab_index);

    auto tab_container = Container::Tab({ panou_stoc, panou_adaugare, panou_vanzare, panou_aprovizionare, panou_alerte, panou_cautare, Button("EXIT", screen.ExitLoopClosure()) }, &tab_index);
    auto main_container = Container::Horizontal({ meniu_lateral, tab_container });

    auto ecran_final = Renderer(main_container, [&] {
        auto culoar_status = mesaj_status.find("Eroare") != std::string::npos ? color(Color::Red) : color(Color::Green);
        return vbox({
            hbox({
                vbox({ text(" DEPOZIT APP ") | bold | bgcolor(Color::Blue) | color(Color::White), separator(), meniu_lateral->Render() | flex }) | size(WIDTH, EQUAL, 25) | border,
                tab_container->Render() | flex | border
            }) | flex,
            hbox({ text(" Status: ") | bold, text(mesaj_status) | culoar_status }) | border
        });
    });

    screen.Loop(ecran_final);
    
    // Salvare finală la închidere
    //depozit.salveazaInFisier(FISIER_STOC);
    return 0;
}