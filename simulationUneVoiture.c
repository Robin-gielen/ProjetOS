#include <stdio.h>
#include <stdlib.h>

#include <time.h>

typedef struct{
	int voitID;
	int tempsSecteur [3];
	int usurePneu;
	int bestTemps [4];
} voiture;

void main() {

	int nbrTours = 10;

	int secteursMoyenne[3] = {40000, 50000, 45000};
	srand(time(NULL));   // should only be called once
	voiture maVoiture;
	int i;
	for(i=0; i<nbrTours; i++) {
		int j;
		for(j=0; j<3; j++) {
			sleep(2);
			int delai = rand()%10000; 
			maVoiture.tempsSecteur[j] = secteursMoyenne[j] - delai;
			& SEMAPHORE 
		}
	}
}
