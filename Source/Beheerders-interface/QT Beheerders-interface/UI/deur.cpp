#include "deur.h"
#include "defines.h"

Deur::Deur(int x, int y, unsigned int l):status (DICHT), x_coordinaat(x), y_coordinaat(y), lengte (l) {

}

Deur::~Deur() {

}

void Deur::open() {
    status = OPEN;
}

void Deur::sluit() {
    status = DICHT;
}

bool Deur::isDeurOpen() {
    return status;
}

unsigned int Deur::deurLengte() {
    return lengte;
}

pair<int,int> Deur::coordinaten() {
    return make_pair(x_coordinaat, y_coordinaat);
}
