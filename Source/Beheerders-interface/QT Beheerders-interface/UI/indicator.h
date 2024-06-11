#ifndef INDICATOR_H
#define INDICATOR_H

#include <QPen>

class QPaintDevice;

class Indicator
{
public:
    Indicator(int, int, bool);
    ~Indicator();
    void teken(QPaintDevice*);
    void setOpen();
    void setDicht();
    void setStatus(bool);
    void setKleur(int, int, int, int);
private:
    QColor kleur;
    int xCoordinaat;
    int yCoordinaat;
    bool status;
};

#endif // INDICATOR_H
