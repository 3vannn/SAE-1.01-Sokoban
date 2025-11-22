#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define NB_COLONNES 12
#define NB_LIGNES 12
const int ZERO=0;
const int TAILLE=12;
const int TAILLE_FICHIER=20;
const int ENLEVER=-1;
const int AJOUTER=1;
const int DOUBLE=2;
const char ARRETER='x';
const char RECOMMENCER='r';
const char GAUCHE='q';
const char DROITE='d';
const char HAUT='z';
const char BAS='s';
const char VALIDATION='O';
const char SOKOBAN='@';
const char MUR='#';
const char RIEN=' ';
const char CAISSE='$';
const char CIBLE='.';
const char SOKOBAN_CIBLE='+';
const char CAISSE_CIBLE='*';
const char ATTENTE='\0';

typedef char t_Plateau[NB_LIGNES][NB_COLONNES];

void afficher_plateau(t_Plateau plateau);
void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void affichier_entete(int nbDepla, char nomFich[]);
void deplacer(t_Plateau plateau, int *ligSok, int *colSok, char deplacement, int *nbDepla);
void deplacement_sokoban(int nvLig, int nvCol, int *ligSok, int *colSok, int *nbDepla);
void recommencer(int *nbDepla, t_Plateau plateauDeJeu, char nomFichier[], int *ligneSokoban, int *colonneSokoban);
void abandon(t_Plateau plateauDeJeu);
void deplacement_sokoban_cible(t_Plateau plateau, int *ligSok, int *colSok, char deplacement, int incrLigSok, int incrColSok, int doubleIncrLigSok, int doubleIncrColSok);
void deplacement_caisse_cible(t_Plateau plateau, int *ligSok, int *colSok, char deplacement, int incrLigSok, int incrColSok, int doubleIncrLigSok, int doubleIncrColSok);

bool gagne(t_Plateau plateauDeJeu);
int kbhit();

int main(){
    t_Plateau plateauDeJeu;
    char nomFichier[TAILLE_FICHIER], touche=ATTENTE;
    int nbDeplacements=ZERO, ligneSokoban, colonneSokoban;
    printf("Entrez le nom du fichier : ");
    scanf("%s", nomFichier);
    charger_partie(plateauDeJeu, nomFichier);
    affichier_entete(nbDeplacements,nomFichier);
    afficher_plateau(plateauDeJeu);
    while(kbhit()==0){}
    touche = getchar();
    for(int i=0;i<NB_LIGNES;i++){
        for(int j=0;j<NB_COLONNES;j++){
            if(plateauDeJeu[i][j]==SOKOBAN){
                ligneSokoban=i;
                colonneSokoban=j;
            }
        }
    }
    while(touche!=ARRETER && !gagne(plateauDeJeu)){
        if(touche==RECOMMENCER){
            recommencer(&nbDeplacements, plateauDeJeu, nomFichier, &ligneSokoban, &colonneSokoban);
        }
        deplacer(plateauDeJeu, &ligneSokoban, &colonneSokoban, touche, &nbDeplacements);

        system("clear");
        affichier_entete(nbDeplacements,nomFichier);
        afficher_plateau(plateauDeJeu);
        touche = ATTENTE;
        while(kbhit()==0){}
        touche=getchar();
    }
    if(touche==ARRETER){
        abandon(plateauDeJeu);
        printf("\nLa partie a été abandonnée\n");
    }
    else{
        printf("\nVous avez gagné !\n");
    }
    return EXIT_SUCCESS;
}

void afficher_plateau(t_Plateau plateau){
    for(int i=ZERO; i<TAILLE; i++){
        for(int j=ZERO; j<TAILLE; j++){
            if(plateau[i][j]==SOKOBAN_CIBLE){
                printf("%c", SOKOBAN);
            }
            else if(plateau[i][j]==CAISSE_CIBLE){
                printf("%c", CAISSE);
            }
            else{
                printf("%c", plateau[i][j]);
            }
        }
        printf("\n");
    }
}

void affichier_entete(int nbDepla, char nomFich[]){
    printf("\nPartie : %s     Nombre de déplacements : %d\n\n", nomFich, nbDepla);
    printf("Actions disponibles :\n");
    printf("'z' = Aller en haut\n");
    printf("'s' = Aller en bas\n");
    printf("'q' = Aller à gauche\n");
    printf("'d' = Aller à droite\n");
    printf("'x' = Abandonner\n");
    printf("'r' = Recommencer la partie\n\n");
}

void deplacer(t_Plateau plateau, int *ligSok, int *colSok, char deplacement, int *nbDepla){
    int incrLigSok=ZERO, incrColSok=ZERO, doubleIncrLigSok=ZERO, doubleIncrColSok=ZERO;
    if(deplacement==HAUT){
        incrLigSok=ENLEVER;
    }
    else if(deplacement==BAS){
        incrLigSok=AJOUTER;
    }
    else if(deplacement==GAUCHE){
        incrColSok=ENLEVER;
    }
    else if(deplacement==DROITE){
        incrColSok=AJOUTER;
    }
    doubleIncrLigSok=incrLigSok*DOUBLE;
    doubleIncrColSok=incrColSok*DOUBLE;
    if(plateau[*ligSok+incrLigSok][*colSok+incrColSok]==RIEN){
        if(plateau[*ligSok][*colSok]==SOKOBAN_CIBLE){
            plateau[*ligSok][*colSok]=CIBLE;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
        }
        else{
            plateau[*ligSok][*colSok]=RIEN;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
        }
    }
    else if(plateau[*ligSok+incrLigSok][*colSok+incrColSok]==CAISSE){
        if(plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]==RIEN){
            plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]=CAISSE;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN;
            plateau[*ligSok][*colSok]=RIEN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
        }
        else if(plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]==CIBLE){
            if(plateau[*ligSok][*colSok]==SOKOBAN_CIBLE){
                plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]=CAISSE_CIBLE;
                plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN;
                plateau[*ligSok][*colSok]=CIBLE;
                deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
            }
            else{
                plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]=CAISSE_CIBLE;
                plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN;
                plateau[*ligSok][*colSok]=RIEN;
                deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
            }
        }
    }
    else if(plateau[*ligSok+incrLigSok][*colSok+incrColSok]==CAISSE_CIBLE){
        if(plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]==RIEN){
            plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]=CAISSE;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN_CIBLE;
            plateau[*ligSok][*colSok]=RIEN;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
        }
        else if(plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]==CIBLE){
            if(plateau[*ligSok][*colSok]==SOKOBAN_CIBLE){
                plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]=CAISSE_CIBLE;
                plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN_CIBLE;
                plateau[*ligSok][*colSok]=CIBLE;
                deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
            }
            else{
                plateau[*ligSok+doubleIncrLigSok][*colSok+doubleIncrColSok]=CAISSE_CIBLE;
                plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN_CIBLE;
                plateau[*ligSok][*colSok]=RIEN;
                deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
            }
        }
    }
    else if(plateau[*ligSok+incrLigSok][*colSok+incrColSok]==CIBLE){
        if(plateau[*ligSok][*colSok]==SOKOBAN_CIBLE){
            plateau[*ligSok][*colSok]=CIBLE;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN_CIBLE;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
        }
        else{
            plateau[*ligSok][*colSok]=RIEN;
            plateau[*ligSok+incrLigSok][*colSok+incrColSok]=SOKOBAN_CIBLE;
            deplacement_sokoban(incrLigSok, incrColSok, &*ligSok, &*colSok, &*nbDepla);
        }
    }
    
}

void deplacement_sokoban(int nvLig, int nvCol, int *ligSok, int *colSok, int *nbDepla){
    *ligSok=*ligSok+nvLig;
    *colSok=*colSok+nvCol;
    *nbDepla=*nbDepla+1;
}

void recommencer(int *nbDepla, t_Plateau plateauDeJeu, char nomFichier[], int *ligneSokoban, int *colonneSokoban){
    char choix;
    printf("Êtes-vous sûr de vouloir recommencer ? (O/N) ");
    scanf("%c", &choix);
    if(choix==VALIDATION){
        charger_partie(plateauDeJeu, nomFichier);
        *nbDepla=0;
        for(int i=0;i<NB_LIGNES;i++){
            for(int j=0;j<NB_COLONNES;j++){
                if(plateauDeJeu[i][j]==SOKOBAN){
                    *ligneSokoban=i;
                    *colonneSokoban=j;
                }
            }
        }
    }
}

void abandon(t_Plateau plateauDeJeu){
    char choix, nomNvFichier[TAILLE_FICHIER];
    printf("Souhaitez-vous enregistrer la partie ? (O/N) ");
    scanf("%c", &choix);
    if(choix==VALIDATION){
        printf("Quel nom souhaitez-vous donner au fichier ? (15 caractères maximum) ");
        scanf("%s", nomNvFichier);
        enregistrer_partie(plateauDeJeu, nomNvFichier);
    }
}

bool gagne(t_Plateau plateauDeJeu){
    bool victoire=true;
    for(int i=0;i<NB_LIGNES;i++){
        for(int j=0;j<NB_COLONNES;j++){
            if(plateauDeJeu[i][j]==CAISSE || plateauDeJeu[i][j]==SOKOBAN_CIBLE){
                victoire=false;
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