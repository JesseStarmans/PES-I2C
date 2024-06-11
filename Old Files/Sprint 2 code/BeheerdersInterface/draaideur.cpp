#include "draaideur.h"
#include "defines.h"
#include <QPaintDevice>
#include <QPainter>
#include <QPen>

Draaideur::Draaideur(int x, int y, unsigned int l, bool lig, bool r):Deur(x,y,l), liggend(lig), rechtsom(r) {

}

Draaideur::~Draaideur() {

}

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
