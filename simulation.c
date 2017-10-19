#include <stdio.h>
#include <stdlib.h>

#include <time.h>

typedef struct{
	int voitID;
	int tempsSecteur [3];
	int usurePneu;
} voiture;

void main() {
	
	int secteursMoyenne[3] = {35000, 45000, 40000};
	srand(time(NULL));   // should only be called once
	voiture maVoiture;
	int i;
	for(i = 0; i < 9; i++) {
		int j;
		for(j = 1; j < 4; j++) {
			int r = rand(); 
			r =  r%2;
			sleep(2);
			int delai = rand()%5000; 
			if (r = 0 ) {maVoiture.tempsSecteur[j-1] = secteursMoyenne[j-1] + delai;}
			else if (r =1) {maVoiture.tempsSecteur[j-1] = secteursMoyenne[j-1] - delai;}
		}
		printf("temps secteur 1 : %d \ntemps secteur 2 : %d \ntemps secteur 3 : %d \n", maVoiture.tempsSecteur[0], maVoiture.tempsSecteur[1], maVoiture.tempsSecteur[2]);
	}
	
}
