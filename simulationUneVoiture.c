#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>

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
 
/*void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}*/

void main() {
	
	/*if args[0] = p;
	else if args[0] = q;
	else if args[0] = c;
	*/
	int shm_ID;
	voiture* mesVoitures;
	
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

	int counter = 0;
	int k;
	for(k = 0; k < NBR_VOIT; k++) {
		sleep(1);
		pid_t pid = fork();
		if(pid < 0) { //erreur
			printf("fork() failed");
			//return 1;
		}
		else if (pid == 0) { //dans le fils
			int voitNum = counter;
			mesVoitures[counter].voitID = numerosDesVoitures[counter];
			int secteursMoyenne[3] = {40000, 50000, 45000};
			srand(time(NULL));   // should only be called once
			mesVoitures[counter].probaPitStop = 1;
			mesVoitures[counter].nbrPitStop = 0;
			mesVoitures[counter].probaCrash = 0;
			mesVoitures[counter].nbrCrash = 0;
			mesVoitures[counter].tour = 0;

			mesVoitures[counter].bestTemps[0] = 70000;
			mesVoitures[counter].bestTemps[1] = 90000;
			mesVoitures[counter].bestTemps[2] = 80000;
			mesVoitures[counter].bestTemps[3] = 240000;
			int m;
			for(m = 0; m < NBR_TOUR; m++) {
				sleep(1);
				if(rand()%((NBR_TOUR-1)*2) < mesVoitures[counter].probaPitStop) { 
					mesVoitures[counter].probaPitStop=0;
					mesVoitures[counter].nbrPitStop += 1;
				}
				else {
					mesVoitures[counter].probaPitStop += 1;
				}
				if(rand()%(1500) < mesVoitures[counter].probaCrash) { 
					mesVoitures[counter].probaCrash = 1;
					mesVoitures[counter].nbrCrash += 1;
					exit(1);
				}
				else {
					mesVoitures[counter].probaCrash += 1;
				}
				int l;
				sleep(2);
				int j; 
				for(j=0; j<3; j++) {
					sleep(1);
					int delai = rand()%10000; 
					mesVoitures[counter].tempsSecteur[j] = (secteursMoyenne[j] - delai)/1000;
					if (mesVoitures[counter].tempsSecteur[j] < mesVoitures[counter].bestTemps[j]) {
						mesVoitures[counter].bestTemps[j] = mesVoitures[counter].tempsSecteur[j];
					}
				}
				mesVoitures[counter].tour +=1;
			}	
			exit(1);
		}
		else {

		}
		counter++;
	}
	int z = 0;
	while(z < 40000) { //HEIGHT WIDTH
		/*
		Gère l'étape de la course à laquelle on est
		if(argv[1] == "p") {
			
		}
		else if(argv[1] == "q") {
			
		}
		else if(argv[1] == "f") {
			
		}*/

		system("cls");
		int i, j;
		afficheSeparateur(5);			     
		printf("|Voiture | Sect 1 | Sect 2 | Sect 3 |Temp Tot|\n");
		for (i = 0; i < NBR_VOIT; i++) {
		int tempTot = 0;
			afficheSeparateur(5);
		printf("| N°: %3d", mesVoitures[i].voitID);
		for (j = 0; j < 3; j++) {
			printf("|%3d sec ", mesVoitures[i].tempsSecteur[j]);
			tempTot += mesVoitures[i].tempsSecteur[j];
		}
		printf("|%3d sec pitStop:%d  crash:%d  tour n°: %d", tempTot, mesVoitures[i].nbrPitStop, mesVoitures[i].nbrCrash, mesVoitures[i].tour);
		puts("|");
		}
		afficheSeparateur(5);
		/*printf("Voiture numéro :    %d   \n", mesVoitures[k].voitID);
		int j;
		for(j = 0; j < 3; j++) {
			printf("temps secteur %d: 	%dsec	\n", j+1, );
		}*/
		z++;					
	}
	shmdt(mesVoitures);
	struct shmid_ds *buf;
	shmctl(shm_ID, IPC_RMID, buf);
}

