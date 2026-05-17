#include <iostream>
#include <string>
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

    // const std::string FISIER_STOC = "stoc.csv";
    // depozit.incarcaDinFisier(FISIER_STOC);

    auto screen = ScreenInteractive::Fullscreen();

    // ==========================================
    // STATE-UL APLICAȚIEI (Variabile globale UI)
    // ==========================================
    int tab_index = 0; 
    std::string mesaj_status = "Sistem pornit. Gata pentru operatiuni.";

    std::string add_id, add_nume, add_cant, add_pret, add_prag;
    std::string vanzare_id, vanzare_cant;
    std::string aprov_id, aprov_cant;
    std::string search_query;


    auto test_date = depozit.getProdusePaginat(1, 0);
    // ==========================================
    // 1. PANOU AFISARE STOC (CU PAGINARE)
    // ==========================================
    int pagina_curenta = 0;
    const int PRODUSE_PER_PAGINA = 10;

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
                rows.push_back({
                    text(std::to_string(p->getId())),
                    text(p->getNume()),
                    text(p->getTipProdus()) | color(Color::Yellow),
                    text(std::to_string(p->getCantitate())),
                    text(p->getDetaliiSpecifice()) | color(Color::Cyan)
                });
            }
        }

        auto tabel_randat = Table(rows);
        tabel_randat.SelectAll().Border(LIGHT); 
        tabel_randat.SelectAll().Separator(LIGHT);
        tabel_randat.SelectRow(0).Decorate(color(Color::Cyan));

        return vbox({
            text(" GESTIUNE STOC - PAGINA " + std::to_string(pagina_curenta + 1)) | bold | hcenter,
            separator(),
            tabel_randat.Render() | flex,  
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
    std::string input_id_vanzare;
    std::string input_cant_vanzare;
    std::string mesaj_vanzare = "Asteptare date tranzactie...";

    Component input_id = Input(&input_id_vanzare, " Ex: 1 ");
    Component input_cant = Input(&input_cant_vanzare, " Ex: 50 ");

    auto btn_vinde = Button("Confirma Vanzarea", [&] {
        try {
            int id = std::stoi(input_id_vanzare);
            int cant = std::stoi(input_cant_vanzare);
            
            // Apelăm EXACT funcția ta din Depozit.cpp
            depozit.vindeProdus(id, cant);
            
            mesaj_vanzare = "SUCCES: Ai vandut " + std::to_string(cant) + " bucati din ID-ul " + std::to_string(id);
            input_id_vanzare.clear();
            input_cant_vanzare.clear();
        } catch (const std::exception& e) {
            mesaj_vanzare = "EROARE: Verifica datele! Asigura-te ca ID-ul exista.";
        }
    });

    auto layout_formular = Container::Vertical({ input_id, input_cant, btn_vinde });

    auto panou_vanzare = Renderer(layout_formular, [&] {
        auto culoar_mesaj = mesaj_vanzare.find("EROARE") != std::string::npos ? color(Color::Red) : color(Color::Green);
        
        return vbox({
            text(" MODUL VANZARE PRODUSE ") | bold | hcenter,
            separator(),
            hbox(text(" ID Produs: "), input_id->Render() | border) | hcenter,
            hbox(text(" Cantitate: "), input_cant->Render() | border) | hcenter,
            separator(),
            btn_vinde->Render() | hcenter,
            separator(),
            text(mesaj_vanzare) | bold | culoar_mesaj | hcenter
        });
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
    std::vector<std::unique_ptr<Produs>> produse_critice;
    int pagina_curenta_alerte = 0;
    const int ALERTE_PER_PAGINA = 8;

    // Butonul de interogare SQL
    auto btn_refresh_alerte = Button(" [ Cauta Produse cu Stoc Critic ] ", [&] {
        produse_critice = depozit.getProduseCuStocCritic();
        pagina_curenta_alerte = 0; // Resetăm la prima pagină la o nouă căutare
    });

    // Butoanele de navigare (Paginare)
    auto btn_inapoi_alerte = Button("< Pagina Anterioara", [&] { 
        if (pagina_curenta_alerte > 0) pagina_curenta_alerte--; 
    });
    auto btn_inainte_alerte = Button("Pagina Urmatoare >", [&] { 
        // Verificăm dacă mai avem elemente pentru pagina următoare
        if ((pagina_curenta_alerte + 1) * ALERTE_PER_PAGINA < produse_critice.size()) {
            pagina_curenta_alerte++; 
        }
    });

    // Grupăm butoanele
    auto layout_nav_alerte = Container::Horizontal({ btn_inapoi_alerte, btn_inainte_alerte });
    auto interactiuni_alerte = Container::Vertical({ btn_refresh_alerte, layout_nav_alerte });

    auto panou_alerte = Renderer(interactiuni_alerte, [&] {
        std::vector<std::vector<Element>> rows;
        rows.push_back({text("ID")|bold, text("Nume")|bold, text("Tip")|bold, text("Stoc")|bold, text("Prag")|bold});

        int total = produse_critice.size();

        if (total == 0) {
            rows.push_back({text("-"), text("STOCURI OPTIME SAU NEACTUALIZATE. Apasa butonul de mai sus."), text("-"), text("-"), text("-")});
        } else {
            // Calculăm ce bucată (slice) din vector afișăm pe pagina curentă
            int start_idx = pagina_curenta_alerte * ALERTE_PER_PAGINA;
            int end_idx = std::min(start_idx + ALERTE_PER_PAGINA, total);

            for (int i = start_idx; i < end_idx; ++i) {
                const auto& p = produse_critice[i];
                rows.push_back({
                    text(std::to_string(p->getId())),
                    text(p->getNume()),
                    text(p->getTipProdus()) | color(Color::Yellow),
                    text(std::to_string(p->getCantitate())) | color(Color::Red) | bold, 
                    text(std::to_string(p->getPragAlerta())) | color(Color::Cyan)
                });
            }
        }

        auto tabel = Table(rows);
        tabel.SelectAll().Border(LIGHT);
        tabel.SelectAll().Separator(LIGHT);
        tabel.SelectRow(0).Decorate(color(Color::Cyan));

        // Calculăm numărul total de pagini pentru display
        int total_pagini = (total > 0) ? (total + ALERTE_PER_PAGINA - 1) / ALERTE_PER_PAGINA : 1;

        return vbox({
            text(" ⚠️ RAPORT ALERTE (" + std::to_string(total) + " produse in pericol) ⚠️ ") | bold | color(Color::Red) | hcenter,
            text(" Pagina " + std::to_string(pagina_curenta_alerte + 1) + " din " + std::to_string(total_pagini)) | dim | hcenter,
            separator(),
            btn_refresh_alerte->Render() | hcenter,
            separator(),
            tabel.Render(),
            separator(),
            layout_nav_alerte->Render() | hcenter,
            filler()
        });
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

    auto tab_container = Container::Tab({ 
        panou_stoc,         
        panou_adaugare,      
        panou_vanzare,       
        panou_aprovizionare,
        panou_alerte,
        panou_cautare,
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