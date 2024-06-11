#include "schuifdeur.h"
#include "defines.h"
#include <QPaintDevice>
#include <QPainter>
#include <QPen>

Schuifdeur::Schuifdeur(int x, int y, unsigned int l):Deur(x,y,l) {

}

Schuifdeur::~Schuifdeur() {

}

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
