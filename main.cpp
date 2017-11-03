#ifdef _DEBUG
#include <vld.h> 
#endif
#include "typedefsandconsts.h"
#include "network_algs_cplex.h"
#include <conio.h>//For _getch()
int main() {
	for (int nnn = 0; nnn < 10; nnn++) {

		NAC *nac = new NAC;
		nac->readprob(nnn);
		delete nac;

		
		std::cout << "________________Problem " << nnn << " over___________________________ " << std::endl;
		//_getch();
	}
	_getch();
}