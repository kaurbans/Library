#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struktura przechowujaca informacje o ksi¹¿ce
typedef struct {
    char tytul[100];
    char autor[100];
    int rok_wydania;
    char typ_okladki[20];
    int liczba_stron;
    char rodzaj_ksiazki[50];
} Ksiazka;

// Struktura bazy danych ksi¹¿ek
typedef struct {
    Ksiazka* ksiazki;
    int ilosc_ksiazek;
} BazaKsiazek;

// Funkcja dodaj¹ca now¹ ksi¹¿kê do bazy danych
void dodaj_ksiazke(BazaKsiazek* baza, Ksiazka nowa_ksiazka) {
    baza->ilosc_ksiazek++;
    Ksiazka* temp = (Ksiazka*)realloc(baza->ksiazki, baza->ilosc_ksiazek * sizeof(Ksiazka));
    if (temp == NULL) {
        printf("Blad alokacji pamieci!\n");
        exit(1);
    }
    baza->ksiazki = temp;
    baza->ksiazki[baza->ilosc_ksiazek - 1] = nowa_ksiazka;
}

// Funkcja wczytuj¹ca bazê ksi¹¿ek z pliku binarnego
BazaKsiazek wczytaj_baze(const char* nazwa_pliku) {
    FILE* plik;
    BazaKsiazek baza = { NULL, 0 };

    if (fopen_s(&plik, nazwa_pliku, "rb") == 0) {
        while (!feof(plik)) {
            Ksiazka temp;
            if (fread(&temp, sizeof(Ksiazka), 1, plik)) {
                dodaj_ksiazke(&baza, temp);
            }
        }
        fclose(plik);
    }

    return baza;
}

// Funkcja zapisuj¹ca bazê ksi¹¿ek do pliku binarnego
void zapisz_baze(const char* nazwa_pliku, BazaKsiazek baza) {
    FILE* plik;
    if (fopen_s(&plik, nazwa_pliku, "wb") == 0) {
        fwrite(baza.ksiazki, sizeof(Ksiazka), baza.ilosc_ksiazek, plik);
        fclose(plik);
    }
}

// Funkcja wyœwietlaj¹ca wszystkie ksi¹¿ki z bazy danych
void wyswietl_baze(BazaKsiazek baza) {
    printf("| %-5s | %-30s | %-30s | %-10s | %-10s | %-10s | %-20s |\n",
        "Indeks", "Tytul", "Autor", "Rok", "Okladka", "Strony", "Rodzaj");
    printf("--------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < baza.ilosc_ksiazek; i++) {
        printf("| %-5d | %-30s | %-30s | %-10d | %-10s | %-10d | %-20s |\n",
            i + 1, baza.ksiazki[i].tytul, baza.ksiazki[i].autor,
            baza.ksiazki[i].rok_wydania, baza.ksiazki[i].typ_okladki,
            baza.ksiazki[i].liczba_stron, baza.ksiazki[i].rodzaj_ksiazki);
    }
    printf("--------------------------------------------------------------------------------------------\n");
}

// Funkcja wyszukuj¹ca ksi¹¿ki po nazwisku autora
void wyszukaj_po_autorze(BazaKsiazek baza, const char* nazwisko) {
    printf("Ksiazki autora %s:\n", nazwisko);
    for (int i = 0; i < baza.ilosc_ksiazek; i++) {
        if (strstr(baza.ksiazki[i].autor, nazwisko) != NULL) {
            printf("%s\n", baza.ksiazki[i].tytul);
        }
    }
}

// Funkcja wyszukuj¹ca ksi¹¿ki po s³owie kluczowym w tytule
void wyszukaj_po_tytule(BazaKsiazek baza, const char* klucz) {
    printf("Ksiazki zawierajace \"%s\" w tytule:\n", klucz);
    for (int i = 0; i < baza.ilosc_ksiazek; i++) {
        if (strstr(baza.ksiazki[i].tytul, klucz) != NULL) {
            printf("%s\n", baza.ksiazki[i].tytul);
        }
    }
}

// Funkcja usuwaj¹ca ksi¹¿kê z bazy danych
void usun_ksiazke(BazaKsiazek* baza, int indeks) {
    if (indeks >= 0 && indeks < baza->ilosc_ksiazek) {
        printf("Czy na pewno chcesz usunac ksiazke \"%s\"? (t/n): ", baza->ksiazki[indeks].tytul);
        char potwierdzenie;
        scanf_s(" %c", &potwierdzenie, 1);
        if (potwierdzenie == 't' || potwierdzenie == 'T') {
            for (int i = indeks; i < baza->ilosc_ksiazek - 1; i++) {
                baza->ksiazki[i] = baza->ksiazki[i + 1];
            }
            baza->ilosc_ksiazek--;
            Ksiazka* temp = (Ksiazka*)realloc(baza->ksiazki, baza->ilosc_ksiazek * sizeof(Ksiazka));
            if (temp == NULL && baza->ilosc_ksiazek > 0) {
                printf("Blad alokacji pamieci!\n");
                exit(1);
            }
            baza->ksiazki = temp;
            printf("Ksiazka zostala usunieta.\n");
        }
        else {
            printf("Anulowano usuwanie ksiazki.\n");
        }
    }
    else {
        printf("Nieprawidlowy indeks.\n");
    }
}

// Porównywacz dla funkcji sortuj¹cej
int porownaj_autorow(const void* a, const void* b) {
    return strcmp(((Ksiazka*)a)->autor, ((Ksiazka*)b)->autor);
}

// Funkcja sortuj¹ca ksi¹¿ki po autorze
void sortuj_po_autorze(BazaKsiazek* baza) {
    qsort(baza->ksiazki, baza->ilosc_ksiazek, sizeof(Ksiazka), porownaj_autorow);
}

// Porównywacz dla funkcji sortuj¹cej
int porownaj_tytuly(const void* a, const void* b) {
    return strcmp(((Ksiazka*)a)->tytul, ((Ksiazka*)b)->tytul);
}

// Funkcja sortuj¹ca ksi¹¿ki po tytule
void sortuj_po_tytule(BazaKsiazek* baza) {
    qsort(baza->ksiazki, baza->ilosc_ksiazek, sizeof(Ksiazka), porownaj_tytuly);
}

int main() {
    BazaKsiazek baza = wczytaj_baze("baza.bin");

    int opcja = -1; // Inicjowanie „opcji”, aby unikn¹æ ostrze¿enia o niezainicjowanej zmiennej
    do {
        printf("\n--- MENU ---\n");
        printf("1. Dodaj ksiazke\n");
        printf("2. Wyswietl ksiazki\n");
        printf("3. Wyszukaj ksiazki po autorze\n");
        printf("4. Wyszukaj ksiazki po slowie kluczowym w tytule\n");
        printf("5. Usun ksiazke\n");
        printf("6. Sortuj ksiazki po autorze\n");
        printf("7. Sortuj ksiazki po tytule\n");
        printf("0. Wyjscie\n");
        printf("Wybierz opcje: ");
        scanf_s("%d", &opcja);
        while (getchar() != '\n');  // Usuniêcie pozosta³ego znaku nowej linii z bufora

        switch (opcja) {
        case 1: {
            Ksiazka nowa_ksiazka;
            printf("Tytul: ");
            fgets(nowa_ksiazka.tytul, sizeof(nowa_ksiazka.tytul), stdin);
            nowa_ksiazka.tytul[strcspn(nowa_ksiazka.tytul, "\n")] = 0; // Usuniêcie znaku nowej linii
            printf("Autor: ");
            fgets(nowa_ksiazka.autor, sizeof(nowa_ksiazka.autor), stdin);
            nowa_ksiazka.autor[strcspn(nowa_ksiazka.autor, "\n")] = 0;
            printf("Rok wydania: ");
            scanf_s("%d", &nowa_ksiazka.rok_wydania);
            while (getchar() != '\n');  // Usuniêcie pozosta³ego znaku nowej linii z bufora
            printf("Typ okladki: ");
            fgets(nowa_ksiazka.typ_okladki, sizeof(nowa_ksiazka.typ_okladki), stdin);
            nowa_ksiazka.typ_okladki[strcspn(nowa_ksiazka.typ_okladki, "\n")] = 0;
            printf("Liczba stron: ");
            scanf_s("%d", &nowa_ksiazka.liczba_stron);
            while (getchar() != '\n');  // Usuniêcie pozosta³ego znaku nowej linii z bufora
            printf("Rodzaj ksiazki: ");
            fgets(nowa_ksiazka.rodzaj_ksiazki, sizeof(nowa_ksiazka.rodzaj_ksiazki), stdin);
            nowa_ksiazka.rodzaj_ksiazki[strcspn(nowa_ksiazka.rodzaj_ksiazki, "\n")] = 0;

            dodaj_ksiazke(&baza, nowa_ksiazka);
            zapisz_baze("baza.bin", baza);
            break;
        }
        case 2:
            wyswietl_baze(baza);
            break;
        case 3: {
            char nazwisko[100];
            printf("Podaj nazwisko autora: ");
            fgets(nazwisko, sizeof(nazwisko), stdin);
            nazwisko[strcspn(nazwisko, "\n")] = 0;
            wyszukaj_po_autorze(baza, nazwisko);
            break;
        }
        case 4: {
            char klucz[100];
            printf("Podaj slowo kluczowe: ");
            fgets(klucz, sizeof(klucz), stdin);
            klucz[strcspn(klucz, "\n")] = 0;
            wyszukaj_po_tytule(baza, klucz);
            break;
        }
        case 5: {
            int indeks;
            printf("Podaj indeks ksiazki do usuniecia: ");
            scanf_s("%d", &indeks);
            while (getchar() != '\n');  // Usuniêcie pozosta³ego znaku nowej linii z bufora
            usun_ksiazke(&baza, indeks - 1);
            zapisz_baze("baza.bin", baza);
            break;
        }
        case 6:
            sortuj_po_autorze(&baza);
            zapisz_baze("baza.bin", baza);
            break;
        case 7:
            sortuj_po_tytule(&baza);
            zapisz_baze("baza.bin", baza);
            break;
        case 0:
            printf("Do widzenia!\n");
            break;
        default:
            printf("Nieprawidlowa opcja!\n");
            break;
        }
    } while (opcja != 0);

    // oczyszczenie pamieci
    free(baza.ksiazki);

    return 0;
}
