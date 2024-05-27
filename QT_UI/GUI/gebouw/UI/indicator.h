#ifndef INDICATOR_H
#define INDICATOR_H

class QPaintDevice;

class Indicator
{
public:
    Indicator(int, int, bool);
    ~Indicator();
    void teken(QPaintDevice*);
    void setOpen();
    void setDicht();
private:
    int xCoordinaat;
    int yCoordinaat;
    bool status;
};

#endif // INDICATOR_H
