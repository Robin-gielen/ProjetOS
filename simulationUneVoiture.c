#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>


#define KEY 12345


typedef struct{
	int voitID;
	int tempsSecteur [3];
	int usurePneu;
	int bestTemps [4];
	int tourActuel;
	int secteurActuel;
	int greeting; 
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
	(*mesVoitures).voitID = 123456;
	printf("valeur de *mesVoitures.voitID = %d\n", (voiture*)(*mesVoitures).voitID);
	fflush(stdout);
	int secteursMoyenne[3] = {40000, 50000, 45000};
	srand(time(NULL));   // should only be called once


	(*mesVoitures).bestTemps[0] = 70000;
	(*mesVoitures).bestTemps[1] = 90000;
	(*mesVoitures).bestTemps[2] = 80000;
	(*mesVoitures).bestTemps[3] = 240000;

	printf("Voiture numéro :    %d   \n", (*mesVoitures).voitID);
	int j; 
	for(j=0; j<3; j++) {
		int delai = rand()%10000; 
		(*mesVoitures).tempsSecteur[j] = (secteursMoyenne[j] - delai)/1000;
		if ((*mesVoitures).tempsSecteur[j] < (*mesVoitures).bestTemps[j]) {
			(*mesVoitures).bestTemps[j] = (*mesVoitures).tempsSecteur[j];
		}

		printf("temps secteur %d: 	%dsec	\n", j+1, (*mesVoitures).tempsSecteur[j]);
		fflush(stdout);
	//printf( ((voiture*)ptr_mem_partagee)[5].greeting);
	
	}
	shmdt(mesVoitures);
}

