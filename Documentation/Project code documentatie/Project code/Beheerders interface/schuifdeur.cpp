/*!
    @file schuifdeur.cpp
    @date 25 May 2024
    @author Jesse Starmans
    @brief CPP file voor de Schuifdeur klasse.

    De implementatie van de Schuifdeur klasse in QT.

    Deze klasse functioneert als een schuifdeur in de QT beheerdersinterface.

    Created on 4 May 2024
    by Jesse Starmans. \n
*/
#include "schuifdeur.h"
#include "defines.h"
#include <QPaintDevice>
#include <QPainter>
#include <QPen>

/*!
    @brief Constructor voor de klasse Schuifdeur.

    Deze constructor maakt een Schuifdeur object aan.

    @param x De X-coordinaat van waar de deur in de interface neergezet moet worden.
    @param y De Y-coordinaat van waar de deur in de interface neergezet moet worden.
    @param l De lengte van de deur.
*/
Schuifdeur::Schuifdeur(int x, int y, unsigned int l):Deur(x,y,l) {

}

/*!
    @brief Destructor voor de klasse Schuifdeur.

    Deze destructor sloopt het Schuifdeur object.
*/
Schuifdeur::~Schuifdeur() {

}

/*!
    @brief Tekent de schuifdeur in de interface.

    Deze functie tekent de schuifdeur in de interface.

    @param tp Painter om de deur weer te kunnen geven.
*/
void Schuifdeur::teken(QPaintDevice* tp) {
    QPainter p(tp);
    QColor kleur = Qt::black;
    p.setPen(QPen(kleur, 3));
    int x = coordinaten().first;
    int y = coordinaten().second;
    if (isDeurOpen() == OPEN) {
        p.drawLine(x, y, x, y+deurLengte());
    }
    else {
        p.drawLine(x, y, x, y-deurLengte());
    }
}
