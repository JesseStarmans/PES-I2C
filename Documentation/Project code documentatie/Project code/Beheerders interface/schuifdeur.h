/*!
    @file schuifdeur.h
    @brief Header file voor de Schuifdeur klasse.

    Deze header file definieert de klasse voor het maken van een schuifdeur in de interface.
*/
#include "deur.h"

#ifndef SCHUIFDEUR_H
#define SCHUIFDEUR_H

/*!
    @class Schuifdeur
    @brief Een klasse om een schuifdeur in een interface weer te geven.
*/
class Schuifdeur : public Deur
{
public:
    Schuifdeur(int, int, unsigned int);
    virtual ~Schuifdeur();
    void teken(QPaintDevice *);
private:
};

#endif // SCHUIFDEUR_H
