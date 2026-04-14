# Sistem de Monitorizare Depozit (C++ / Linux)

Acesta este un sistem modern de gestiune a inventarului, dezvoltat în C++17, optimizat pentru medii Linux/WSL. Proiectul utilizează containere STL avansate și algoritmi eficienți pentru monitorizarea produselor în timp real.

## Caracteristici Principale
- **Gestiune Produse:** Adăugare, eliminare și actualizare stoc.
- **Performanță:** Utilizarea `std::unordered_map` pentru căutări instantanee (O(1)).
- **Sistem de Alerte:** Notificări automate pentru produsele care scad sub pragul de alertă.
- **Programare Defensivă:** Gestionarea erorilor prin excepții (`try-catch`).
- **Standard Profesional:** Structură modulară (Header/Source) și build system cu CMake.

## Tehnologii utilizate
- **Limbaj:** C++17 (Standard modern)
- **Build System:** CMake 3.10+
- **Platformă:** Linux (Testat pe Ubuntu 24.04 via WSL)
- **Control Versiune:** Git (GitHub Organization)

## Structura Proiectului
- `include/`: Fișiere header (.hpp) - Declarațiile claselor.
- `src/`: Fișiere sursă (.cpp) - Implementarea logicii.
- `CMakeLists.txt`: Configurația de build.
- `build/`: Fișierele binare rezultate (exclus din Git).

## Cum se compilează și rulează

Pentru a rula proiectul pe un sistem Linux, urmează pașii de mai jos:

1. **Configurare CMake:**
   ```bash
   cmake -B build