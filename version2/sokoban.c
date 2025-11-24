/**
* @file sokoban.c
* @brief Jeu du Sokoban
* @author Evan BENOIT
* @version 1.0
* @date 8/11/2025
*
* Le programme reprend le jeu du Sokoban où un petit bonhomme (identifié
* ici par un '@' doit pousser des caisses sur des cibles
* respectivement '$' et '.' pour gagner la partie).
*
*/

/* Fichiers inclus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

/* Déclaration des constantes */
#define NB_COLONNES 12
#define NB_LIGNES 12
#define NB_DEPLACEMENTS_MAX 1000
const int ZERO=0;
const int MILLE=1000;
const int TAILLE=12;
const int TAILLE_FICHIER=20;
const int ENLEVER=-1;
const int AJOUTER=1;
const int DOUBLE=2;
const int INVERSE=-1;
const int ZOOM1=1;
const int ZOOM2=2;
const int ZOOM3=3;
const char ARRETER='x';
const char RECOMMENCER='r';
const char GAUCHE='q';
const char DROITE='d';
const char HAUT='z';
const char BAS='s';
const char ZOOM='+';
const char DEZOOM='-';
const char RETOUR='u';
const char VALIDATION='O';
const char SOKOBAN='@';
const char MUR='#';
const char RIEN=' ';
const char CAISSE='$';
const char CIBLE='.';
const char SOKOBAN_GAUCHE='g';
const char SOKOBAN_DROITE='d';
const char SOKOBAN_HAUT='h';
const char SOKOBAN_BAS='b';
const char SOKOBAN_CAISSE_GAUCHE='G';
const char SOKOBAN_CAISSE_DROITE='D';
const char SOKOBAN_CAISSE_HAUT='H';
const char SOKOBAN_CAISSE_BAS='B';
const char AUCUN_DEPLACEMENT=' ';
const char SOKOBAN_CIBLE='+';
const char CAISSE_CIBLE='*';
const char ATTENTE='\0';

typedef char t_Plateau[NB_LIGNES][NB_COLONNES];
typedef char t_tabDeplacement[NB_DEPLACEMENTS_MAX];

/*
* Tout au long du programme les lignes pourront être suivies d'un retour à la
* ligne et d'une indentation car elles font à elles seules plus de
* 80 caractères mais forment bien une et une execution
*/

/* Déclaration des fonctions */
/**
 * @brief Procédure qui fait tourner le jeu principal
 * @param toucheAppuyee Adresse de la touche appuyée par le joueur
 * @param plateau Plateau du jeu
 * @param fichier Nom du fichier contenant la partie
 * @param ligSok Ligne où se trouve Sokoban
 * @param colSok Colonne où se trouve Sokoban
 * @param nbDepla Nombre de déplacements déjà effectués
 * @param zoom Niveau de zoom choisi
 * @param histoDepla Historique des déplacements
 */
void jeu(char * toucheAppuyee, t_Plateau plateau, char fichier[], int ligSok, int colSok, int nbDepla, int zoom, t_tabDeplacement histoDepla);

/**
 * @brief Affiche le plateau selon le niveau de zoom
 * @param plateau Plateau du jeu
 * @param zoom Niveau de zoom choisi
 */
void afficher_plateau(t_Plateau plateau, int zoom);

/**
 * @brief Charge un plateau à partir d'un fichier
 * @param plateau Plateau du jeu à remplir
 * @param fichier Nom du fichier source
 */
void charger_partie(t_Plateau plateau, char fichier[]);

/**
 * @brief Enregistre un plateau dans un fichier
 * @param plateau Plateau à sauvegarder
 * @param fichier Nom du fichier de destination
 */
void enregistrer_partie(t_Plateau plateau, char fichier[]);

/**
 * @brief Affiche l'en-tête du jeu avec les informations essentielles
 * @param nbDepla Nombre de déplacements effectués
 * @param nomFich Nom du fichier de la partie
 */
void affichier_entete(int nbDepla, char nomFich[]);

/**
 * @brief Regroupe les déplacements possibles et les applique
 * @param plateau Plateau du jeu
 * @param ligSok Coordonnée ligne de Sokoban
 * @param colSok Coordonnée colonne de Sokoban
 * @param deplacement Touche correspondant au mouvement
 * @param nbDepla Nombre de déplacements effectués
 * @param histoDepla Historique des déplacements
 */
void deplacer(t_Plateau plateau, int * ligSok, int * colSok, char deplacement, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Déplace Sokoban en modifiant ses coordonnées
 * @param nvLig Variation de ligne
 * @param nvCol Variation de colonne
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param nbDepla Adresse du compteur de déplacements
 */
void deplacement_sokoban(int nvLig, int nvCol, int * ligSok, int * colSok, int * nbDepla);

/**
 * @brief Permet de recommencer la partie depuis le fichier original
 * @param nbDepla Adresse du compteur de déplacements
 * @param plateauDeJeu Plateau du jeu
 * @param nomFichier Nom du fichier chargé
 * @param ligneSokoban Adresse de la ligne de Sokoban
 * @param colonneSokoban Adresse de la colonne de Sokoban
 * @param histoDepla Historique des déplacements
 */
void recommencer(int * nbDepla, t_Plateau plateauDeJeu, char nomFichier[], int * ligneSokoban, int * colonneSokoban, t_tabDeplacement histoDepla);

/**
 * @brief Procédure permettant d'abandonner la partie
 * @param plateauDeJeu Plateau actuel du joueur
 */
void abandon(t_Plateau plateauDeJeu);

/**
 * @brief Déplacement simple de Sokoban si aucune case spéciale n'est présente
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément de ligne
 * @param incrColSok Incrément de colonne
 * @param nbDepla Adresse du nombre de déplacements
 * @param histoDepla Historique des déplacements
 * @param deplacement Caractère représentant le mouvement
 */
void deplacement_rien(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int * nbDepla, t_tabDeplacement histoDepla, char deplacement);

/**
 * @brief Gère le déplacement d'une caisse devant Sokoban
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément de ligne
 * @param incrColSok Incrément de colonne
 * @param doubleIncrLigSok Double incrément de ligne
 * @param doubleIncrColSok Double incrément de colonne
 * @param nbDepla Adresse du nombre de déplacements
 * @param histoDepla Historique des déplacements
 * @param deplacement Caractère représentant le mouvement
 */
void deplacement_caisse(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int doubleIncrLigSok, int doubleIncrColSok, int * nbDepla, t_tabDeplacement histoDepla, char deplacement);

/**
 * @brief Gère le déplacement d'une caisse vers une cible
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément de ligne
 * @param incrColSok Incrément de colonne
 * @param doubleIncrLigSok Double incrément de ligne
 * @param doubleIncrColSok Double incrément de colonne
 * @param nbDepla Adresse du nombre de déplacements
 * @param histoDepla Historique des déplacements
 * @param deplacement Caractère représentant le mouvement
 */
void deplacement_caisse_cible(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int doubleIncrLigSok, int doubleIncrColSok, int * nbDepla, t_tabDeplacement histoDepla, char deplacement);

/**
 * @brief Déplace Sokoban sur une case cible
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément de ligne
 * @param incrColSok Incrément de colonne
 * @param nbDepla Adresse du nombre de déplacements
 * @param histoDepla Historique des déplacements
 * @param deplacement Caractère représentant le mouvement
 */
void deplacement_cible(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int * nbDepla, t_tabDeplacement histoDepla, char deplacement);

/**
 * @brief Annule le dernier déplacement, déduit de l'historique
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param nbDepla Adresse du nombre de déplacements
 * @param histoDepla Historique des déplacements
 */
void anuulation_deplacer(t_Plateau plateau, int * ligSok, int * colSok, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Annule le dernier déplacement effectué en fonction de l'historique
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne actuelle de Sokoban
 * @param colSok Adresse de la colonne actuelle de Sokoban
 * @param nbDepla Adresse du compteur de déplacements
 * @param histoDepla Historique des déplacements
 */
void annulation_deplacer(t_Plateau plateau, int * ligSok, int * colSok, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Annule un déplacement simple sans caisse
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément inverse de ligne
 * @param incrColSok Incrément inverse de colonne
 * @param nbDepla Adresse du compteur de déplacements
 * @param histoDepla Historique des déplacements
 */
void annule_deplacement_rien(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Annule un déplacement où une caisse avait été poussée
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément inverse de ligne
 * @param incrColSok Incrément inverse de colonne
 * @param nbDepla Adresse du compteur de déplacements
 * @param histoDepla Historique des déplacements
 */
void annule_deplacement_caisse(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Annule un déplacement où une caisse sur cible avait été poussée
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément inverse de ligne
 * @param incrColSok Incrément inverse de colonne
 * @param nbDepla Adresse du compteur de déplacements
 * @param histoDepla Historique des déplacements
 */
void annule_deplacement_caisse_cible(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Annule un déplacement où Sokoban s'était déplacé sur une cible
 * @param plateau Plateau du jeu
 * @param ligSok Adresse de la ligne de Sokoban
 * @param colSok Adresse de la colonne de Sokoban
 * @param incrLigSok Incrément inverse de ligne
 * @param incrColSok Incrément inverse de colonne
 * @param nbDepla Adresse du compteur de déplacements
 * @param histoDepla Historique des déplacements
 */
void annule_deplacement_cible(t_Plateau plateau, int * ligSok, int * colSok, int incrLigSok, int incrColSok, int * nbDepla, t_tabDeplacement histoDepla);

/**
 * @brief Modifie les coordonnées de Sokoban lors d'une annulation
 * @param nvLig Variation inverse de ligne
 * @param nvCol Variation inverse de colonne
 * @param ligSok Adresse de la ligne actuelle de Sokoban
 * @param colSok Adresse de la colonne actuelle de Sokoban
 * @param nbDepla Adresse du compteur de déplacements
 */
void annulation_deplacement_sokoban(int nvLig, int nvCol, int * ligSok, int * colSok, int * nbDepla);

/**
 * @brief Affiche une ligne du plateau en zoom 1
 * @param plateau Plateau du jeu
 * @param zoom Niveau de zoom (1)
 * @param ligne Numéro de la ligne à afficher
 */
void plateaux1(t_Plateau plateau, int zoom, int ligne);

/**
 * @brief Affiche une ligne du plateau en zoom 2 ou 3
 * @param plateau Plateau du jeu
 * @param zoom Niveau de zoom (2 ou 3)
 * @param ligne Numéro de la ligne à afficher
 */
void plateaux2_3(t_Plateau plateau, int zoom, int ligne);

/**
 * @brief Initialise l'historique des déplacements
 * @param histoDepla Tableau des déplacements à réinitialiser
 */
void initialiser_historique_deplacement(t_tabDeplacement histoDepla);

/**
 * @brief Ajoute un déplacement simple à l'historique
 * @param histoDepla Tableau contenant l'historique des déplacements
 * @param dernierDepla Caractère représentant le déplacement effectué
 * @param nbDepla Index auquel ajouter ce déplacement
 */
void ajout_deplacement(t_tabDeplacement histoDepla, char dernierDepla, int nbDepla);

/**
 * @brief Modifie un déplacement pour indiquer qu'une caisse a été poussée
 * @param dernierDepla Adresse du déplacement à transformer
 */
void ajout_deplacement_caisse(char * dernierDepla);

/**
 * @brief Remplace le dernier déplacement de l'historique par un caractère vide
 * @param histoDepla Historique des déplacements
 * @param nbDepla Index du déplacement à annuler
 */
void annulation_deplacement(t_tabDeplacement histoDepla, int nbDepla);

/**
 * @brief Enregistre la suite de déplacements dans un fichier
 * @param t Tableau contenant les déplacements
 * @param nb Nombre de déplacements à enregistrer
 * @param fic Nom du fichier où sauvegarder
 */
void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]);


/**
 * @brief Fonction qui renvoie si le joueur a gagné
 * @param plateauDeJeu Plateau du jeu de type t_Plateau
 * @return true si le joueur a gagné sinon False
 */
bool gagne(t_Plateau plateauDeJeu);

int kbhit();

int main(){
    t_Plateau plateauDeJeu; // Plateau du jeu
    t_tabDeplacement historiqueDeplacement;
    char nomFichier[TAILLE_FICHIER], touche=ATTENTE; // Nom du fichier + touche
    int nbDeplacements=ZERO, ligneSokoban, colonneSokoban, nvZoom=1; 
    printf("Entrez le nom du fichier : ");
    scanf("%s", nomFichier);
    charger_partie(plateauDeJeu, nomFichier);
    affichier_entete(nbDeplacements,nomFichier);
    afficher_plateau(plateauDeJeu, nvZoom);
    // Définition des coordonnées où se trouve Sokoban
    for (int i=0 ; i<NB_LIGNES ; i++) {
        for(int j=0;j<NB_COLONNES;j++) {
            if((plateauDeJeu[i][j]==SOKOBAN) || (plateauDeJeu[i][j]==SOKOBAN_CIBLE)) {
                ligneSokoban=i;
                colonneSokoban=j;
            }
        }
    }
    initialiser_historique_deplacement(historiqueDeplacement);
    jeu(&touche, plateauDeJeu, nomFichier, ligneSokoban,
        colonneSokoban, nbDeplacements, nvZoom, historiqueDeplacement);
    // Dit si le joueur a gagné ou abandonné en fonction de la dernirèe touche
    if(touche == ARRETER) {
        abandon(plateauDeJeu);
        printf("\nLa partie a été abandonnée\n");
    } else {
        printf("\nVous avez gagné !\n");
    }
    return EXIT_SUCCESS;
}

void jeu(char *toucheAppuyee, t_Plateau plateau, char fichier[],
    int ligSok, int colSok, int nbDepla, int zoom,
    t_tabDeplacement histoDepla){
    
    // Boucle du jeu qui se termine si le joueur gagne ou abandonne
    while (*toucheAppuyee != ARRETER && !gagne(plateau)) {
        *toucheAppuyee = ATTENTE;
        while (kbhit() == 0) {}
        *toucheAppuyee = getchar();
        if (*toucheAppuyee == RECOMMENCER) {
            recommencer(&nbDepla, plateau, fichier, &ligSok, &colSok,
                histoDepla);
        } else if (*toucheAppuyee == RETOUR) {
            annulation_deplacer(plateau, &ligSok, &colSok, &nbDepla,
                histoDepla);
        } else if (*toucheAppuyee == ZOOM) {
            if(zoom < 3) {
                zoom++;
            }
        } else if (*toucheAppuyee == DEZOOM) {
            if(zoom > 1) {
                zoom=zoom-1;
            }
        }
        deplacer(plateau, &ligSok, &colSok, *toucheAppuyee, &nbDepla,
            histoDepla);

        system("clear");
        affichier_entete(nbDepla, fichier);
        afficher_plateau(plateau, zoom);
        for(int k=0; k<70 ; k++){
            printf("%c", histoDepla[k]);
        }
    }
}

void afficher_plateau(t_Plateau plateau, int zoom){
    for(int i = ZERO ; i < TAILLE ; i++) {
        if (zoom == ZOOM1) {
            plateaux1(plateau, zoom, i);
        } else {
            plateaux2_3(plateau, zoom, i);
        }
    }
}

void plateaux1(t_Plateau plateau, int zoom, int ligne){
    for(int j = ZERO ; j < TAILLE ; j++){
        if(plateau[ligne][j] == SOKOBAN_CIBLE) {
            printf("%c", SOKOBAN);
        } else if (plateau[ligne][j] == CAISSE_CIBLE) {
            printf("%c", CAISSE);
        } else {
            printf("%c", plateau[ligne][j]);
        }
    }
    printf("\n");
    
}

void plateaux2_3(t_Plateau plateau, int zoom, int ligne){
    if (zoom == ZOOM2) {
        for(int k = ZERO ; k < ZOOM2 ; k++){
            for(int j = ZERO ; j < TAILLE ; j++){
                if(plateau[ligne][j] == SOKOBAN_CIBLE) {
                    printf("%c", SOKOBAN);
                    printf("%c", SOKOBAN);      
                } else if (plateau[ligne][j] == CAISSE_CIBLE) {
                    printf("%c", CAISSE);
                    printf("%c", CAISSE);
                } else {
                    printf("%c", plateau[ligne][j]);
                    printf("%c", plateau[ligne][j]);
                }
            }
            printf("\n");
        }
    } else {
        for(int k = ZERO ; k < ZOOM3 ; k++){
            for(int j = ZERO ; j < TAILLE ; j++){
                if(plateau[ligne][j] == SOKOBAN_CIBLE) {
                    printf("%c", SOKOBAN);
                    printf("%c", SOKOBAN);
                    printf("%c", SOKOBAN);  
                } else if (plateau[ligne][j] == CAISSE_CIBLE) {
                    printf("%c", CAISSE);
                    printf("%c", CAISSE);
                    printf("%c", CAISSE);
                } else {
                    printf("%c", plateau[ligne][j]);
                    printf("%c", plateau[ligne][j]);
                    printf("%c", plateau[ligne][j]);
                }
            }
            printf("\n");
        }
    }
}

void affichier_entete(int nbDepla, char nomFich[]){
    // Affiche tous les éléments de l'en-tête
    printf("\nPartie : %s     Nombre de déplacements : %d\n\n",
        nomFich, nbDepla);
    printf("Actions disponibles :\n");
    printf("'z' = Aller en haut\n");
    printf("'s' = Aller en bas\n");
    printf("'q' = Aller à gauche\n");
    printf("'d' = Aller à droite\n");
    printf("'x' = Abandonner\n");
    printf("'r' = Recommencer la partie\n");
    printf("'+' = Zoomer\n");
    printf("'-' = Dézoomer\n");
    printf("'u' = Mouvement précédent\n\n");
}

void deplacer(t_Plateau plateau, int *ligSok, int *colSok,
    char deplacement, int *nbDepla, t_tabDeplacement histoDepla){

    int incrLigSok=ZERO, incrColSok=ZERO, doubleIncrLigSok=ZERO,
    doubleIncrColSok=ZERO;
    char deplacementHistorique;
    // incrLigSok et incrColSok changent en fonction de la direction choisie
    if(deplacement == HAUT) {
        incrLigSok = ENLEVER;
        deplacementHistorique = SOKOBAN_HAUT;
    } else if (deplacement == BAS) {
        incrLigSok = AJOUTER;
        deplacementHistorique = SOKOBAN_BAS;
    } else if (deplacement == GAUCHE) {
        incrColSok = ENLEVER;
        deplacementHistorique = SOKOBAN_GAUCHE;
    } else if (deplacement == DROITE) {
        incrColSok = AJOUTER;
        deplacementHistorique = SOKOBAN_DROITE;
    }
    // doubleIncrLigSok et doubleIncrColSok représentent la ligne/colonne plus loin
    doubleIncrLigSok = incrLigSok * DOUBLE;
    doubleIncrColSok = incrColSok * DOUBLE;
    // Le déplacement se fait en fonction du cas dans lequel le joueur se trouve
    if(plateau[*ligSok+incrLigSok][*colSok+incrColSok] == RIEN){
        deplacement_rien(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, &*nbDepla, histoDepla, deplacementHistorique);
    } else if (plateau[*ligSok+incrLigSok][*colSok+incrColSok] == CAISSE) {
        deplacement_caisse(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, doubleIncrLigSok, doubleIncrColSok, &*nbDepla,
            histoDepla, deplacementHistorique);
    } else if (plateau[*ligSok+incrLigSok][*colSok+incrColSok]
        == CAISSE_CIBLE) {
        deplacement_caisse_cible(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, doubleIncrLigSok, doubleIncrColSok, &*nbDepla,
            histoDepla, deplacementHistorique);
    } else if (plateau[*ligSok+incrLigSok][*colSok+incrColSok] == CIBLE) {
        deplacement_cible(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, &*nbDepla, histoDepla, deplacementHistorique);
    }
}

void deplacement_rien(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int *nbDepla, t_tabDeplacement histoDepla,
    char deplacement){

    // Si Sokoban se trouve sur une cible
    if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
        plateau[*ligSok][*colSok] = CIBLE;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
        deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
        
        ajout_deplacement(histoDepla, deplacement, *nbDepla);
    } else {
        plateau[*ligSok][*colSok] = RIEN;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
        deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
        
        ajout_deplacement(histoDepla, deplacement, *nbDepla);
    }
}

void deplacement_caisse(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int doubleIncrLigSok,
    int doubleIncrColSok, int *nbDepla, t_tabDeplacement histoDepla,
    char deplacement){
    
    // Si 2 cases plus loin il ne se trouve rien
    if (plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok] == RIEN){
        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
            plateau[*ligSok][*colSok] = CIBLE;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        } else {
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
            plateau[*ligSok][*colSok] = RIEN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        }
    // Sinon si 2 cases plus loin il se trouve une cible
    } else if (plateau[*ligSok+doubleIncrLigSok]
        [*colSok+doubleIncrColSok] == CIBLE) {

        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
            plateau[*ligSok][*colSok] = CIBLE;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        } else {
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
            plateau[*ligSok][*colSok] = RIEN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        }
    }
}

void deplacement_caisse_cible(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int doubleIncrLigSok,
    int doubleIncrColSok, int *nbDepla, t_tabDeplacement histoDepla,
    char deplacement){

    // Si 2 cases plus loin il ne se trouve rien
    if (plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok] == RIEN){
        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok]==SOKOBAN_CIBLE){
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
            plateau[*ligSok][*colSok] = CIBLE;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        } else {
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
            plateau[*ligSok][*colSok] = RIEN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        }
    // Sinon si 2 cases plus loin il se trouve une cible
    } else if (plateau[*ligSok+doubleIncrLigSok]
        [*colSok+doubleIncrColSok] == CIBLE) {
        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
            plateau[*ligSok][*colSok] = CIBLE;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);
                
            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        } else {
            plateau[*ligSok+doubleIncrLigSok]
            [*colSok+doubleIncrColSok] = CAISSE_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
            plateau[*ligSok][*colSok] = RIEN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
                &*colSok, &*nbDepla);

            ajout_deplacement_caisse(&deplacement);
            ajout_deplacement(histoDepla, deplacement, *nbDepla);
        }
    }
}

void deplacement_cible(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int *nbDepla, t_tabDeplacement histoDepla,
    char deplacement){
    
    // Si Sokoban se trouve sur une cible
    if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
        plateau[*ligSok][*colSok] = CIBLE;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
        deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);

        ajout_deplacement(histoDepla, deplacement, *nbDepla);
    } else {
        plateau[*ligSok][*colSok] = RIEN;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
        deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
        
        ajout_deplacement(histoDepla, deplacement, *nbDepla);
    }
}

void deplacement_sokoban(int nvLig, int nvCol, int *ligSok,
    int *colSok, int *nbDepla){
    
    // Effectue les changements dans les coordonnées de Sokoban
    *ligSok = *ligSok + nvLig;
    *colSok= *colSok + nvCol;
    *nbDepla= *nbDepla + 1;
}

void ajout_deplacement(t_tabDeplacement histoDepla, char dernierDepla,
    int nbDepla){
    
    histoDepla[nbDepla]=dernierDepla;
}

void ajout_deplacement_caisse(char *dernierDepla){
    if (*dernierDepla == SOKOBAN_GAUCHE) {
        *dernierDepla = SOKOBAN_CAISSE_GAUCHE;

    } else if (*dernierDepla == SOKOBAN_DROITE) {
        *dernierDepla = SOKOBAN_CAISSE_DROITE;

    } else if (*dernierDepla == SOKOBAN_HAUT) {
        *dernierDepla = SOKOBAN_CAISSE_HAUT;

    } else {
        *dernierDepla = SOKOBAN_CAISSE_BAS;
    }
}

void annulation_deplacer(t_Plateau plateau, int *ligSok, int *colSok,
    int *nbDepla, t_tabDeplacement histoDepla){

    int incrLigSok=ZERO, incrColSok=ZERO;
    // incrLigSok et incrColSok changent en fonction de la direction choisie
    if(histoDepla[*nbDepla] == SOKOBAN_HAUT) {
        incrLigSok = AJOUTER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_BAS) {
        incrLigSok = ENLEVER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_GAUCHE) {
        incrColSok = AJOUTER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_DROITE) {
        incrColSok = ENLEVER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_CAISSE_HAUT) {
        incrLigSok = ENLEVER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_CAISSE_BAS) {
        incrLigSok = AJOUTER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_CAISSE_GAUCHE) {
        incrColSok = ENLEVER;
    } else if (histoDepla[*nbDepla] == SOKOBAN_CAISSE_DROITE){
        incrColSok = AJOUTER;
    }

    // Le déplacement se fait en fonction du cas dans lequel le joueur se trouve
    if(plateau[*ligSok+incrLigSok][*colSok+incrColSok] == RIEN){
        annule_deplacement_rien(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, &*nbDepla, histoDepla);
    } else if (plateau[*ligSok+incrLigSok][*colSok+incrColSok] == CAISSE) {
        annule_deplacement_caisse(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, &*nbDepla, histoDepla);
    } else if (plateau[*ligSok+incrLigSok][*colSok+incrColSok]
        == CAISSE_CIBLE) {
        annule_deplacement_caisse_cible(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, &*nbDepla, histoDepla);
    } else if (plateau[*ligSok+incrLigSok][*colSok+incrColSok] == CIBLE) {
        annule_deplacement_cible(plateau, &*ligSok, &*colSok, incrLigSok,
            incrColSok, &*nbDepla, histoDepla);
    }
}

void annule_deplacement_rien(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int *nbDepla, t_tabDeplacement histoDepla){

    // Si Sokoban se trouve sur une cible
    if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
        plateau[*ligSok][*colSok] = CIBLE;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
        annulation_deplacement(histoDepla, *nbDepla);
        
        annulation_deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
    } else {
        plateau[*ligSok][*colSok] = RIEN;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN;
        annulation_deplacement(histoDepla, *nbDepla);
        
        annulation_deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
    }
}

void annule_deplacement_caisse(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int *nbDepla, t_tabDeplacement histoDepla){

    int invIncrColSok = incrColSok*INVERSE, invIncrLigSok = incrLigSok*INVERSE;
    
    // Si 2 cases plus loin il ne se trouve rien
    if (plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] == RIEN){
        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
            plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] = SOKOBAN;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = RIEN;
            plateau[*ligSok][*colSok] = CAISSE_CIBLE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        } else {
            plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] = SOKOBAN;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = RIEN;
            plateau[*ligSok][*colSok] = CAISSE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        }
    // Sinon si 2 cases plus loin il se trouve une cible
    } else if (plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] == CIBLE) {

        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
            plateau[*ligSok+invIncrLigSok]
            [*colSok+invIncrColSok] = SOKOBAN_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = RIEN;
            plateau[*ligSok][*colSok] = CAISSE_CIBLE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        } else {
            plateau[*ligSok+invIncrLigSok]
            [*colSok+invIncrColSok] = SOKOBAN_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = RIEN;
            plateau[*ligSok][*colSok] = CAISSE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        }
    }
}

void annule_deplacement_caisse_cible(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int *nbDepla, t_tabDeplacement histoDepla){

    int invIncrColSok = incrColSok*INVERSE, invIncrLigSok = incrLigSok*INVERSE;
    
    // Si 2 cases plus loin il ne se trouve rien
    if (plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] == RIEN){
        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok]==SOKOBAN_CIBLE){
            plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] = SOKOBAN;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = CIBLE;
            plateau[*ligSok][*colSok] = CAISSE_CIBLE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        } else {
            plateau[*ligSok+invIncrLigSok][*colSok+invIncrColSok] = SOKOBAN;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = CIBLE;
            plateau[*ligSok][*colSok] = CAISSE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        }
    // Sinon si 2 cases plus loin il se trouve une cible
    } else if (plateau[*ligSok+invIncrLigSok]
        [*colSok+invIncrColSok] == CIBLE) {
        // Si Sokoban se trouve sur une cible
        if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
            plateau[*ligSok+invIncrLigSok]
            [*colSok+invIncrColSok] = SOKOBAN_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = CIBLE;
            plateau[*ligSok][*colSok] = CAISSE_CIBLE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        } else {
            plateau[*ligSok+invIncrLigSok]
            [*colSok+invIncrColSok] = SOKOBAN_CIBLE;

            plateau[*ligSok+incrLigSok][*colSok+incrColSok] = CIBLE;
            plateau[*ligSok][*colSok] = CAISSE;
            annulation_deplacement(histoDepla, *nbDepla);
            annulation_deplacement_sokoban(invIncrLigSok, invIncrColSok,
                &*ligSok, &*colSok, &*nbDepla);
        }
    }
}

void annule_deplacement_cible(t_Plateau plateau, int *ligSok, int *colSok,
    int incrLigSok, int incrColSok, int *nbDepla, t_tabDeplacement histoDepla){
    
    // Si Sokoban se trouve sur une cible
    if (plateau[*ligSok][*colSok] == SOKOBAN_CIBLE){
        plateau[*ligSok][*colSok] = CIBLE;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
        annulation_deplacement(histoDepla, *nbDepla);
        
        annulation_deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
    } else {
        plateau[*ligSok][*colSok] = RIEN;
        plateau[*ligSok+incrLigSok][*colSok+incrColSok] = SOKOBAN_CIBLE;
        annulation_deplacement(histoDepla, *nbDepla);
        
        annulation_deplacement_sokoban(incrLigSok, incrColSok, &*ligSok,
            &*colSok, &*nbDepla);
    }
}

void annulation_deplacement_sokoban(int nvLig, int nvCol, int *ligSok,
    int *colSok, int *nbDepla){
    
    // Effectue les changements dans les coordonnées de Sokoban
    *ligSok = *ligSok + nvLig;
    *colSok = *colSok + nvCol;
    *nbDepla = *nbDepla - 1;
}

void annulation_deplacement(t_tabDeplacement histoDepla,  int nbDepla){
    
    histoDepla[nbDepla]=AUCUN_DEPLACEMENT;
}


void initialiser_historique_deplacement(t_tabDeplacement histoDepla){
    for (int i = ZERO ; i < MILLE ; i++){
        histoDepla[i]=AUCUN_DEPLACEMENT;
    }
}

void recommencer(int *nbDepla, t_Plateau plateauDeJeu, char nomFichier[],
    int *ligneSokoban, int *colonneSokoban, t_tabDeplacement histoDepla){

    char choix;
    printf("Êtes-vous sûr de vouloir recommencer ? (O/N) ");
    scanf("%c", &choix);
    // Regarde si le joueur a choisi de valider de recommencer
    if (choix == VALIDATION) {
        charger_partie(plateauDeJeu, nomFichier);
        *nbDepla=0;
        for (int i=0 ; i<NB_LIGNES ; i++) {
            for (int j=0 ; j<NB_COLONNES ; j++) {
                if (plateauDeJeu[i][j] == SOKOBAN){
                    *ligneSokoban = i;
                    *colonneSokoban = j;
                }
            }
        }
        initialiser_historique_deplacement(histoDepla);
    }
}

void abandon(t_Plateau plateauDeJeu){
    char choix, nomNvFichier[TAILLE_FICHIER];
    printf("Souhaitez-vous enregistrer la partie ? (O/N) ");
    scanf("%c", &choix);
    // Regarde si le joueur a choisi de sauvegarder
    if (choix == VALIDATION){
        printf("Quel nom au fichier ? (15 caractères maximum) ");
        scanf("%s", nomNvFichier);
        enregistrer_partie(plateauDeJeu, nomNvFichier);
    }
}

bool gagne(t_Plateau plateauDeJeu){
    bool victoire = true;
    // Regarde si il reste des caisses ou Sokoban sur une cible
    for (int i=0 ; i<NB_LIGNES ; i++){
        for (int j=0 ; j<NB_COLONNES ; j++){
            if (plateauDeJeu[i][j] == CAISSE
                || plateauDeJeu[i][j] == SOKOBAN_CIBLE){
                victoire = false;
            }
        }
    }
    return victoire;
}


void charger_partie(t_Plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}



void enregistrer_partie(t_Plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne='\n';

    f = fopen(fichier, "w");
    for (int ligne=0 ; ligne<TAILLE ; ligne++){
        for (int colonne=0 ; colonne<TAILLE ; colonne++){
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}


int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]){
    FILE * f;

    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}