#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

struct Carte {
    char titlu[100];
    char autor[100];
    char tip[50];
    char locatie[50];
    int disponibilitate;
    int zileImprumut;
};

void clearScreen() {
    system("cls");
}

void pauza() {
    printf("\nApasa Enter pentru a continua...");
    getchar();
}

void afisareMeniuPrincipal() {
    printf("=== SISTEM BIBLIOTECA ===\n");
    printf("1. Adaugare carte\n");
    printf("2. Cautare carte dupa titlu\n");
    printf("3. Cautare carte dupa autor\n");
    printf("4. Cautare carte dupa disponibilitate\n");
    printf("5. Imprumuta carte\n");
    printf("6. Returneaza carte\n");
    printf("7. Afisare toate cartile\n");
    printf("0. Iesire\n");
    printf("Selecteaza o optiune: ");
}

void citesteString(const char* mesaj, char* dest, int dim) {
    do {
        printf("%s", mesaj);
        fgets(dest, dim, stdin);
        dest[strcspn(dest, "\n")] = '\0';
        if (strlen(dest) == 0) {
            printf(RED "Campul nu poate fi gol. Incearca din nou.\n" RESET);
        }
    } while (strlen(dest) == 0);
}

int citesteInt(const char* mesaj, int min, int max) {
    int val;
    char buf[20];
    while (1) {
        printf("%s", mesaj);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d", &val) != 1 || val < min || val > max) {
            printf(RED "Valoare invalida. Introdu un numar intre %d si %d.\n" RESET, min, max);
        } else {
            return val;
        }
    }
}

void adaugaCarte() {
    FILE *fisier = fopen("carti.txt", "a");
    if (fisier == NULL) {
        printf(RED "Eroare la deschiderea fisierului!\n" RESET);
        return;
    }

    struct Carte carte;
    citesteString("Introdu titlul cartii (ex: Ion): ", carte.titlu, sizeof(carte.titlu));
    citesteString("Introdu numele autorului (ex: Liviu Rebreanu): ", carte.autor, sizeof(carte.autor));
    citesteString("Introdu tipul cartii (ex: roman, poezie): ", carte.tip, sizeof(carte.tip));
    citesteString("Introdu locatia in biblioteca (ex: raft 3): ", carte.locatie, sizeof(carte.locatie));

    carte.disponibilitate = 1;
    carte.zileImprumut = 0;

    fprintf(fisier, "%s,%s,%s,%s,%d,%d\n", carte.titlu, carte.autor, carte.tip, carte.locatie, carte.disponibilitate, carte.zileImprumut);
    fclose(fisier);

    printf(GREEN "\nCarte adaugata cu succes!\n" RESET);
}

int fisierExista(const char* nume) {
    FILE* f = fopen(nume, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

int comparareCaseInsensitive(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (tolower(*s1) != tolower(*s2)) {
            return 0;
        }
        s1++; s2++;
    }
    return *s1 == '\0' && *s2 == '\0';
}

int contineSubsirCaseInsensitive(const char* text, const char* subsir) {
    int lenText = strlen(text);
    int lenSubsir = strlen(subsir);

    for (int i = 0; i <= lenText - lenSubsir; i++) {
        if (strncasecmp(&text[i], subsir, lenSubsir) == 0) {
            return 1;
        }
    }
    return 0;
}

void afiseazaCarte(const struct Carte* carte) {
    printf("Titlu: " CYAN "%s\n" RESET, carte->titlu);
    printf("Autor: " YELLOW "%s\n" RESET, carte->autor);
    printf("Tip: %s\n", carte->tip);
    printf("Locatie: %s\n", carte->locatie);
    printf("Disponibil: %s\n", carte->disponibilitate ? GREEN "Da" RESET : RED "Nu" RESET);
    if (!carte->disponibilitate) {
        printf("Zile ramase imprumut: %d\n", carte->zileImprumut);
    }
    printf("--------------------------\n");
}

void afiseazaCarti() {
    if (!fisierExista("carti.txt")) {
        printf(RED "Fisierul carti.txt nu exista. Adauga o carte mai intai.\n" RESET);
        return;
    }

    FILE *fisier = fopen("carti.txt", "r");
    struct Carte carte;
    char linie[400];

    while (fgets(linie, sizeof(linie), fisier)) {
        sscanf(linie, "%[^,],%[^,],%[^,],%[^,],%d,%d",
               carte.titlu, carte.autor, carte.tip, carte.locatie, &carte.disponibilitate, &carte.zileImprumut);
        afiseazaCarte(&carte);
    }

    fclose(fisier);
}

void actualizeazaCarte(const char* titluCautat, int nouaDisponibilitate) {
    if (!fisierExista("carti.txt")) {
        printf(RED "Fisierul carti.txt nu exista.\n" RESET);
        return;
    }

    FILE *fisier = fopen("carti.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    struct Carte carte;
    char linie[400];
    int modificat = 0;

    while (fgets(linie, sizeof(linie), fisier)) {
        linie[strcspn(linie, "\n")] = '\0';
        sscanf(linie, "%[^,],%[^,],%[^,],%[^,],%d,%d",
               carte.titlu, carte.autor, carte.tip, carte.locatie, &carte.disponibilitate, &carte.zileImprumut);

        if (comparareCaseInsensitive(carte.titlu, titluCautat)) {
            carte.disponibilitate = nouaDisponibilitate;
            if (nouaDisponibilitate == 0) {
                carte.zileImprumut = citesteInt("Pentru cate zile vrei sa imprumuti cartea? (1-30): ", 1, 30);
            } else {
                carte.zileImprumut = 0;
            }
            modificat = 1;
        }

        fprintf(temp, "%s,%s,%s,%s,%d,%d\n", carte.titlu, carte.autor, carte.tip, carte.locatie, carte.disponibilitate, carte.zileImprumut);
    }

    fclose(fisier);
    fclose(temp);
    remove("carti.txt");
    rename("temp.txt", "carti.txt");

    if (!modificat) {
        printf(RED "Cartea nu a fost gasita.\n" RESET);
    }
}

void imprumutaCarte() {
    char titlu[100];
    citesteString("Introdu titlul cartii de imprumutat: ", titlu, sizeof(titlu));
    actualizeazaCarte(titlu, 0);
}

void returneazaCarte() {
    char titlu[100];
    citesteString("Introdu titlul cartii de returnat: ", titlu, sizeof(titlu));
    actualizeazaCarte(titlu, 1);
}

void cautaCarte() {
    if (!fisierExista("carti.txt")) {
        printf(RED "Fisierul carti.txt nu exista.\n" RESET);
        return;
    }

    FILE *fisier = fopen("carti.txt", "r");
    char titluCautat[100];
    citesteString("Introdu titlul cartii cautate: ", titluCautat, sizeof(titluCautat));

    struct Carte carte;
    char linie[400];
    int gasit = 0;

    while (fgets(linie, sizeof(linie), fisier)) {
        sscanf(linie, "%[^,],%[^,],%[^,],%[^,],%d,%d",
               carte.titlu, carte.autor, carte.tip, carte.locatie, &carte.disponibilitate, &carte.zileImprumut);

        if (contineSubsirCaseInsensitive(carte.titlu, titluCautat)) {
            afiseazaCarte(&carte);
            gasit = 1;
        }
    }

    if (!gasit) {
        printf(RED "Nu a fost gasita nicio carte cu titlul specificat.\n" RESET);
    }

    fclose(fisier);
}

void cautaCarteDupaAutor() {
    if (!fisierExista("carti.txt")) {
        printf(RED "Fisierul carti.txt nu exista.\n" RESET);
        return;
    }

    FILE *fisier = fopen("carti.txt", "r");
    char autorCautat[100];
    citesteString("Introdu numele autorului: ", autorCautat, sizeof(autorCautat));

    struct Carte carte;
    char linie[400];
    int gasit = 0;

    while (fgets(linie, sizeof(linie), fisier)) {
        sscanf(linie, "%[^,],%[^,],%[^,],%[^,],%d,%d",
               carte.titlu, carte.autor, carte.tip, carte.locatie, &carte.disponibilitate, &carte.zileImprumut);

        if (contineSubsirCaseInsensitive(carte.autor, autorCautat)) {
            afiseazaCarte(&carte);
            gasit = 1;
        }
    }

    if (!gasit) {
        printf(RED "Nu a fost gasita nicio carte scrisa de acest autor.\n" RESET);
    }

    fclose(fisier);
}

void cautaCarteDupaDisponibilitate() {
    if (!fisierExista("carti.txt")) {
        printf(RED "Fisierul carti.txt nu exista.\n" RESET);
        return;
    }

    FILE *fisier = fopen("carti.txt", "r");
    int disponibil = citesteInt("Introdu 1 pentru carti disponibile sau 0 pentru indisponibile: ", 0, 1);

    struct Carte carte;
    char linie[400];
    int gasit = 0;

    while (fgets(linie, sizeof(linie), fisier)) {
        sscanf(linie, "%[^,],%[^,],%[^,],%[^,],%d,%d",
               carte.titlu, carte.autor, carte.tip, carte.locatie, &carte.disponibilitate, &carte.zileImprumut);

        if (carte.disponibilitate == disponibil) {
            afiseazaCarte(&carte);
            gasit = 1;
        }
    }

    if (!gasit) {
        printf(RED "Nu au fost gasite carti cu aceasta disponibilitate.\n" RESET);
    }

    fclose(fisier);
}

int main() {
    int optiune;
    do {
        clearScreen();
        afisareMeniuPrincipal();
        if (scanf("%d", &optiune) != 1) {
            printf(RED "Optiune invalida! Introdu un numar intre 0 si 7.\n" RESET);
            while (getchar() != '\n');
            continue;
        }
        getchar();

        switch(optiune) {
            case 1:
                clearScreen();
                printf("Submeniu: Adaugare carte\n");
                adaugaCarte();
                pauza();
                break;
            case 2:
                clearScreen();
                printf("Submeniu: Cautare dupa titlu\n");
                cautaCarte();
                pauza();
                break;
            case 3:
                clearScreen();
                printf("Submeniu: Cautare dupa autor\n");
                cautaCarteDupaAutor();
                pauza();
                break;
            case 4:
                clearScreen();
                printf("Submeniu: Cautare dupa disponibilitate\n");
                cautaCarteDupaDisponibilitate();
                pauza();
                break;
            case 5:
                clearScreen();
                printf("Submeniu: Imprumuta carte\n");
                imprumutaCarte();
                pauza();
                break;
            case 6:
                clearScreen();
                printf("Submeniu: Returneaza carte\n");
                returneazaCarte();
                pauza();
                break;
            case 7:
                clearScreen();
                printf("Submeniu: Afisare carti\n");
                afiseazaCarti();
                pauza();
                break;
            case 0:
                printf(GREEN "La revedere!\n" RESET);
                break;
            default:
                printf(RED "Optiune invalida! Alege intre 0 si 7.\n" RESET);
        }
    } while(optiune != 0);
    return 0;
}
