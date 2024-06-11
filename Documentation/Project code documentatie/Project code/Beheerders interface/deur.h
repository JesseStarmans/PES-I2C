/*!
    @file deur.h
	@brief Header file voor de Deur hoofdklasse.

    Deze header file definieert de Deur hoofdklasse voor het maken van een deur.
*/
#ifndef DEUR_H
#define DEUR_H

#include <utility>

using namespace std;

class QPaintDevice;

/*!
    @class Deur
    @brief Een klasse om een deur weer te geven in de interface.
*/
class Deur
{
public:
    Deur(int, int, unsigned int);
    ~Deur();
    void open();
    void sluit();
    virtual void teken(QPaintDevice *) = 0;
    bool isDeurOpen();
    unsigned int deurLengte();
    std::pair<int,int> coordinaten();

private:
    bool status;            /*!< De status van de deur.*/
    int x_coordinaat;       /*!< De X-coordinaat van de deur.*/
    int y_coordinaat;       /*!< De Y-coordinaat van de deur.*/
    unsigned int lengte;    /*!< De lengte van de deur.*/
};

#endif // DEUR_H
