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

// Déclaration des constantes
const int TIME = 500000;
const char TETE = 'O';
const char CORPS = 'X';
const char ARRET = 'a';

const int HAUTEUR = 40;
const int LARGEUR = 80;
const int TAILLE_CIBLE = 20;
const int INITIALE_X = 40;
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
const int NOMBREPAVES = 4;

// Espace minimum entre les pavés et les bordures
const int ESPACEBORDURE = 2;


// Déclarations de fonctions pour les différentes fonctionnalités du programme
void afficher(int, int, char);                // Affiche un caractère à une position (x, y)
void effacer(int, int);                       // Efface un caractère à une position (x, y)
void dessinerSerpent(int[], int[]);           // Affiche le serpent sur l'écran
void progresser(int[], int[], char, bool*);   // Fait avancer le serpent vers la droite
void initPlateau(char[LARGEUR][HAUTEUR], int[], int[]);     // Initialiser le plateau de jeu avec les pavés
void dessinerPlateau(char[LARGEUR][HAUTEUR]); // Dessine le plateau initialisé
void ajouterPomme(char[LARGEUR][HAUTEUR], int[], int[]);    // Ajoute une pomme en fonction du plateau 

// Déclaration des fonctions données
void gotoXY(int, int);                        // Déplace le curseur à la position (x, y)
int kbhit();                                  // Fonction qui vérifie si une touche a été pressée
void disableEcho();                           // Fonction qui désactife l'echo
void enableEcho();                            // Fonction qui réactive l'echo

/*************************************************************
 *                       Bloque principale                    *
 **************************************************************/

int x_pomme; 
int y_pomme;
int TAILLESERPENT = 10;

int main()
{
    char direction = PREMIEREDIRECTION; // Initialisation de la direction du serpent
    disableEcho();
    // Efface l'écran et repositionne le curseur en haut de l'écran
    system("clear");
    printf("\033[H");
    
    int i, lesX[TAILLE_CIBLE], lesY[TAILLE_CIBLE]; // Variables pour les coordonnées du serpent
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
    initPlateau(plateau, lesX, lesY);
    dessinerPlateau(plateau);
    ajouterPomme(plateau, lesX, lesY);
    usleep(TIME); // Pause de 0,5 seconde pour permettre à l'utilisateur de voir le serpent

    test = false; // Initialise le booléen pour la boucle
    bool mange = false; // Initialise le booléen pour la procédure progresser
    while (test != true)
    {
        if (kbhit())
        {                        // Vérifie si une touche est pressée
            char ch = getchar(); // Lit la touche pressée
            if(ch == ARRET || TAILLESERPENT == TAILLE_CIBLE) {
                // Si la touche 'a' est pressée, la boucle se termine
                test=true;
            }

            // ne pas faire aller le serpend dans la direction opposée
            if(ch == DROITE && direction != GAUCHE) direction = ch;
            if(ch == GAUCHE && direction != DROITE) direction = ch;
            if(ch == HAUT && direction != BAS) direction = ch;
            if(ch == BAS && direction != HAUT) direction = ch;
        }

        int rapidite = TIME;

        progresser(lesX, lesY, direction, &mange); // Déplace le serpent d'une case vers la droite
        if (mange == true){
            rapidite = rapidite - 100000; 
            ajouterPomme(plateau, lesX, lesY);
            mange = false;
        }
        dessinerSerpent(lesX, lesY);            // Réaffiche le serpent à la nouvelle position
        usleep(rapidite);                           // Pause de 0,25 seconde pour animer le serpent
        if (plateau[lesX[0]][lesY[0]] == DELIMITATION){
            test = true;
        }
        for (int c = 1; c < TAILLESERPENT; c++){
            if (lesX[0] == lesX[c] && lesY[0] == lesY[c]){
                test = true;
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
    if (lesX[0] == x_pomme && lesY[0] == y_pomme) {
        *mange = true; // Si la tête du serpent rencontre une pomme
        TAILLESERPENT ++;
    } else {
        effacer(lesX[TAILLESERPENT - 1], lesY[TAILLESERPENT - 1]);
    }

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
    if (lesX[0] < 0){
        lesX[0] += LARGEUR;
    }
    if (lesX[0] > LARGEUR - 1){
        lesX[0] = 0;
    }
    if (lesY[0] < 0){
        lesY[0] += HAUTEUR;
    }
    if (lesY[0] > HAUTEUR - 1){
        lesY[0] = 0;
    }
}



void initPlateau(char plateau[LARGEUR][HAUTEUR], int lesX[], int lesY[]) {
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
    srand(time(NULL)); // Initialiser le générateur de nombres aléatoires

    bool test = false;
    for(int c = 0; c<NOMBREPAVES; c++) {
        // Générer une position aléatoire pour le coin supérieur gauche du pavé
        int x = rand() % (LARGEUR - TAILLEPAVE - ESPACEBORDURE * 2) + ESPACEBORDURE;
        int y = rand() % (HAUTEUR - TAILLEPAVE - ESPACEBORDURE * 2) + ESPACEBORDURE;

        while (test == false)
        {
            x = rand() % (LARGEUR - TAILLEPAVE - ESPACEBORDURE * 2) + ESPACEBORDURE;
            y = rand() % (HAUTEUR - TAILLEPAVE - ESPACEBORDURE * 2) + ESPACEBORDURE;
            test = true;
            for (int i = 0; i < TAILLESERPENT; i++){
                if (lesX[i] == x && lesY[i] == y){
                    test = false;
                } 
            }
        }

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

void ajouterPomme(char plateau[LARGEUR][HAUTEUR], int lesX[], int lesY[]){
    bool test = false;
    x_pomme = rand() % (LARGEUR - ESPACEBORDURE) + 1;
    y_pomme = rand() % (HAUTEUR - ESPACEBORDURE) + 1;
    while (plateau[x_pomme][y_pomme] == DELIMITATION || test == false) {
        x_pomme = rand() % (LARGEUR - ESPACEBORDURE) + 1;
        y_pomme = rand() % (HAUTEUR - ESPACEBORDURE) + 1;
        test = true;
        for (int i = 0; i < TAILLESERPENT; i++){
            if (lesX[i] == x_pomme && lesY[i] == y_pomme){
                test = false;
            } 
        }
    }
    afficher(x_pomme, y_pomme, '6');
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