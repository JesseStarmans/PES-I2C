#include "indicator.h"
#include "Defines.h"
#include <QPaintDevice>
#include <QPainter>
#include <QPen>

Indicator::Indicator(int x, int y, bool s): xCoordinaat(x), yCoordinaat(y), status(s), kleur(Qt::blue) {

}

Indicator::~Indicator() {

}

void Indicator::teken(QPaintDevice* tp) {
    QPainter p(tp);
    //QColor kleur;
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(Qt::SolidPattern);
    // if(status == OPEN)
    //     kleur=Qt::red;
    // else
    //     kleur=Qt::blue;

    p.setBrush(kleur);
    QPen pen(kleur,2,Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    p.drawEllipse(xCoordinaat, yCoordinaat,20,20);
}

void Indicator::setOpen() {
    status = OPEN;
}

void Indicator::setDicht() {
    status = DICHT;
}

void Indicator::setStatus(bool stat){
    status = stat;
}

void Indicator::setKleur(int r, int g, int b, int h){
    kleur = QColor(r, g, b, h);
}
