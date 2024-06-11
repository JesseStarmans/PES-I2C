/*!
    @file draaideur.h
    @brief Header file voor de Draaideur klasse.

    Deze header file definieert de subklasse Draaideur voor het maken van een Draaideur.
*/
#include "deur.h"

#ifndef DRAAIDEUR_H
#define DRAAIDEUR_H

/*!
    @class Draaideur
    @brief Een klasse om een draaideur weer te geven in de interface.
*/
class Draaideur : public Deur
{
public:
    Draaideur(int, int, unsigned int, bool, bool);
    virtual ~Draaideur();
    void teken(QPaintDevice *);

private:
    bool liggend;       /*!< Definieert of de draaideur ligt in de open positie.*/
    bool rechtsom;      /*!< Definieert of de draaideur rechtsom opent.*/
};

#endif // DRAAIDEUR_H
