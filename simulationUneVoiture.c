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
#define NBR_TOUR 10

typedef struct{
	int voitID;
	int tempsSecteur [3];
	int bestTemps [4];
	int tourActuel;
	int usurePneu;
	int tempsTotal;
	double probaPitStop;
	int nbrPitStop;
	double probaCrash;
	int nbrCrash;
	int tour;
} voiture;



void afficheSeparateur(int n) {
  int i;
   
  for (i = 0; i < n; i++)
    printf("+--------");
  puts("+");
}

void main(int argc, char* argv[]) {
	sem_t semaphores[NBR_VOIT];
	int a;
	for(a = 0; a < NBR_VOIT;a++) {
		sem_init(&semaphores[a], 0, 1);
	}
	int shm_ID;
	voiture* mesVoitures;
	
	char typeSimulation;
	if (argv[1][0] == 'p') {
		typeSimulation = 'p';
	}
	else if (argv[1][0] == 'q') {
		typeSimulation = 'q';
	}
	else if (argv[1][0] == 'f') {
		typeSimulation = 'f';
	}
	else {
		printf("Il n'y a pas d'arguments, veuillez entrer P, Q ou F");
		exit(0);
	}
	voiture maVoiture;
	shm_ID = shmget(KEY, 21*sizeof(maVoiture), 0666 | IPC_CREAT);
	if(shm_ID < 0) {
		perror("shmget");
		exit(1);
	}
	if((mesVoitures = shmat(shm_ID, NULL, 0)) == (void*) -1) {
		perror("shmat");
		exit(1);
	}
	int numerosDesVoitures[20] = {44, 77, 3, 33, 5, 7, 11, 31, 19, 18, 14, 2, 10, 55, 8, 20, 7, 30, 9, 94};
	int k;
	for(k = 0; k < NBR_VOIT; k++) {
		sleep(1);
		pid_t pid = fork();

		if(pid < 0) { //erreur
			printf("fork() failed");
		}
		else if (pid == 0) { //dans le fils

			int voitNum = k;
			voiture voituresSimulateur;
			voituresSimulateur.voitID = numerosDesVoitures[k];
			int secteursMoyenne[3] = {40000, 50000, 45000};
			srand(time(NULL));   // should only be called once
			voituresSimulateur.probaPitStop = 1;
			voituresSimulateur.nbrPitStop = 0;
			voituresSimulateur.probaCrash = 0;
			voituresSimulateur.nbrCrash = 0;
			voituresSimulateur.tour = 0;

			voituresSimulateur.bestTemps[0] = 70000;
			voituresSimulateur.bestTemps[1] = 90000;
			voituresSimulateur.bestTemps[2] = 80000;
			voituresSimulateur.bestTemps[3] = 240000;
			int m;

			for(m = 0; m < NBR_TOUR; m++) {

				sleep(5);
				if(rand()%((NBR_TOUR-1)*2) < voituresSimulateur.probaPitStop) { 
					voituresSimulateur.probaPitStop=0;
					voituresSimulateur.nbrPitStop += 1;
				}
				else {
					voituresSimulateur.probaPitStop += 1;
				}
				if(rand()%(1000) < voituresSimulateur.probaCrash) { 
					voituresSimulateur.probaCrash = 1;
					voituresSimulateur.nbrCrash += 1;
					exit(1);
				}
				else {
					voituresSimulateur.probaCrash += 1;
				}
				int l;
				sleep(2);
				int j; 
				for(j=0; j<3; j++) {
					sleep(1);
					int delai = rand()%10000; 
					voituresSimulateur.tempsSecteur[j] = (secteursMoyenne[j] - delai)/1000;
					if (voituresSimulateur.tempsSecteur[j] < voituresSimulateur.bestTemps[j]) {
						voituresSimulateur.bestTemps[j] = voituresSimulateur.tempsSecteur[j];
					}
				}
				voituresSimulateur.tour +=1;
				//SEMAPHORE
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
	if(typeSimulation == 'p') {
		//SEMAPHORE
		int b;
		for(b=0; b<NBR_VOIT;b++) {
			printf("en attente du sempahore %d \n", b);
			sem_wait(&semaphores[b]);
			
		}
		voiture* voituresAfficheur = mesVoitures;
		for(b=0; b<NBR_VOIT;b++) {
			sem_post(&semaphores[b]);
		}
		int classement[20];
		int z = 0;
		while(z < 40000) { 
			//Gestion de l'affichage du tableau
			printf("\033[2J");        //  Clear l'ecran
			printf("\033[H");         //  Positionne le curseur au dessus a gauche de la console
			system("cls");

			int i, j;
			afficheSeparateur(5);			     
			printf("|Voiture | Sect 1 | Sect 2 | Sect 3 |Temp Tot|\n");
			for (i = 0; i < NBR_VOIT; i++) {
			int tempTot = 0;
				afficheSeparateur(5);
			printf("| N°: %3d", voituresAfficheur[i].voitID);
			for (j = 0; j < 3; j++) {
				printf("|%3d sec ", voituresAfficheur[i].tempsSecteur[j]);
				tempTot += voituresAfficheur[i].tempsSecteur[j];
			}
			printf("|%3d sec pitStop:%d  crash:%d  tour n°: %d", tempTot, voituresAfficheur[i].nbrPitStop, voituresAfficheur[i].nbrCrash, voituresAfficheur[i].tour);
			puts("|");
			}
			afficheSeparateur(5);
			z++;				
		}
	}
	else if(typeSimulation == 'q') {
		//SEMAPHORE
		voiture* voituresAfficheur = mesVoitures;
		int elimine[5];
		int z = 0;
		while(z < 40000) { 
			//Gestion de l'affichage du tableau
			printf("\033[2J");        //  Clear l'ecran
			printf("\033[H");         //  Positionne le curseur au dessus a gauche de la console
			system("cls");

			int i, j;
			afficheSeparateur(5);			     
			printf("|Voiture | Sect 1 | Sect 2 | Sect 3 |Temp Tot|\n");
			for (i = 0; i < NBR_VOIT; i++) {
			int tempTot = 0;
				afficheSeparateur(5);
			printf("| N°: %3d", voituresAfficheur[i].voitID);
			for (j = 0; j < 3; j++) {
				printf("|%3d sec ", voituresAfficheur[i].tempsSecteur[j]);
				tempTot += voituresAfficheur[i].tempsSecteur[j];
			}
			printf("|%3d sec pitStop:%d  crash:%d  tour n°: %d", tempTot, voituresAfficheur[i].nbrPitStop, voituresAfficheur[i].nbrCrash, voituresAfficheur[i].tour);
			puts("|");
			}
			afficheSeparateur(5);
			z++;				
		}
	}
	else if(typeSimulation == 'f') {
		//SEMAPHORE
		voiture* voituresAfficheur = mesVoitures;
		int z = 0;
		while(z < 40000) { 
			//Gestion de l'affichage du tableau
			printf("\033[2J");        //  Clear l'ecran
			printf("\033[H");         //  Positionne le curseur au dessus a gauche de la console
			system("cls");

			int i, j;
			afficheSeparateur(5);			     
			printf("|Voiture | Sect 1 | Sect 2 | Sect 3 |Temp Tot|\n");
			for (i = 0; i < NBR_VOIT; i++) {
			int tempTot = 0;
				afficheSeparateur(5);
			printf("| N°: %3d", voituresAfficheur[i].voitID);
			for (j = 0; j < 3; j++) {
				printf("|%3d sec ", voituresAfficheur[i].tempsSecteur[j]);
				tempTot += voituresAfficheur[i].tempsSecteur[j];
			}
			printf("|%3d sec pitStop:%d  crash:%d  tour n°: %d", tempTot, voituresAfficheur[i].nbrPitStop, voituresAfficheur[i].nbrCrash, voituresAfficheur[i].tour);
			puts("|");
			}
			afficheSeparateur(5);
			z++;				
		}
	}
	shmdt(mesVoitures);
	struct shmid_ds *buf;
	shmctl(shm_ID, IPC_RMID, buf);
}

