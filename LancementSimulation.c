#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>  /* Pour semget, semctl, semop */
#include <semaphore.h>

#include <unistd.h>

#define KEY 123
#define NBR_VOIT 20

typedef struct{
	int voitID;
	int tempsSecteur [4];
	int bestTemps [4];
	int tempsTotal;
	double probaPitStop;
	int nbrPitStop;
	double probaCrash;
	int nbrCrash;
	int tour;
	int eliminee;
} voiture;

//Fonction de gestion pour l'affichage des données de la course
void afficheSeparateur(int n) {
	int i;

	for (i = 0; i < n; i++)
		printf("+--------");
	puts("+");
}

//Fonction d'affichage d'espaces blancs

void printSpace(int n) {
	int i;
	for (i = 0; i < n+1; i++)
		printf(" ");
}

//Tri du tableau recupere dans la memPartagee, lors des phases de tests/qualifications
void triP(voiture  voituresAfficheur[]) {
	int d;
	int e;
	voiture tempVoiture;
	for(d=0; d<NBR_VOIT; d++){
		for(e=d+1; e<NBR_VOIT; e++) {
			if(voituresAfficheur[e].bestTemps[3] < voituresAfficheur[d].bestTemps[3]) {
				tempVoiture = voituresAfficheur[d];
				voituresAfficheur[d] = voituresAfficheur[e];
				voituresAfficheur[e] = tempVoiture;
			}
		}
	}
}
//Tri du tableau recupere dans la memPartagee, lors de la course finale
void triF(voiture  voituresAfficheur[]) {
	int d;
	int e;
	voiture tempVoiture;
	for(d=0; d<NBR_VOIT; d++){
		for(e=d+1; e<NBR_VOIT; e++) {
			if(voituresAfficheur[d].nbrCrash == 1) {
				tempVoiture = voituresAfficheur[d];
				voituresAfficheur[d] = voituresAfficheur[e];
				voituresAfficheur[e] = tempVoiture;
			}
			if(voituresAfficheur[e].tempsTotal < voituresAfficheur[d].tempsTotal) {
				tempVoiture = voituresAfficheur[d];
				voituresAfficheur[d] = voituresAfficheur[e];
				voituresAfficheur[e] = tempVoiture;
			}
		}
	}
}

//Le main, qui sera lance a l'appel du programme
void main(int argc, char* argv[]) {
	int NUMERO_ETAPE;
	NUMERO_ETAPE = atoi(argv[2]);

	int NBR_TOUR;
	NBR_TOUR = atoi(argv[3]);
	//Utilises pour recuperes les voitures eliminees a la qualif precedente
	int voitsElimineesIDCinq[5];
	int voitsElimineesIDDix[10];
	//La liste des numeroes des voitures 
	int numerosDesVoitures[20] = {44, 77, 3, 33, 5, 7, 11, 31, 19, 18, 14, 2, 10, 55, 8, 20, 27, 30, 9, 94};
	
	//Creation et initialisation des semaphores
	sem_t semaphores[NBR_VOIT];
	int a;
	for(a = 0; a < NBR_VOIT;a++) {
		sem_init(&semaphores[a], 0, 1);
	}
	int shm_ID;
	voiture* mesVoitures;
	
	
	//Récupération du type de la simulation des les arguments
	char TYPE_SIMULATION;
	if (argv[1][0] == 'p') {
		TYPE_SIMULATION = 'p';
	}
	else if (argv[1][0] == 'q') {
		TYPE_SIMULATION = 'q';
	}
	else if (argv[1][0] == 'f') {
		TYPE_SIMULATION = 'f';
	}
	else {
		printf("Il n'y a pas d'arguments, veuillez entrer p, q ou f");
		exit(0);
	}
	
	//Allouage de la zone de mémoire partagée
	voiture maVoiture;
	shm_ID = shmget(KEY, 24*sizeof(maVoiture), 0666 | IPC_CREAT);
	if(shm_ID < 0) {
		perror("shmget");
		exit(1);
	}
	if((mesVoitures = shmat(shm_ID, NULL, 0)) == (void*) -1) {
		perror("shmat");
		exit(1);
	}
	//Initialisation des voitures a 0
	int n;
	for(n=0; n<NBR_VOIT+4; n++) {
		mesVoitures[n].probaPitStop = 1;
		mesVoitures[n].nbrPitStop = 0;
		mesVoitures[n].probaCrash = 0;
		mesVoitures[n].nbrCrash = 0;
		mesVoitures[n].tour = 0;
		mesVoitures[n].tempsSecteur [0] = 0;
		mesVoitures[n].tempsSecteur [1] = 0;
		mesVoitures[n].tempsSecteur [2] = 0;
		mesVoitures[n].tempsSecteur [3] = 0;
		mesVoitures[n].bestTemps[0] = 0;
		mesVoitures[n].bestTemps[1] = 0;
		mesVoitures[n].bestTemps[2] = 0;
		mesVoitures[n].bestTemps[3] = 0;
		mesVoitures[n].eliminee = 0;
	}
	//Récupération des données dans le fichier passe en argument
	if(TYPE_SIMULATION == 'q') {
		if(NUMERO_ETAPE == 2) { //Dans le cas de la deuxieme qualif
			FILE *fichierResultatInitiaux;
			fichierResultatInitiaux = fopen(argv[4], "r");
			int voitsElimineesID[5];
			char numerosVoitsRecup[80];
			fgets(numerosVoitsRecup, 79, fichierResultatInitiaux);
			
			int u, count = 0;
			for(u=0; u<80;u++){
				if(count == 4 && numerosVoitsRecup[u+3] == '\0') {
					char temp1[2];
					temp1[0] = numerosVoitsRecup[u];
					temp1[1] = numerosVoitsRecup[u+1];
					voitsElimineesID[count] = atoi(temp1);	
					u=80;
				}
				else if(count == 4 && numerosVoitsRecup[u+2] == '\0') {
					int temp2 = numerosVoitsRecup[u] - '0';
					voitsElimineesID[count] = temp2;
					u=80;
				}
				if(numerosVoitsRecup[u+1] == ',' && count!=4) {
					int temp3 = numerosVoitsRecup[u] - '0';
					voitsElimineesID[count] = temp3;
					count++;
					u++;
				}
				else if (numerosVoitsRecup[u+2] == ',' && count!=4) {
					char temp4[2];
					temp4[0] = numerosVoitsRecup[u];
					temp4[1] = numerosVoitsRecup[u+1];
					voitsElimineesID[count] = atoi(temp4);	
					count++;
					u+=2;
				} 
				
			}
			int j;
			for(j=0; j<5; j++) {
				voitsElimineesIDCinq[j] = voitsElimineesID[j];
				printf("La voiture numero %d a ete eliminee precedement\n", voitsElimineesID[j]);
			}
			fclose(fichierResultatInitiaux);
		}
		else if(NUMERO_ETAPE == 3) { //Dans le cas de la deuxieme qualif
			FILE *fichierResultatInitiaux;
			fichierResultatInitiaux = fopen(argv[4], "r");
			int voitsElimineesID[10];
			char numerosVoitsRecup[80];
			
			fgets(numerosVoitsRecup, 79, fichierResultatInitiaux);
			
			int u, count = 0;
			for(u=0; u<80;u++){
				if(count == 9 && numerosVoitsRecup[u+3] == '\0') {
					char temp[2];
					temp[0] = numerosVoitsRecup[u];
					temp[1] = numerosVoitsRecup[u+1];
					voitsElimineesID[count] = atoi(temp);
					u=80;
				}
				else if(count == 9 && numerosVoitsRecup[u+2] == '\0') {
					int temp = numerosVoitsRecup[u] - '0';
					voitsElimineesID[count] = temp;
					u=80;
				}
				if(numerosVoitsRecup[u+1] == ',') {
					int temp = numerosVoitsRecup[u] - '0';
					voitsElimineesID[count] = temp;
					count++;
					u++;
				}
				else if (numerosVoitsRecup[u+2] == ',') {
					char temp[2];
					temp[0] = numerosVoitsRecup[u];
					temp[1] = numerosVoitsRecup[u+1];
					voitsElimineesID[count] = atoi(temp);		
					count++;
					u+=2;
				} 
			}
			int j;
			for(j=0; j<10; j++) {
				voitsElimineesIDDix[j] = voitsElimineesID[j];
				printf("La voiture numero %d a ete eliminee precedement\n", voitsElimineesID[j]);
			}
			fclose(fichierResultatInitiaux);
		}
	} else if (TYPE_SIMULATION == 'f') { //Dans le cas de la course finale
		FILE *fichierResultatInitiaux;
		fichierResultatInitiaux = fopen(argv[4], "r");
		int classement[20];
		char numerosVoitsRecup[160];
		fgets(numerosVoitsRecup, 159, fichierResultatInitiaux);
		
		int u, count = 0;
		for(u=0; u<160;u++){
			if(count == 19 && numerosVoitsRecup[u+3] == '\0') {
				char temp1[2];
				temp1[0] = numerosVoitsRecup[u];
				temp1[1] = numerosVoitsRecup[u+1];
				classement[count] = atoi(temp1);	
				u=160;
			}
			else if(count == 19 && numerosVoitsRecup[u+2] == '\0') {
				int temp2 = numerosVoitsRecup[u] - '0';
				classement[count] = temp2;
				u=160;
			}
			if(numerosVoitsRecup[u+1] == ',' && count!=19) {
				int temp3 = numerosVoitsRecup[u] - '0';
				classement[count] = temp3;
				count++;
				u++;
			}
			else if (numerosVoitsRecup[u+2] == ',' && count!=19) {
				char temp4[2];
				temp4[0] = numerosVoitsRecup[u];
				temp4[1] = numerosVoitsRecup[u+1];
				classement[count] = atoi(temp4);	
				count++;
				u+=2;
			}
			
		}
		int j;
		for(j=0; j<20; j++) {
			numerosDesVoitures[j] = classement[j];
			printf("La voiture numero %d démarre en position %d\n", classement[j],j+1);
		}
		fclose(fichierResultatInitiaux);
	}
	
	//On lance les processus fils, on attend 1 seconde avant chaque pour le srand() 
	int k;
	for(k = 0; k < NBR_VOIT; k++) {
		sleep(1);
		pid_t pid = fork();

		if(pid < 0) { //erreur
			printf("fork() failed");
		}
		else if (pid == 0) { //dans le fils
			voiture voituresSimulateur;
			voituresSimulateur.voitID = numerosDesVoitures[k];
			int secteursMoyenne[3] = {40000, 50000, 45000};
			srand(time(NULL));   // should only be called once per process
			voituresSimulateur.probaPitStop = 1;
			voituresSimulateur.nbrPitStop = 0;
			voituresSimulateur.probaCrash = 0;
			voituresSimulateur.nbrCrash = 0;
			voituresSimulateur.tour = 0;
			voituresSimulateur.tempsTotal = 0;

			voituresSimulateur.bestTemps[0] = 100000;
			voituresSimulateur.bestTemps[1] = 100000;
			voituresSimulateur.bestTemps[2] = 100000;
			voituresSimulateur.bestTemps[3] = 300000;
			if(TYPE_SIMULATION == 'q') {
				if(NUMERO_ETAPE == 2) {
					int l;
					for(l=0; l<5; l++) {
						//Si le numero de la voitures a ete elimine a la qualification precedente
						if(voitsElimineesIDCinq[l] == voituresSimulateur.voitID) {
							voituresSimulateur.tempsSecteur [0] = 999;
							voituresSimulateur.tempsSecteur [1] = 999;
							voituresSimulateur.tempsSecteur [2] = 999;
							voituresSimulateur.tempsSecteur [3] = 999;
							voituresSimulateur.bestTemps [0] = 998;
							voituresSimulateur.bestTemps [1] = 999;
							voituresSimulateur.bestTemps [2] = 999;
							voituresSimulateur.bestTemps [3] = 999;
							voituresSimulateur.eliminee = 1;
							sem_wait(&semaphores[k]);
							mesVoitures[k] = voituresSimulateur;
							sem_post(&semaphores[k]);
							exit(1);
						}
					}
				} else if(NUMERO_ETAPE == 3) {
					int l;
					for(l=0; l<10; l++) {
						//Si le numero de la voitures a ete elimine a la qualification precedente
						if(voitsElimineesIDDix[l] == voituresSimulateur.voitID) {
							voituresSimulateur.tempsSecteur [0] = 999;
							voituresSimulateur.tempsSecteur [1] = 999;
							voituresSimulateur.tempsSecteur [2] = 999;
							voituresSimulateur.tempsSecteur [3] = 999;
							voituresSimulateur.bestTemps [0] = 999;
							voituresSimulateur.bestTemps [1] = 999;
							voituresSimulateur.bestTemps [2] = 999;
							voituresSimulateur.bestTemps [3] = 999;
							voituresSimulateur.eliminee = 1;
							sem_wait(&semaphores[k]);
							mesVoitures[k] = voituresSimulateur;
							sem_post(&semaphores[k]);
							exit(1);
						}
					}
				}
			}
			int m;
			for(m = 0; m < NBR_TOUR; m++) {
				//Init du temps au tour a 0
				voituresSimulateur.tempsSecteur [3] = 0;
				sleep(4);
				//Gestion de l'arret aux stands
				if(rand()%((NBR_TOUR-1)*2) < voituresSimulateur.probaPitStop) { 
					voituresSimulateur.probaPitStop=0;
					voituresSimulateur.nbrPitStop += 1;
					voituresSimulateur.tempsSecteur [3] += 3;
				}
				else {
					voituresSimulateur.probaPitStop += 1;
				}
				//Gestion des crashs
				if(rand()%(1000) < voituresSimulateur.probaCrash) { 
					voituresSimulateur.probaCrash = 1;
					sem_wait(&semaphores[k]);
					mesVoitures[k].nbrCrash = 1;
					sem_post(&semaphores[k]);
					exit(1);
				}
				else {
					voituresSimulateur.probaCrash += 1;
				}
				
				int l;
				int j; 
				//Boucles pour simuler chacun des secteurs
				for(j=0; j<3; j++) {
					sleep(1); 
					int delai = rand()%10000; //Le delai qui simule le temps mis par la voiture pour un secteur (random)
					voituresSimulateur.tempsSecteur[j] = (secteursMoyenne[j] - delai)/1000; 
					voituresSimulateur.tempsSecteur[3] += voituresSimulateur.tempsSecteur[j];
					//Acces a la mem partagee pour ecriture du temps de secteur
					sem_wait(&semaphores[k]);
					mesVoitures[k].tempsSecteur[j] = voituresSimulateur.tempsSecteur[j];
					sem_post(&semaphores[k]);
					if (voituresSimulateur.tempsSecteur[j] < voituresSimulateur.bestTemps[j]) {
						voituresSimulateur.bestTemps[j] = voituresSimulateur.tempsSecteur[j];
					}
				}
				voituresSimulateur.tempsTotal += voituresSimulateur.tempsSecteur[3];
				if (voituresSimulateur.tempsSecteur[3] < voituresSimulateur.bestTemps[3]){
					voituresSimulateur.bestTemps[3] = voituresSimulateur.tempsSecteur[3];
				}
				voituresSimulateur.tour +=1;
				//Acces a la mem partagee pour mettre a jour toutes les donnees de la voiture
				sem_wait(&semaphores[k]);
				mesVoitures[k] = voituresSimulateur;
				sem_post(&semaphores[k]);
				
			}	

			exit(1);
		}
		else {
			
		}
	}
	//Gère l'étape de la course à laquelle on est
	if(TYPE_SIMULATION == 'p') {
		int classement[NBR_VOIT];
		int afficheGoOn[NBR_VOIT];
		int affiche = 1;	
		int nextHopStop = 0;
		int h;
		//reset du tableau de bool qui gere la continuite de l'affichage 
		for(h=0; h<NBR_VOIT; h++) {
			afficheGoOn[h] = 1;
		}
		while(affiche == 1) { 
			//Gestion de l'affichage du tableau
			printf("\033[2J");        //  Clear l'ecran
			printf("\033[H");         //  Positionne le curseur au dessus a gauche de la console
			system("cls");
			voiture voituresAfficheur[NBR_VOIT];
			int b;
			//Acces a la mem partagee pour recupere les donnees des voitures
			for(b=0; b<NBR_VOIT;b++) {
				sem_wait(&semaphores[b]);
				voituresAfficheur[b] = mesVoitures[b];
				sem_post(&semaphores[b]);
			}
			//appel de la fonction de tri du tableau
			triP(voituresAfficheur);
			//Gestion de l'affichage de toutes les donnees pour la course
			int i, j;
			afficheSeparateur(12);			     
			printf("|Voiture | Sect 1 | Sect 2 | Sect 3 | Total  |M Sect 1|M Sect 2|M Sect 3|M Total |Pit Stop| Crash  |Nbr Tour|\n");
			for (i = 0; i < NBR_VOIT; i++) {
				int tempTot = 0;
				afficheSeparateur(12);
				printf("| N°: %3d", voituresAfficheur[i].voitID);
				for (j = 0; j < 4; j++) {
					printf("|%3d sec ", voituresAfficheur[i].tempsSecteur[j]);
				}
				for (j = 0; j < 4; j++) {
					printf("|%3d sec ", voituresAfficheur[i].bestTemps[j]);
				}
				printf("|   %d    |   %d    |   %d    ", voituresAfficheur[i].nbrPitStop, voituresAfficheur[i].nbrCrash, voituresAfficheur[i].tour);
				puts("|");
			}
			afficheSeparateur(12);

			//Affichage des meilleurs temps/secteur et temps/tour
			int q, tempVoitID, tempBestTemps;
			printf("Meilleur temps secteur 1 :\n");
			afficheSeparateur(5);
			tempBestTemps = 998;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[0] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[0];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printf("%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);
			
			printf("Meilleur temps secteur 2 :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[1] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[1];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printSpace(7);
			printf("|%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);
			
			printf("Meilleur temps secteur 3 :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[2] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[2];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printSpace(16);
			printf("|%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);

			printf("Meilleur temps au tour :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[3] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[3];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printSpace(25);
			printf("|%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);

			//Verification pour savoir si on doit continuer d'afficher
			int z;
			for(z = 0; z<NBR_VOIT; z++) {
				if(voituresAfficheur[z].tour == NBR_TOUR){
					afficheGoOn[z] = 0;
				}
				else if(voituresAfficheur[z].nbrCrash == 1) {
					afficheGoOn[z] = 0;
				}
				else afficheGoOn[z] = 1;
			}
			if(nextHopStop == 1) {
				affiche = 0;
			}
			int y;
			int tempAfficheGoOn = 0;
			for(y=0; y<NBR_VOIT; y++) {
				if(afficheGoOn[y] == 1) {
					tempAfficheGoOn = 1;
				}
			}
			if(tempAfficheGoOn == 0) {
				int f;
				for(f=0; f<NBR_VOIT; f++) {
					classement[f] = voituresAfficheur[f].voitID;
				}
				nextHopStop = 1;
			}
		}
	}
	else if(TYPE_SIMULATION == 'q') {
		int classement[NBR_VOIT];
		int afficheGoOn[NBR_VOIT];
		int affiche = 1;
		int nextHopStop = 0;
		int h;
		for(h=0; h<NBR_VOIT; h++) {
			afficheGoOn[h] = 1;
		}
		while(affiche == 1) { 
			//Gestion de l'affichage du tableau
			printf("\033[2J");        //  Clear l'ecran
			printf("\033[H");         //  Positionne le curseur au dessus a gauche de la console
			system("cls");
			voiture voituresAfficheur[NBR_VOIT];
			int b;
			//Acces a la mem partagee pour recupere les donnees des voitures
			for(b=0; b<NBR_VOIT;b++) {
				sem_wait(&semaphores[b]);
				voituresAfficheur[b] = mesVoitures[b];
				sem_post(&semaphores[b]);
			}
			//appel de la fonction de tri du tableau
			triP(voituresAfficheur);
			//Gestion de l'affichage de toutes les donnes de la course
			int i, j;
			afficheSeparateur(12);			     
			printf("|Voiture | Sect 1 | Sect 2 | Sect 3 | Total  |M Sect 1|M Sect 2|M Sect 3|M Total |Pit Stop| Crash  |Nbr Tour|\n");
			for (i = 0; i < NBR_VOIT; i++) {
				int tempTot = 0;
				afficheSeparateur(12);
				printf("| N°: %3d", voituresAfficheur[i].voitID);
				for (j = 0; j < 4; j++) {
					printf("|%3d sec ", voituresAfficheur[i].tempsSecteur[j]);
				}
				for (j = 0; j < 4; j++) {
					printf("|%3d sec ", voituresAfficheur[i].bestTemps[j]);
				}
				printf("|   %d    |   %d    |   %d    ", voituresAfficheur[i].nbrPitStop, voituresAfficheur[i].nbrCrash, voituresAfficheur[i].tour);
				puts("|");
			}
			afficheSeparateur(12);

			if(NUMERO_ETAPE == 2) {
				//Affichage des meilleurs temps/secteur et temps/tour
				int q, tempVoitID, tempBestTemps;
				printf("Meilleur temps secteur 1 :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-5;q++) {
					if(voituresAfficheur[q].bestTemps[0] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[0];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printf("%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);
			
				printf("Meilleur temps secteur 2 :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-5;q++) {
					if(voituresAfficheur[q].bestTemps[1] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[1];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printSpace(7);
				printf("|%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);
			
				printf("Meilleur temps secteur 3 :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-5;q++) {
					if(voituresAfficheur[q].bestTemps[2] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[2];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printSpace(16);
				printf("|%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);

				printf("Meilleur temps au tour :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-5;q++) {
					if(voituresAfficheur[q].bestTemps[3] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[3];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printSpace(25);
				printf("|%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);
			}
			else if (NUMERO_ETAPE == 3) {
				//Affichage des meilleurs temps/secteur et temps/tour
				int q, tempVoitID, tempBestTemps;
				printf("Meilleur temps secteur 1 :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-10;q++) {
					if(voituresAfficheur[q].bestTemps[0] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[0];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printf("%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);
			
				printf("Meilleur temps secteur 2 :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-10;q++) {
					if(voituresAfficheur[q].bestTemps[1] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[1];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printSpace(7);
				printf("|%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);
			
				printf("Meilleur temps secteur 3 :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-10;q++) {
					if(voituresAfficheur[q].bestTemps[2] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[2];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printSpace(16);
				printf("|%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);

				printf("Meilleur temps au tour :\n");
				afficheSeparateur(5);
				tempBestTemps = 999;
				for(q=0;q<NBR_VOIT-10;q++) {
					if(voituresAfficheur[q].bestTemps[3] < tempBestTemps){
						tempBestTemps = voituresAfficheur[q].bestTemps[3];
						tempVoitID = voituresAfficheur[q].voitID;
					}
				}
				printf("| N°: %3d|",tempVoitID);
				printSpace(25);
				printf("|%3d sec |\n",tempBestTemps);
				afficheSeparateur(5);
			}
			

			//Gestion de la continuite d'affichage 
			int z;
			for(z = 0; z<NBR_VOIT; z++) {
				if(voituresAfficheur[z].tour == NBR_TOUR){
					afficheGoOn[z] = 0;
				}
				else if(voituresAfficheur[z].nbrCrash == 1) {
					afficheGoOn[z] = 0;
				}
				else if(voituresAfficheur[z].eliminee == 1) {
					afficheGoOn[z] = 0;
				}
				else afficheGoOn[z] = 1;
			}
			int y;
			int tempAfficheGoOn = 0;
			for(y=0; y<NBR_VOIT; y++) {
				if(afficheGoOn[y] == 1) {
					tempAfficheGoOn = 1;
				}
			}
			if(nextHopStop == 1) {
				affiche = 0;
			}
			if(tempAfficheGoOn == 0) {
				int f;
				for(f=0; f<NBR_VOIT; f++) {
					classement[f] = voituresAfficheur[f].voitID;
				}
				if(NUMERO_ETAPE == 1) {
					int classementToSave[5];
					int i;
					for(i=0; i<5; i++) {
						classementToSave[i] = classement[15+i];
					}
					FILE *fichierResultat;
					fichierResultat = fopen(argv[5], "w");
					char str[50];
					char temp[12];
					int h;
					for(h=0; h < 4; h++) { 
						sprintf(temp, "%d", classementToSave[h]);
						strcat(str, temp);
						strcat(str, ",");
					}
					sprintf(temp, "%d", classementToSave[4]);
					strcat(str, temp);
					strcat(str, "\0");
					strcat(str, "\n");
					fprintf(fichierResultat, str);
					fclose(fichierResultat);
					nextHopStop = 1;
				}
				else if (NUMERO_ETAPE == 2) {
					int classementToSave[10];
					int i;
					for(i=0; i<10; i++) {
						classementToSave[i] = classement[10+i];
					}
					FILE *fichierResultat;
					fichierResultat = fopen(argv[5], "w");
					char str[50];
					char temp[12];
					int h;
					for(h=0; h < 9; h++) { 
						sprintf(temp, "%d", classementToSave[h]);
						strcat(str, temp);
						strcat(str, ",");
					}
					sprintf(temp, "%d", classementToSave[9]);
					strcat(str, temp);
					strcat(str, "\0");
					strcat(str, "\n");
					fprintf(fichierResultat, str);
					fclose(fichierResultat);
					nextHopStop = 1;
				}
				else if (NUMERO_ETAPE == 3) {
					int classementToSave[20];
					int i;
					for(i=0; i<20; i++) {
						classementToSave[i] = classement[i];
					}
					FILE *fichierResultat;
					fichierResultat = fopen(argv[5], "w");
					char str[50];
					char temp[12];
					int h;
					for(h=0; h < 19; h++) { 
						sprintf(temp, "%d", classementToSave[h]);
						strcat(str, temp);
						strcat(str, ",");
					}
					sprintf(temp, "%d", classementToSave[19]);
					strcat(str, temp);
					strcat(str, "\0");
					strcat(str, "\n");
					fprintf(fichierResultat, str);
					fclose(fichierResultat);
					nextHopStop = 1;
				}
			}
		}
	}
	else if(TYPE_SIMULATION == 'f') {
		int afficheGoOn[NBR_VOIT];
		int affiche = 1;
		int nextHopStop = 0; 
		int h;
		for(h=0; h<NBR_VOIT; h++) {
			afficheGoOn[h] = 1;
		}
		while(affiche == 1) { 
			//Gestion de l'affichage du tableau
			printf("\033[2J");        //  Clear l'ecran
			printf("\033[H");         //  Positionne le curseur au dessus a gauche de la console
			system("cls");
			voiture voituresAfficheur[NBR_VOIT];
			int b;
			//Acces a la mem partagee pour recupere les donnees des voitures
			for(b=0; b<NBR_VOIT;b++) {
				sem_wait(&semaphores[b]);
				voituresAfficheur[b] = mesVoitures[b];
				sem_post(&semaphores[b]);
			}
			//Appel de la fonction de tri du tableau 
			triF(voituresAfficheur);
			//Gestion de l'affichage de toutes les donnees des voitures pendant la course
			int i, j;
			afficheSeparateur(13);			     
			printf("|Voiture | Sect 1 | Sect 2 | Sect 3 | Total  |M Sect 1|M Sect 2|M Sect 3|M Total |Pit Stop| Crash  |Nbr Tour|Tmps Tot|\n");
			for (i = 0; i < NBR_VOIT; i++) {
				int tempTot = 0;
				afficheSeparateur(13);
				printf("| N°: %3d", voituresAfficheur[i].voitID);
				for (j = 0; j < 4; j++) {
					printf("|%3d sec ", voituresAfficheur[i].tempsSecteur[j]);
				}
				for (j = 0; j < 4; j++) {
					printf("|%3d sec ", voituresAfficheur[i].bestTemps[j]);
				}
				printf("|   %d    |   %d    |   %d    |%d sec ", voituresAfficheur[i].nbrPitStop, voituresAfficheur[i].nbrCrash, voituresAfficheur[i].tour, voituresAfficheur[i].tempsTotal);
				puts("|");
			}
			afficheSeparateur(13);

			//Affichage des meilleurs temps/secteur et temps/tour
			int q, tempVoitID, tempBestTemps;
			printf("Meilleur temps secteur 1 :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[0] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[0];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printf("%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);
			
			printf("Meilleur temps secteur 2 :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[1] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[1];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printSpace(7);
			printf("|%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);
			
			printf("Meilleur temps secteur 3 :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[2] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[2];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printSpace(16);
			printf("|%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);

			printf("Meilleur temps au tour :\n");
			afficheSeparateur(5);
			tempBestTemps = 999;
			for(q=0;q<NBR_VOIT;q++) {
				if(voituresAfficheur[q].bestTemps[3] < tempBestTemps){
					tempBestTemps = voituresAfficheur[q].bestTemps[3];
					tempVoitID = voituresAfficheur[q].voitID;
				}
			}
			printf("| N°: %3d|",tempVoitID);
			printSpace(25);
			printf("|%3d sec |\n",tempBestTemps);
			afficheSeparateur(5);

			//Verification pour savoir si on doit continuer d'afficher
			int z;
			for(z = 0; z<NBR_VOIT; z++) {
				if(voituresAfficheur[z].tour == NBR_TOUR){
					afficheGoOn[z] = 0;
				}
				else if(voituresAfficheur[z].nbrCrash == 1) {
					afficheGoOn[z] = 0;
				}
				else afficheGoOn[z] = 1;
			}
			int y;
			int tempAfficheGoOn = 0;
			for(y=0; y<NBR_VOIT; y++) {
				if(afficheGoOn[y] == 1) {
					tempAfficheGoOn = 1;
				}
			}
			if(nextHopStop == 1) {
				affiche = 0;
			}
			if(tempAfficheGoOn == 0) {
				int classementToSave[20][2];
				int i;
				for(i=0; i<20; i++) {
					classementToSave[i][0] = voituresAfficheur[i].voitID;
					classementToSave[i][1] = voituresAfficheur[i].bestTemps[3];
				}
				FILE *fichierResultat;
				fichierResultat = fopen(argv[5], "w");
				char str[200];
				str[0] = ' ';
				char temp[12];
				char tmp[12];
				int h;
				for(h=0; h < 19; h++) { 
					strcat(str, "VoitID : ");
					sprintf(temp, "%d", classementToSave[h][0]);
					strcat(str, temp);
					strcat(str, "\n");
					strcat(str, "     Meilleur temps : ");
					sprintf(tmp, "%d", classementToSave[h][1]);
					strcat(str, tmp);
					strcat(str, " secondes\n");
				}
				strcat(str, "VoitID : ");
				sprintf(temp, "%d", classementToSave[19][0]);
				strcat(str, temp);
				strcat(str, "\n");
				strcat(str, "     Meilleur temps : ");
				sprintf(tmp, "%d", classementToSave[19][1]);
				strcat(str, tmp);
				strcat(str, " secondes\n");
				fprintf(fichierResultat, str);
				fclose(fichierResultat);
				nextHopStop = 1;
			}
		}
	}
	//Destruction des ressources utilisees pour ne pas surcharger le systeme
	shmdt(mesVoitures);
	struct shmid_ds *buf;
	shmctl(shm_ID, IPC_RMID, buf);
}

