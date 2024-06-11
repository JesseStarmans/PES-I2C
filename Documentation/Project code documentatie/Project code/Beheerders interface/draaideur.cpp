/*!
    @file draaideur.cpp
    @date 25 May 2024
    @author Jesse Starmans
    @brief CPP file voor de Draaideur klasse

    De implementatie van de Draaideur klasse.

    Deze klasse maakt het mogelijk om een draaideur in de interface te plaatsen.

    Created on 4 May 2024
    by Jesse Starmans. 
*/
#include "draaideur.h"
#include "defines.h"
#include <QPaintDevice>
#include <QPainter>
#include <QPen>

/*!
    @brief Constructor voor de klasse Draaideur.

    Deze constructor wordt gebruikt voor het het maken van een Draaideur in de interface.

    @param x De X-coordinaat van waar de deur in de interface neergezet moet worden.
    @param y De Y-coordinaat van waar de deur in de interface neergezet moet worden.
    @param l De lengte van de deur.
    @param lig Definieert of de deur in de open positie liggend is of niet.
    @param r Definieert of de deur rechtsom sluit of niet.
*/
Draaideur::Draaideur(int x, int y, unsigned int l, bool lig, bool r):Deur(x,y,l), liggend(lig), rechtsom(r) {

}

/*!
    @brief Destructor voor de klasse Draaideur.

    Deze destructor sloopt de draaideur.
*/
Draaideur::~Draaideur() {

}

/*!
    @brief Tekent de draaideur in de interface.

    Deze functie tekent de draaideur in de interface. Er wordt gecheckt of de deur open of dicht is, de deur linksom of rechtsom sluit en of deze liggend open is of staand.
*/
void Draaideur::teken(QPaintDevice* tp) {
    QPainter p(tp);
    QColor kleur = Qt::black;
    p.setPen(QPen(kleur, 3));
    int x = coordinaten().first;
    int y = coordinaten().second;
    if (isDeurOpen() == OPEN) {
        if (liggend == true) {
            p.drawLine(x, y, x+deurLengte(), y);
        }
        else {
            p.drawLine(x, y, x, y-deurLengte());
        }
    }
    else {
        if (liggend == true) {
            if(rechtsom == true) {
                p.drawLine(x, y, x, y+deurLengte());
            }
            else {
                p.drawLine(x, y, x, y-deurLengte());
            }
        }
        else {
            if (rechtsom == true) {
                p.drawLine(x, y, x+deurLengte(), y);
            }
            else {
                p.drawLine(x, y, x-deurLengte(), y);
            }

        }
    }
}
