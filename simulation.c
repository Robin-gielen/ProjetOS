#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>

#define KEY 12345
/**int numVoits[20];
int comptVoitActuel = 0;
numVoits[0] = 15;
numVoits[1] = 23;
numVoits[2] = 26;
numVoits[3] = 28;
numVoits[4] = 32;
numVoits[5] = 39;
numVoits[6] = 40;
numVoits[7] = 46;
numVoits[8] = 48;
numVoits[9] = 51;
numVoits[10] = 54;
numVoits[11] = 59;
numVoits[12] = 64;
numVoits[13] = 65;
numVoits[14] = 68;
numVoits[15] = 70;
numVoits[16] = 77;
numVoits[17] = 88;
numVoits[18] = 94;
numVoits[19] = 99;*/

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

	
	int mem_ID;
	void* ptr_mem_partagee;
	
	voiture maVoiture[10];
	maVoiture[5].greeting = 987654321;
	mem_ID = shmget(KEY, sizeof(maVoiture), 0666 | IPC_CREAT);
	if(mem_ID < 0) {
		perror("shmget");
		exit(1);
	}
	
	if((ptr_mem_partagee = shmat(mem_ID, NULL, 0)) == (void*) -1) {
		perror("shmat");
		exit(1);
	}
	
	int secteursMoyenne[3] = {40000, 50000, 45000};
	srand(time(NULL));   // should only be called once


	int a;
	for(a = 0; a < 10; a++) {
		maVoiture[a].bestTemps[0] = 70000;
		maVoiture[a].bestTemps[1] = 90000;
		maVoiture[a].bestTemps[2] = 80000;
		maVoiture[a].bestTemps[3] = 240000;
	}
	//printf("Voiture numéro :        1	 2	 3	 4	 5	 6	 7	 8 	 9 	 10\n");
	int j; 
	for(j=0; j<3; j++) {
		int k;
		for(k=0; k<10; k++) {
			sleep(1);
			int delai = rand()%10000; 
			maVoiture[k].tempsSecteur[j] = secteursMoyenne[j] - delai;
			if (maVoiture[k].tempsSecteur[j] < maVoiture[k].bestTemps[j]) {
				maVoiture[k].bestTemps[j] = maVoiture[k].tempsSecteur[j];
			}
		}
		//printf("temps secteur %d : 	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\n", j+1, maVoiture[0].tempsSecteur[j], maVoiture[1].tempsSecteur[j], maVoiture[2].tempsSecteur[j], maVoiture[3].tempsSecteur[j], maVoiture[4].tempsSecteur[j],maVoiture[5].tempsSecteur[j], maVoiture[6].tempsSecteur[j], maVoiture[7].tempsSecteur[j], maVoiture[8].tempsSecteur[j], maVoiture[9].tempsSecteur[j]);
		//fflush(stdout);
	memcpy(ptr_mem_partagee, &maVoiture, sizeof(voiture[10]));
	printf( ((voiture*)ptr_mem_partagee)[5].greeting);
	}
}
