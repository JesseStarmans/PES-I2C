#include "deur.h"

#ifndef SCHUIFDEUR_H
#define SCHUIFDEUR_H

class Schuifdeur : public Deur
{
public:
    Schuifdeur(int, int, unsigned int);
    virtual ~Schuifdeur();
    void teken(QPaintDevice *);
private:
};

#endif // SCHUIFDEUR_H
