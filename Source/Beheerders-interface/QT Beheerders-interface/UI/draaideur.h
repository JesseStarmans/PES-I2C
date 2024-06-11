#include "deur.h"

#ifndef DRAAIDEUR_H
#define DRAAIDEUR_H

class Draaideur : public Deur
{
public:
    Draaideur(int, int, unsigned int, bool, bool);
    virtual ~Draaideur();
    void teken(QPaintDevice *);

private:
    bool liggend;
    bool rechtsom;
};

#endif // DRAAIDEUR_H
