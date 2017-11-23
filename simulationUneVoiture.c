#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>

#include <unistd.h>

#define KEY 123


typedef struct{
	int voitID;
	int tempsSecteur [3];
	int usurePneu;
	int bestTemps [4];
	int tourActuel;
	int secteurActuel;
	int p1[50][3];
	int p2[50][3];
	int p3[50][3];
	int q1[50][3];
	int q2[50][3];
	int q3[50][3];
	int course[50][3];
} voiture;



void main() {

	
	int shm_ID;
	voiture* mesVoitures;
	
	voiture maVoiture;
	shm_ID = shmget(KEY, 20*sizeof(maVoiture), 0666 | IPC_CREAT);
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
	pid_t pid = fork();
	while(counter < 20) {
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

			printf("Voiture numéro :    %d   \n", mesVoitures[counter].voitID);
			int j; 
			for(j=0; j<3; j++) {
				int delai = rand()%10000; 
				mesVoitures[counter].tempsSecteur[j] = (secteursMoyenne[j] - delai)/1000;
				if (mesVoitures[counter].tempsSecteur[j] < mesVoitures[counter].bestTemps[j]) {
					mesVoitures[counter].bestTemps[j] = mesVoitures[counter].tempsSecteur[j];
				}

				printf("temps secteur %d: 	%dsec	\n", j+1, mesVoitures[counter].tempsSecteur[j]);
				//fflush(stdout);
			}
		}
		else {
			
		}
		sleep(1);
		counter++;
	}

	
	shmdt(mesVoitures);
	struct shmid_ds *buf;
	shmctl(shm_ID, IPC_RMID, buf);
}

