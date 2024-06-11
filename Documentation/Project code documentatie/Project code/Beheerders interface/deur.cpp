/*!
    @file deur.cpp
	@date 25 May 2024
	@author Jesse Starmans
	@brief CPP file voor de Deur hoofdklasse.

    De implementatie van de Deur hoofdklasse.

    Deze klasse is de hoofdklasse van de verschillende deuren.

    Created on 4 May 2024
    by Jesse Starmans. 
*/
#include "deur.h"
#include "defines.h"

/*!
    @brief Constructor voor de hoofdklasse Deur.

    Deze constructor wordt gebruikt door de andere deuren om de coordinaten en de lengte in op te slaan.

    @param x De X-coordinaat van waar de deur in de interface neergezet moet worden.
    @param y De Y-coordinaat van waar de deur in de interface neergezet moet worden.
    @param l De lengte van de deur.
*/
Deur::Deur(int x, int y, unsigned int l):status (OPEN), x_coordinaat(x), y_coordinaat(y), lengte (l) {

}

/*!
    @brief Destructor voor de hoofdklasse Deur.

    Deze destructor sloopt de deur.
*/
Deur::~Deur() {

}

/*!
    @brief Opent de deur.

    Deze functie opent de deur.
*/
void Deur::open() {
    status = OPEN;
}

/*!
    @brief Sluit de deur.

    Deze functie sluit de deur.
*/
void Deur::sluit() {
    status = DICHT;
}

/*!
    @brief Geeft de status van de deur terug.

    Deze functie returnt de status van de deur.

    @return De status van de deur.
*/
bool Deur::isDeurOpen() {
    return status;
}

/*!
    @brief Geeft de lengte van de deur terug.

    Deze functie returnt de lengte van de deur.

    @return De lengte van de deur.
*/
unsigned int Deur::deurLengte() {
    return lengte;
}

/*!
    @brief Geeft de coordinaten van de deur terug.

    Deze functie returnt de X en Y coordinaten van de deur als een pair<int,int>.

    @return De coordinaten van de deur.
*/
pair<int,int> Deur::coordinaten() {
    return make_pair(x_coordinaat, y_coordinaat);
}
