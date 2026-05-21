import csv
import random

def genereaza_csv(nume_fisier="stoc.csv", numar_produse=10000):
    categorii = [
        ("Auto", ["Furtun Intercooler 2.0 TDI", "Rulment Roata Fata", "Filtru Aer", "Set Placute Frana", "Senzor Debitmetru", "Baterie 12V 70Ah"]),
        ("Electronice", ["[Electronice] Multimetru Analogic", "[Electronice] Modul Transmisie LoRa", "[Electronice] Osciloscop", "[Electronice] Sursa Laborator"]),
        ("Perisabile", ["[Perisabile] Spray Degripant", "[Perisabile] Solutie Curatare Frane", "[Perisabile] Adeziv Industrial"]),
        ("Hardware", ["Incarcator Gaz", "Trusa Tubulare", "Cheie Dinamometrica", "Set Imbusuri"])
    ]

    with open(nume_fisier, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        # Am adăugat 'VolumM3' la finalul header-ului
        writer.writerow(["ID", "Tip", "Nume", "Cantitate", "PragAlerta", "PretAchizitie", "PretVanzare", "VolumM3"])

        for id_produs in range(1, numar_produse + 1):
            categorie_aleasa = random.choice(categorii)
            tip = categorie_aleasa[0]
            nume = random.choice(categorie_aleasa[1]) + f" Model {random.randint(1, 99)}"
            
            cantitate = random.randint(0, 150)
            prag = random.randint(5, 20)
            pret_achizitie = round(random.uniform(10.0, 300.0), 2)
            pret_vanzare = round(pret_achizitie * random.uniform(1.2, 2.5), 2)
            
            # Generăm un volum random (ex: 0.045 m3)
            volum = round(random.uniform(0.01, 0.50), 3)

            writer.writerow([id_produs, tip, nume, cantitate, prag, pret_achizitie, pret_vanzare, volum])

    print(f"Gata! Fisierul '{nume_fisier}' a fost generat cu noua coloana VolumM3.")

if __name__ == "__main__":
    genereaza_csv()