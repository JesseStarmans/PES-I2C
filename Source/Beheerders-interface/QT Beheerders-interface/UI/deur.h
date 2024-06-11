#ifndef DEUR_H
#define DEUR_H

#include <utility>

using namespace std;

class QPaintDevice;

class Deur
{
public:
    Deur(int, int, unsigned int);
    virtual ~Deur();
    void open();
    void sluit();
    virtual void teken(QPaintDevice *) = 0;
    bool isDeurOpen();
    unsigned int deurLengte();
    std::pair<int,int> coordinaten();

private:
    bool status;
    int x_coordinaat;
    int y_coordinaat;
    unsigned int lengte;
};

#endif // DEUR_H
