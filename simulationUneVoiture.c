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

typedef struct{
	int voitID;
	int tempsSecteur [3];
	int bestTemps [4];
	int tourActuel;
	int usurePneu;
	int tempsTotal;
} voiture;


void afficheSeparateur(int n) {
  int i;
   
  for (i = 0; i < n; i++)
    printf("+---");
  puts("+");
}
 

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


			mesVoitures[counter].bestTemps[0] = 70000;
			mesVoitures[counter].bestTemps[1] = 90000;
			mesVoitures[counter].bestTemps[2] = 80000;
			mesVoitures[counter].bestTemps[3] = 240000;
			int l;
			for(l=0; l<10; l++) {
				sleep(2);
				int j; 
				for(j=0; j<3; j++) {
					int delai = rand()%10000; 
					mesVoitures[counter].tempsSecteur[j] = (secteursMoyenne[j] - delai)/1000;
					if (mesVoitures[counter].tempsSecteur[j] < mesVoitures[counter].bestTemps[j]) {
						mesVoitures[counter].bestTemps[j] = mesVoitures[counter].tempsSecteur[j];
					}
				}
			}
			exit(1);
		}
		else {

		}
		counter++;
	}
	while(1) { //HEIGHT WIDTH
		int i, j;
		for (i = 0; i < NBR_VOIT; i++) {
			afficheSeparateur(3);
		for (j = 0; j < 3; j++) {
			printf("|%3d", mesVoitures[i].tempsSecteur[j]);
		}
		puts("|");
		}
		afficheSeparateur(3);

		/*printf("Voiture numéro :    %d   \n", mesVoitures[k].voitID);
		int j;
		for(j = 0; j < 3; j++) {
			printf("temps secteur %d: 	%dsec	\n", j+1, );
		}*/					
	}
	
	shmdt(mesVoitures);
	struct shmid_ds *buf;
	shmctl(shm_ID, IPC_RMID, buf);
}

