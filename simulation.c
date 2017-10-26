#include <stdio.h>
#include <stdlib.h>

#include <time.h>

typedef struct{
	int voitID;
	int tempsSecteur [3];
	int usurePneu;
	int bestTemps [4];
	int tourActuel;
	int secteurActuel;
} voiture;

void main() {
	int secteursMoyenne[3] = {40000, 50000, 45000};
	srand(time(NULL));   // should only be called once
	voiture maVoiture[10];

	int a;
	for(a = 0; a < 10; a++) {
		maVoiture[a].bestTemps[0] = 70000;
		maVoiture[a].bestTemps[1] = 90000;
		maVoiture[a].bestTemps[2] = 80000;
		maVoiture[a].bestTemps[3] = 240000;
	}
	printf("Voiture numéro :        1	 2	 3	 4	 5	 6	 7	 8 	 9 	 10\n");
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
		printf("temps secteur %d : 	%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\n", j+1, maVoiture[0].tempsSecteur[j], maVoiture[1].tempsSecteur[j], maVoiture[2].tempsSecteur[j], maVoiture[3].tempsSecteur[j], maVoiture[4].tempsSecteur[j],maVoiture[5].tempsSecteur[j], maVoiture[6].tempsSecteur[j], maVoiture[7].tempsSecteur[j], maVoiture[8].tempsSecteur[j], maVoiture[9].tempsSecteur[j]);
		fflush(stdout);
	}
}
