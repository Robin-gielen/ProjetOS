#include <stdio.h>
#include <stdlib.h>

#include <time.h>

void main() {

	srand(time(NULL));   // should only be called once
	int r = rand(); 
	int tempsSecteur;
	r =  r%2;
	sleep(2);
	int delai = rand()%10; 
	if (r = 0 ) {tempsSecteur = 40+ delai;}
	else if (r=1) {tempsSecteur = 40- delai;}

	printf("ajout/retrait : %d, tempssecteur : %d, delai : %d", r, tempsSecteur, delai);
	//ajout dans la table

}
