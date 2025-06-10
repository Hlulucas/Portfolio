/*************************************************************
 *              Bibliothèque et appels de fonctions           *
 **************************************************************/

#include <stdio.h>   // Bibliothèque standard pour les fonctions d'entrée/sortie
#include <stdlib.h>  // Bibliothèque standard pour les fonctions système
#include <unistd.h>  // Bibliothèque pour les fonctions de pause (comme usleep)
#include <stdbool.h> // Bibliothèque pour le type booléen
#include <termios.h> // Bibliothèque pour configurer le terminal
#include <fcntl.h>   // Bibliothèque pour le contrôle des fichiers
#include <time.h>
#include <sys/time.h>

// Déclaration des constantes
const int TIME = 25000;
const char TETE = 'O';
const char CORPS = 'X';
const char ARRET = 'a';

const int HAUTEUR = 41;
const int LARGEUR = 81;
#define NB_POMMES 10
const int INITIALE_X = 39;
const int INITIALE_Y = 20;

const char DELIMITATION = '#';
const char ESPACE = ' ';

// Directions
const char HAUT = 'z';
const char BAS = 's';
const char DROITE = 'd';
const char GAUCHE = 'q';

const char PREMIEREDIRECTION = DROITE;

// Taille d'un pavé
const int TAILLEPAVE = 5;

// Nombre de pavés à placer
#define NOMBREPAVES 6

// Espace minimum entre les pavés et les bordures
const int ESPACEBORDURE = 2;


// Déclarations de fonctions pour les différentes fonctionnalités du programme
void afficher(int, int, char);                // Affiche un caractère à une position (x, y)
void effacer(int, int);                       // Efface un caractère à une position (x, y)
void dessinerSerpent(int[], int[]);           // Affiche le serpent sur l'écran
void progresser(int[], int[], char, bool*);   // Fait avancer le serpent vers la droite
void initPlateau(char[LARGEUR][HAUTEUR]);     // Initialiser le plateau de jeu avec les pavés
void dessinerPlateau(char[LARGEUR][HAUTEUR]); // Dessine le plateau initialisé
void ajouterPomme(int);    // Ajoute une pomme en fonction du plateau 
char calculerDeplacement(char[LARGEUR][HAUTEUR], int[], int[], char, int, int); // Calcule les déplacements du serpent
bool checkProchainDeplacement(char[LARGEUR][HAUTEUR], int[], int[], int, int);
char demiTour(char[LARGEUR][HAUTEUR], int[], int[], char);
void plusProchePortail(int[], int[], int*, int*);

// Déclaration des fonctions données
void gotoXY(int, int);                        // Déplace le curseur à la position (x, y)
int kbhit();                                  // Fonction qui vérifie si une touche a été pressée
void disableEcho();                           // Fonction qui désactife l'echo
void enableEcho();                            // Fonction qui réactive l'echo

/*************************************************************
 *                       Bloque principale                    *
 **************************************************************/

int lesPommesX[NB_POMMES] = {75, 75, 78, 2, 8, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] = { 8, 39, 2, 2, 5, 39, 33, 38, 35, 2};
int lesPavesX[NOMBREPAVES] = { 3, 74, 3, 74, 38, 38};
int lesPavesY[NOMBREPAVES] = { 3, 3, 34, 34, 21, 15};
int x_pomme;
int y_pomme;
int TAILLESERPENT = 10;
int compt = 0;
int c_deplacement = 0;
bool portail_passe = false;

int main()
{
    char direction = PREMIEREDIRECTION; // Initialisation de la direction du serpent
    disableEcho();
    // Efface l'écran et repositionne le curseur en haut de l'écran
    system("clear");
    printf("\033[H");
    
    int i, lesX[TAILLESERPENT], lesY[TAILLESERPENT]; // Variables pour les coordonnées du serpent
    char plateau[LARGEUR][HAUTEUR];
    bool test;                 // Booléen pour la boucle principale

    // Initialise les positions de chaque segment du serpent
    for (i = 0; i < TAILLESERPENT; i++)
    {
        lesX[i] = INITIALE_X - i; // Position x de chaque segment
        lesY[i] = INITIALE_Y;     // Position y de chaque segment
    }

    // Affiche le serpent pour la première fois
    dessinerSerpent(lesX, lesY);
    initPlateau(plateau);
    dessinerPlateau(plateau);
    ajouterPomme(compt);
    usleep(TIME); // Pause de 0,5 seconde pour permettre à l'utilisateur de voir le serpent

    int x_obj, y_obj;
    plusProchePortail(lesX, lesY, &x_obj, &y_obj);

    test = false; // Initialise le booléen pour la boucle
    bool mange = false; // Initialise le booléen pour la procédure progresser
    struct timeval debut_temps, fin_temps;
    gettimeofday(&debut_temps, NULL);
    while (test != true)
    {
        if (kbhit())
        {                        // Vérifie si une touche est pressée
            char ch = getchar(); // Lit la touche pressée
            if(ch == ARRET) {
                // Si la touche 'a' est pressée, la boucle se termine
                test=true;
            }
        }
        
        c_deplacement = c_deplacement + 1;

        if (mange == true){
            compt = compt + 1;
            if (compt == 10){
                test = true;
            } else {
                ajouterPomme(compt);
                mange = false;
                plusProchePortail(lesX, lesY, &x_obj, &y_obj);
            }
        }

        if (portail_passe == true){
            x_obj = lesPommesX[compt];
            y_obj = lesPommesY[compt];
            portail_passe = false;
        }

        direction = calculerDeplacement(plateau, lesX, lesY, direction, x_obj, y_obj);

        progresser(lesX, lesY, direction, &mange); // Déplace le serpent d'une case vers la droite
        
        dessinerSerpent(lesX, lesY);            // Réaffiche le serpent à la nouvelle position
        usleep(TIME);                           // Pause de 0,25 seconde pour animer le serpent
        if (plateau[lesX[0]][lesY[0]] == DELIMITATION){
            test = true;
        }

        for (int c = 1; c < TAILLESERPENT; c++){
            if (lesX[0] == lesX[c] && lesY[0] == lesY[c]){
                test = true;
            }
        }
    }
    gettimeofday(&fin_temps, NULL);
    if (compt == NB_POMMES){
        system("clear");
        test = false;
        double time_in_seconds = (fin_temps.tv_sec - debut_temps.tv_sec) + (fin_temps.tv_usec - debut_temps.tv_usec) / 1000000.0;        
        double time_in_minutes = time_in_seconds / 60.0;
        printf("Fini avec un nombre de déplacements unitaires de : %d coups\n", c_deplacement);
        printf("Et en %.6f secondes, et en %.6f minutes\n", time_in_seconds, time_in_minutes);
        printf("Pour quitter, appuyez sur a\n");
        while (test != true)
        {
            if (kbhit())
            {
                char ch = getchar(); // Lit la touche pressée
                if(ch == ARRET) {
                    // Si la touche 'a' est pressée, la boucle se termine
                    test=true;
                }
            }
        }
    }
    system("clear");
    enableEcho();
    return 0; // Fin du programme
}

/*************************************************************
 *                    Fonctions créées                        *
 **************************************************************/

// Fonction qui affiche un caractère 'c' à la position (x, y) de l'écran
void afficher(int x, int y, char c)
{
    gotoXY(x+1, y+1);    // Déplace le curseur
    printf("%c", c); // Affiche le caractère
    fflush(stdout);  // Vide le buffer pour afficher immédiatement
}

// Fonction qui efface le caractère à la position (x, y) en affichant un espace
void effacer(int x, int y)
{
    gotoXY(x+1, y+1);   // Déplace le curseur
    printf(" ");    // Affiche un espace pour effacer
    fflush(stdout); // Vide le buffer
}

// Fonction qui dessine le serpent à partir des tableaux de positions lesX et lesY
void dessinerSerpent(int lesX[], int lesY[])
{
    effacer(lesX[9], lesY[9]); // Efface la queue du serpent (dernier segment)

    afficher(lesX[0], lesY[0], TETE); // Affiche la tête du serpent
    for (int i = 1; i < TAILLESERPENT; i++)
    { // Affiche le corps du serpent
        afficher(lesX[i], lesY[i], CORPS);
    }
}

// Fonction qui fait progresser le serpent d'une case vers la droite
void progresser(int lesX[], int lesY[], char direction, bool * mange) {

    effacer(lesX[TAILLESERPENT - 1], lesY[TAILLESERPENT - 1]);

    // Déplace chaque segment vers la position du segment précédent
    for (int i = TAILLESERPENT - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Gestion des directions pour déplacer la tête
    if (direction == DROITE) {
        lesX[0] += 1;
    }
    if (direction == GAUCHE) {
        lesX[0] -= 1;
    }
    if (direction == HAUT) {
        lesY[0] -= 1;
    }
    if (direction == BAS) {
        lesY[0] += 1;
    }

    // Portails
    if (lesX[0] < 0){
        lesX[0] += LARGEUR;
        portail_passe = true;
    }
    if (lesX[0] > LARGEUR - 1){
        lesX[0] = 0;
        portail_passe = true;
    }
    if (lesY[0] < 0){
        lesY[0] += HAUTEUR;
        portail_passe = true;
    }
    if (lesY[0] > HAUTEUR - 1){
        lesY[0] = 0;
        portail_passe = true;
    }

    if (lesX[0] == x_pomme && lesY[0] == y_pomme) {
        *mange = true;
    } 
}



void initPlateau(char plateau[LARGEUR][HAUTEUR]) {
    // Initialiser le plateau vide
    for (int i = 0; i < LARGEUR; i++) {
        for (int y = 0; y < HAUTEUR; y++) {
            if (i == 0 || y == 0 || i == LARGEUR - 1 || y == HAUTEUR - 1) {
                plateau[i][y] = DELIMITATION;
            } else {
                plateau[i][y] = ESPACE;
            }
        }
    }
    plateau[LARGEUR/2-1][0] = ESPACE;
    plateau[LARGEUR/2-1][HAUTEUR - 1] = ESPACE;
    plateau[0][HAUTEUR/2-1] = ESPACE;
    plateau[LARGEUR - 1][HAUTEUR/2-1] = ESPACE;

    // Générer les pavés
    for(int c = 0; c<NOMBREPAVES; c++) {
        // Générer une position aléatoire pour le coin supérieur gauche du pavé
        int x = lesPavesX[c];
        int y = lesPavesY[c];

        // Placer le pavé
        for (int i = x; i < x + TAILLEPAVE; i++) {
            for (int j = y; j < y + TAILLEPAVE; j++) {
                plateau[i][j] = DELIMITATION;
            }
        }
    }
}


void dessinerPlateau(char plateau[LARGEUR][HAUTEUR]){
    for (int i = 0; i < LARGEUR; i++){
        for (int y = 0; y < HAUTEUR; y++){
            afficher(i, y, plateau[i][y]);
        }
    }
}

void ajouterPomme(int compt){
    x_pomme = lesPommesX[compt];
    y_pomme = lesPommesY[compt];
    afficher(x_pomme, y_pomme, '6');
}

char calculerDeplacement(char plateau[LARGEUR][HAUTEUR], int lesX[], int lesY[], char direction, int x_obj, int y_obj){
    if (lesX[0] < x_obj){
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0]+1, lesY[0])) return DROITE;
    } 
    if (lesX[0] > x_obj){
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0]-1, lesY[0])) return GAUCHE;
    } 
    if (lesY[0] < y_obj){
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]+1)) return BAS;
    } 
    if (lesY[0] > y_obj){
        if(checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]-1)) return HAUT;
    }
    return demiTour(plateau, lesX, lesY, direction);
}

bool checkProchainDeplacement(char plateau[LARGEUR][HAUTEUR], int lesX[], int lesY[], int x, int y){
    for (int i = 1; i < TAILLESERPENT; i ++){
        if(lesX[i] == x && lesY[i] == y){
            return false;
        }
    } 
    if(plateau[x][y] == DELIMITATION){
        return false;
    }
    return true;
}

char demiTour(char plateau[LARGEUR][HAUTEUR], int lesX[], int lesY[], char direction){
    if (direction == HAUT || direction == BAS){
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0]+1, lesY[0]) && checkProchainDeplacement(plateau, lesX, lesY, lesX[0]+2, lesY[0])) return DROITE;
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0]-1, lesY[0]) && checkProchainDeplacement(plateau, lesX, lesY, lesX[0]-2, lesY[0])) return GAUCHE;
    } else{
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]+1) && checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]+2)) return BAS;
        if(checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]-1) && checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]-2)) return HAUT;
    }

    if (direction == HAUT || direction == BAS){
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0]+1, lesY[0])) return DROITE;
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0]-1, lesY[0])) return GAUCHE;
    } else{
        if (checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]+1)) return BAS;
        if(checkProchainDeplacement(plateau, lesX, lesY, lesX[0], lesY[0]-1)) return HAUT;
    }
    return direction;
}

void plusProchePortail(int lesX[], int lesY[], int * x_obj, int * y_obj) {
    int x_tete = lesX[0];
    int y_tete = lesY[0];
    
    int x_pomme = lesPommesX[compt];
    int y_pomme = lesPommesY[compt];
    
    int distance_pomme = abs(x_tete - x_pomme) + abs(y_tete - y_pomme);
    
    int portails[4][2] = {
        {LARGEUR / 2 - 1, 0},
        {LARGEUR / 2 - 1, HAUTEUR - 1},
        {0, HAUTEUR / 2 - 1},
        {LARGEUR - 1, HAUTEUR / 2 - 1}
    };

    int sorties[4][2] = {
        {LARGEUR / 2 - 1, HAUTEUR - 1}, 
        {LARGEUR / 2 - 1, 0},           
        {LARGEUR - 1, HAUTEUR / 2 - 1}, 
        {0, HAUTEUR / 2 - 1}            
    };

    int distance_min_totale = distance_pomme;
    int x_portail_cible = x_tete;
    int y_portail_cible = y_tete;
    bool portail_utilise = false;

    for (int i = 0; i < 4; i++) {
        int x_portail = portails[i][0];
        int y_portail = portails[i][1];
        int distance_vers_portail = abs(x_tete - x_portail) + abs(y_tete - y_portail);
        
        int x_sortie = sorties[i][0];
        int y_sortie = sorties[i][1];
        int distance_sortie_vers_pomme = abs(x_sortie - x_pomme) + abs(y_sortie - y_pomme);
        
        int distance_totale = distance_vers_portail + distance_sortie_vers_pomme;
        
        if (distance_totale < distance_min_totale) {
            distance_min_totale = distance_totale;
            x_portail_cible = x_portail;
            y_portail_cible = y_portail;
            portail_utilise = true;
        }
    }

    if (portail_utilise) {
        *x_obj = x_portail_cible;
        *y_obj = y_portail_cible;
    } else {
        *x_obj = x_pomme;
        *y_obj = y_pomme;
    }
}

/*************************************************************
 *                       Fonctions données                    *
 **************************************************************/

// Fonction pour déplacer le curseur à une position (x, y) dans le terminal
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y, x); // Utilise les séquences ANSI pour déplacer le curseur
}

// Fonction qui détecte si une touche a été appuyée sans bloquer le programme
int kbhit()
{
    int unCaractere = 0;       // Indicateur si une touche est pressée
    struct termios oldt, newt; // Structures pour la configuration du terminal
    int ch;                    // Variable pour stocker la touche
    int oldf;                  // Mode du terminal

    tcgetattr(STDIN_FILENO, &oldt); // Sauvegarde la configuration actuelle du terminal
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Désactive l'écho et le mode canonique
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Applique la nouvelle configuration
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK); // Active le mode non-bloquant

    ch = getchar(); // Lit la touche

    // Restaure la configuration du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {                      // Si une touche a été pressée
        ungetc(ch, stdin); // Remet la touche dans le flux d'entrée
        unCaractere = 1;   // Indique qu'une touche est disponible
    }
    return unCaractere; // Retourne 1 si une touche est disponible, sinon 0
}

// Fonction qui enlève l'echo dans le terminal lors d'une touche pressée
void disableEcho()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Fonction qui réactive l'echo
void enableEcho()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}